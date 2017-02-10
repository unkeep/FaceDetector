#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
    : QRunnable()
{
}

QString ImageProcessor::imagePath() const
{
    return _imagePath;
}

void ImageProcessor::setImagePath(const QString &imagePath)
{
    _imagePath = imagePath;
}
