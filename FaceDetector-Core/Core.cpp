#include "Core.h"
#include "CorePrivate.h"

using namespace FaceDetector;

Core::Core(QObject *parent)
    :QObject(parent),
      _p(new CorePrivate(this))
{
    connect(_p, &CorePrivate::finished, this, &Core::finished);
    connect(_p, &CorePrivate::canceled, this, &Core::canceled);
    connect(_p, &CorePrivate::paused, this, &Core::paused);
    connect(_p, &CorePrivate::resumed, this, &Core::resumed);
    connect(_p, &CorePrivate::started, this, &Core::started);
    connect(_p, &CorePrivate::newImageProcessingResult, this, &Core::newImageProcessingResult);
    connect(_p, &CorePrivate::progressChanged, this, &Core::progressChanged);
    connect(_p, &CorePrivate::progressRangeChanged, this, &Core::progressRangeChanged);
}

int Core::maxThreadCount() const
{
    return _p->maxThreadCount();
}

void Core::setMaxThreadCount(int maxThreadCount)
{
    _p->setMaxThreadCount(maxThreadCount);
}

QString Core::inputDir() const
{
    return _p->inputDir();
}

void Core::setInputDir(const QString &inputDir)
{
    _p->setInputDir(inputDir);
}

QString Core::outputDir() const
{
    return _p->outputDir();
}

void Core::setOutputDir(const QString &outputDir)
{
    _p->setOutputDir(outputDir);
}

bool Core::isStarted() const
{
    return _p->isStarted();
}

bool Core::isRunning() const
{
    return _p->isRunning();
}

bool Core::isPaused() const
{
    return _p->isPaused();
}

bool Core::isCanceled() const
{
    return _p->isCanceled();
}

bool Core::isFinished() const
{
    return _p->isFinished();
}

bool Core::loadAlgorithm(const QString &path)
{
    return _p->loadAlgorithm(path);
}

QString Core::loadedAlgorithmPath() const
{
    return _p->loadedAlgorithmPath();
}

QStringList Core::supportedImages() const
{
    return _p->supportedImages();
}

void Core::setSupportedImages(const QStringList &supportedImages)
{
    _p->setSupportedImages(supportedImages);
}

int Core::progressMaximum() const
{
    return _p->progressMaximum();
}

int Core::progressMinimum() const
{
    return _p->progressMinimum();
}

bool Core::start()
{
    return _p->start();
}

bool Core::cancel()
{
    return _p->cancel();
}

bool Core::pause()
{
    return _p->pause();
}

bool Core::resume()
{
    return _p->resume();
}
