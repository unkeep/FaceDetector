#include "CorePrivate.h"
#include <QThreadPool>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QPluginLoader>
#include <QFileInfo>
#include <QUuid>
#include <functional>
#include <QJsonObject>
#include <QJsonDocument>

using namespace FaceDetector;

CorePrivate::CorePrivate(QObject *parent)
    : QObject(parent),
      _algorithm(0),
      _supportedImages({"png", "jpg", "bmp"})
{
}

int CorePrivate::maxThreadCount() const
{
    return QThreadPool::globalInstance()->maxThreadCount();
}

void CorePrivate::setMaxThreadCount(int maxThreadCount)
{
    QThreadPool::globalInstance()->setMaxThreadCount(maxThreadCount);
}

QString CorePrivate::loadedAlgorithmPath() const
{
    return _algorithmPluginPath;
}

QString CorePrivate::inputDir() const
{
    return _inputDir;
}

void CorePrivate::setInputDir(const QString &inputDir)
{
    _inputDir = inputDir;
}

QString CorePrivate::outputDir() const
{
    return _outputDir;
}

void CorePrivate::setOutputDir(const QString &outputDir)
{
    _outputDir = outputDir;
}

bool CorePrivate::isStarted() const
{
    return !_futureWatcher.isNull() && _futureWatcher->isStarted();
}

bool CorePrivate::isRunning() const
{
    return !_futureWatcher.isNull() && _futureWatcher->isRunning();
}

bool CorePrivate::isPaused() const
{
    return !_futureWatcher.isNull() && _futureWatcher->isPaused();
}

bool CorePrivate::isCanceled() const
{
    return !_futureWatcher.isNull() && _futureWatcher->isCanceled();
}

bool CorePrivate::isFinished() const
{
    return !_futureWatcher.isNull() && _futureWatcher->isFinished();
}

bool CorePrivate::loadAlgorithm(const QString &path)
{
    if(isStarted() || isRunning())
    {
        return false;
        qWarning() << "You can not load the algorithm during the processing!";
    }

    if(_algorithm)
    {
        delete _algorithm;
        _algorithm = 0;
    }

    QPluginLoader loader(path);
    if(!loader.load()) return false;
    _algorithm = qobject_cast<IAlgorithm*>(loader.instance());
    if(!_algorithm) return false;
    _algorithmPluginPath = path;
    return true;
}

bool CorePrivate::start()
{
    if(isStarted())
    {
        qWarning() << "Already started!";
        return false;
    }

    if(isRunning())
    {
        qWarning() << "Already running!";
        return false;
    }

    if(!_algorithm)
    {
        qWarning() << "Algorithm is not loaded!";
        return false;
    }

    if(!QDir(_inputDir).exists())
    {
        qWarning() << "Invalid input dir" << _inputDir << "!";
        return false;
    }

    QVector<QString> srcImagePaths = getInputDirImages();

    if(srcImagePaths.isEmpty())
    {
        qWarning() << "No supported images found in" << _inputDir << "!";
        return false;
    }

    QDir outputDir(_outputDir);
    if(!outputDir.exists())
    {
        if(!outputDir.mkpath("."))
        {
            qWarning() << "Unable to create dir" << _outputDir<< "!";
            return false;
        }
    }

    if(!QFileInfo(_outputDir).isWritable())
    {
        qWarning() << "Output dir" << _outputDir<< " is read only!";
        return false;
    }

    std::function<ImageProcessingResult(const QString&)> mapFunctor ([=](const QString& srcImagePath)
    {
        return processImage(srcImagePath);
    });

    _results.clear();
    QFuture<ImageProcessingResult> future = QtConcurrent::mapped(srcImagePaths, mapFunctor);

    if(_futureWatcher.isNull())
    {
        _futureWatcher = new QFutureWatcher<ImageProcessingResult>(this);
        connect(_futureWatcher.data(), &QFutureWatcherBase::resultReadyAt, this, &CorePrivate::onResultReadyAt);
        connect(_futureWatcher.data(), &QFutureWatcherBase::started, this, &CorePrivate::started);
        connect(_futureWatcher.data(), &QFutureWatcherBase::finished, this, &CorePrivate::finished);
        connect(_futureWatcher.data(), &QFutureWatcherBase::finished, this, &CorePrivate::makeReport);
        connect(_futureWatcher.data(), &QFutureWatcherBase::finished, _futureWatcher.data(), &QFutureWatcherBase::deleteLater);
        connect(_futureWatcher.data(), &QFutureWatcherBase::canceled, this, &CorePrivate::canceled);
        connect(_futureWatcher.data(), &QFutureWatcherBase::paused, this, &CorePrivate::paused);
        connect(_futureWatcher.data(), &QFutureWatcherBase::resumed, this, &CorePrivate::resumed);
        connect(_futureWatcher.data(), &QFutureWatcherBase::progressValueChanged, this, &CorePrivate::progressChanged);
        connect(_futureWatcher.data(), &QFutureWatcherBase::progressRangeChanged, this, &CorePrivate::progressRangeChanged);
        _futureWatcher->setFuture(future);
    }
    return true;
}

