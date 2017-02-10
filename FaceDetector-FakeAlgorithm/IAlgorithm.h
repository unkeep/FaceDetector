#pragma once

#include <QRect>
#include <QImage>

namespace FaceDetector
{
    struct Face
    {
        QRect boundRect;
        QRect leftEyeRect;
        QRect rightEyeRect;
        QRect mouthRect;
    };

    class IAlgorithm
    {
    public:
        virtual ~IAlgorithm(){}
        virtual QList<Face> getFaces(const QImage& image) const = 0;
    };
}

Q_DECLARE_INTERFACE(FaceDetector::IAlgorithm, "Regula.FaceDetector.IAlgorithm/1.0")
