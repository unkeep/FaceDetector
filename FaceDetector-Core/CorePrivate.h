#pragma once
#include <QObject>
#include "IAlgorithm.h"
#include <QFutureWatcher>
#include <QPointer>
#include <QJsonObject>
#include "ImageProcessingResult.h"

namespace FaceDetector
{
    class CorePrivate : public QObject
    {
        Q_OBJECT
    public:
        explicit CorePrivate(QObject *parent);

        int maxThreadCount() const;
        void setMaxThreadCount(int maxThreadCount);

        QString inputDir() const;
        void setInputDir(const QString &inputDir);

        QString outputDir() const;
        void setOutputDir(const QString &outputDir);

        bool isStarted() const;
        bool isRunning() const;
        bool isPaused() const;
        bool isCanceled() const;
        bool isFinished() const;

        bool loadAlgorithm(const QString& path);
        QString loadedAlgorithmPath() const;

        QStringList supportedImages() const;
        void setSupportedImages(const QStringList &supportedImages);

        QVector<ImageProcessingResult> results() const;

        int progressMaximum() const;
        int progressMinimum() const;

    public slots:
        bool start();
        bool cancel();
        bool pause();
        bool resume();

    signals:
        void finished();
        void canceled();
        void paused();
        void resumed();
        void started();
        void newImageProcessingResult(ImageProcessingResult);
        void progressChanged(int);
        void progressRangeChanged(int, int);

    private:
        bool isSupportedImage(const QString& path) const;
        QVector<QString> getInputDirImages() const;
        ImageProcessingResult processImage(const QString& srcImagePath) const;
        void onResultReadyAt(int index);
        void makeReport();
        QJsonObject resultToJson(const ImageProcessingResult& result) const;
        QJsonObject faceToJson(const QString &imgPath, const Face& face) const;
        QJsonObject rectToJson(const QRect& rect) const;

    private:
        QString _algorithmPluginPath;
        QString _inputDir;
        QString _outputDir;
        IAlgorithm* _algorithm;
        QStringList _supportedImages;
        QPointer<QFutureWatcher<ImageProcessingResult>> _futureWatcher;
        QVector<ImageProcessingResult> _results;
    };

}
