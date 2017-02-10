#pragma once
#include <QString>
#include <QMap>
#include "IAlgorithm.h"

namespace FaceDetector
{
    struct ImageProcessingResult
    {
        ImageProcessingResult() : successfullyProcessed(false){}

        bool successfullyProcessed;
        QString srcImagePath;
        QMap<QString, Face> faces; //file name to Face map
    };
}
