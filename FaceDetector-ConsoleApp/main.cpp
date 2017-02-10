#include <QCoreApplication>
#include <QTimer>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QThread>
#include <QDebug>

#include "FaceDetector-Core/Core.h"

using namespace FaceDetector;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Ankip");

    QCommandLineParser parser;
    parser.setApplicationDescription("FaceDetector");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption inputPathOption( {"i" ,"input"},
                                        "Input directory path",
                                        "path", ".");
    parser.addOption(inputPathOption);

    QCommandLineOption outputPathOption( {"o" ,"output"},
                                         "Output directory path",
                                         "path", "../Results");
    parser.addOption(outputPathOption);

    QCommandLineOption algorithmPathOption( {"a" ,"algorithm"},
                                         "Algorithm plugin path",
                                         "path", "FaceDetector-FakeAlgorithm-1.0.dll");
    parser.addOption(algorithmPathOption);

    QCommandLineOption formatOption( {"f" ,"format"},
                                     "Supported imgage formats '|' separated(jpg|png|bmp etc.)",
                                     "formats", "jpg|JPEG|png|bmp");
    parser.addOption(formatOption);

    QCommandLineOption threadMaxOption( {"t" ,"threadMax"},
                                        "Maximum thread count(The default max thread count is QThread::idealThreadCount)",
                                        "count", "");
    parser.addOption(threadMaxOption);

    parser.process(a);

    Core* core = new Core(&a);

    QString inputPath = parser.value(inputPathOption);
    core->setInputDir(inputPath);

    QString outputPath = parser.value(outputPathOption);
    core->setOutputDir(outputPath);

    QString algorithmPath = parser.value(algorithmPathOption);
    if(!core->loadAlgorithm(algorithmPath))
    {
        qCritical() << "Unabled load algorithm" << algorithmPath;
        return 1;
    }

    QStringList supportedFormats = parser.value(formatOption).toLower().split("|");
    core->setSupportedImages(supportedFormats);

    int threadMaxCount = parser.value(threadMaxOption).toInt();
    core->setMaxThreadCount(threadMaxCount ? threadMaxCount : QThread::idealThreadCount());


    QObject::connect(core, &Core::finished, qApp, &QCoreApplication::quit);
    QObject::connect(core, &Core::newImageProcessingResult, [](const ImageProcessingResult& result)
    {
        qDebug() << "Image" << result.srcImagePath << "processed" << (result.successfullyProcessed ? "succesfully" : "with errors");
        if(result.successfullyProcessed)
            qDebug() << "Faces:" << result.faces.keys();
    });

    if(!core->start())
    {
        qCritical() << "Unable to start processing!";
        return 1;
    }

    return a.exec();
}