bool CorePrivate::isSupportedImage(const QString &path) const
{
    QFileInfo fi(path);
    if(!fi.isFile()) return false;
    return _supportedImages.contains(fi.suffix());
}

QVector<QString> CorePrivate::getInputDirImages() const
{
    QVector<QString> imagePaths;
    QDirIterator dirIt(_inputDir, QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
        dirIt.next();
        if(isSupportedImage(dirIt.filePath()))
            imagePaths << dirIt.filePath();
    }
    return imagePaths;
}

ImageProcessingResult CorePrivate::processImage(const QString &srcImagePath) const
{
    QImage srcImage(srcImagePath);
    ImageProcessingResult result;
    result.srcImagePath = srcImagePath;

    if(srcImage.isNull())
    {
        qWarning() << "Unable to load image" << srcImagePath<< "!";
        return result;
    }

    QList<Face> faces;
    try
    {
        faces = _algorithm->getFaces(srcImage);
    }
    catch(...)
    {
        qCritical() << "Algorithm exeption! Source image" << srcImagePath;
        return result;
    }

    result.successfullyProcessed = true;

    foreach (const Face& face, faces)
    {
        QImage faceImage = srcImage.copy(face.boundRect).mirrored(false, true);
        if(faceImage.isNull())
            continue;
        QString faceImageName = QString("%1.jpg").arg(QUuid::createUuid().toString().mid(1, 36).remove('-'));
        QString faceImagePath = QDir(_outputDir).filePath(faceImageName);
        if(!faceImage.save(faceImagePath))
            continue;
        result.faces.insert(faceImageName, face);
    }
    return result;
}

void CorePrivate::onResultReadyAt(int index)
{
    ImageProcessingResult r = _futureWatcher->resultAt(index);
    _results << r;
    emit newImageProcessingResult(r);
}

void CorePrivate::makeReport()
{
    QFile reportFile(QDir(_outputDir).filePath("Results.json"));

    if(reportFile.exists())
    {
        if(!reportFile.remove())
        {
            qWarning() << "Unabled to remove previous report" << reportFile.fileName() << "!";
            return;
        }
    }

    if (!reportFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Unable to create report file" << reportFile.fileName() << "!";
        return;
    }

    QJsonObject reportObject;

    QJsonArray resultArray;
    foreach (const ImageProcessingResult& result, _results)
        resultArray.append(resultToJson(result));

    reportObject["date"] = QDateTime::currentDateTime().toString("dd.mm.yyyy hh:mm");
    reportObject["results"] = resultArray;

    QJsonDocument reportDoc(reportObject);
    reportFile.write(reportDoc.toJson());
    reportFile.close();
}

QJsonObject CorePrivate::resultToJson(const ImageProcessingResult &result) const
{
    QJsonObject obj;
    obj["srcImagePath"] = result.srcImagePath;
    QJsonArray array;
    foreach (QString path, result.faces.keys())
    {
        Face face = result.faces.value(path);
        array.append(faceToJson(path, face));
    }
    obj["faces"] = array;
    return obj;
}

QJsonObject CorePrivate::faceToJson(const QString& imgPath, const Face &face) const
{
    QJsonObject obj;
    obj["imgPath"] = imgPath;
    obj["boundRect"] = rectToJson(face.boundRect);
    obj["leftEyeRect"] = rectToJson(face.leftEyeRect);
    obj["rightEyeRect"] = rectToJson(face.rightEyeRect);
    obj["mouthRect"] = rectToJson(face.mouthRect);
    return obj;
}

QJsonObject CorePrivate::rectToJson(const QRect &rect) const
{
    QJsonObject obj;
    obj["x"] = rect.x();
    obj["y"] = rect.y();
    obj["width"] = rect.width();
    obj["height"] = rect.height();
    return obj;
}


QVector<ImageProcessingResult> CorePrivate::results() const
{
    return _results;
}

int CorePrivate::progressMaximum() const
{
    return _futureWatcher.isNull() ? 0 : _futureWatcher->progressMaximum();
}

int CorePrivate::progressMinimum() const
{
    return _futureWatcher.isNull() ? 0 : _futureWatcher->progressMinimum();
}

QStringList CorePrivate::supportedImages() const
{
    return _supportedImages;
}

void CorePrivate::setSupportedImages(const QStringList &supportedImages)
{
    _supportedImages = supportedImages;
}
