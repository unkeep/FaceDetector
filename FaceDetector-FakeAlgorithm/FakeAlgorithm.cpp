#include "FakeAlgorithm.h"
#include <QDebug>
#include <QThread>
#include <QTranslator>


using namespace FaceDetector;
FakeAlgorithm::FakeAlgorithm(QObject *parent)
    :QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
}

FakeAlgorithm::~FakeAlgorithm()
{
    qDebug() << Q_FUNC_INFO;
}

QList<Face> FakeAlgorithm::getFaces(const QImage &image) const
{
    QList<Face> faces;
    if(image.width() > 500 && image.height() > 500)
    {
        QRect imgRect = image.rect();
        QTransform t;
        t.scale(0.2, 0.2);
        Face f;
        f.boundRect = t.mapRect(imgRect);
        f.boundRect.moveCenter(imgRect.center());
        faces << f;
    }

    return faces;
}
