#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "generator.h"
#include "computeengine.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Generator generator;
    engine.rootContext()->setContextProperty("generator", &generator);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    ComputeEngine computeEngine(&generator);
    computeEngine.start(QThread::TimeCriticalPriority);

    return app.exec();
}
