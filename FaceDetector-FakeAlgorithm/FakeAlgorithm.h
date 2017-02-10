#pragma once
#include "facedetector-fakealgorithm_global.h"
#include "IAlgorithm.h"

#include <QObject>

namespace FaceDetector
{
    class FACEDETECTORFAKEALGORITHMSHARED_EXPORT FakeAlgorithm : public QObject, IAlgorithm
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "Regula.FaceDetector.IAlgorithm/1.0" FILE "Meta.json")
        Q_INTERFACES(FaceDetector::IAlgorithm)
    public:
        explicit FakeAlgorithm(QObject* parent = 0);
        ~FakeAlgorithm();

        // IAlgorithm interface
    public:
        QList<Face> getFaces(const QImage &image) const;
    };
}
