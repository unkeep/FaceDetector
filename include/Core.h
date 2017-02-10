#pragma once
#include "facedetector-core_global.h"
#include <QObject>
#include "ImageProcessingResult.h"

namespace FaceDetector
{
    class CorePrivate;
    class FACEDETECTORCORESHARED_EXPORT Core : public QObject
    {
        Q_OBJECT
    public:
        explicit Core(QObject* parent = 0);

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

    signals:
        void finished();
        void canceled();
        void paused();
        void resumed();
        void started();
        void newImageProcessingResult(ImageProcessingResult);
        void progressChanged(int);
        void progressRangeChanged(int min, int max);

    private:
        CorePrivate* _p;
    };
}
