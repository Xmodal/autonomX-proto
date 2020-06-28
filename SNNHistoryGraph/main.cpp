#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QFontDatabase>

#include "generator.h"
#include "computeengine.h"
#include "historygraph.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<HistoryGraph>("com.dynamiclights", 1, 0, "HistoryGraph");

    Generator generator;
    engine.rootContext()->setContextProperty("generator", &generator);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    ComputeEngine computeEngine(&generator);
    computeEngine.start(QThread::TimeCriticalPriority);

    return app.exec();
}
