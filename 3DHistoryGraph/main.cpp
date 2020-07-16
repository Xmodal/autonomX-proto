#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QList>
#include <QThread>
#include <QSharedPointer>
#include <QQmlPropertyMap>

#include "Generator.h"
#include "SpikingNet.h"
#include "ComputeEngine.h"
#include "Facade.h"
#include "customgeometry.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    // make Generator virtual class recognizable to QML
    // this line is apparently necessary for the QML engine to receive Generator pointers
    // and retrieve a class instance's exposed properties by model index through said pointer
    qmlRegisterUncreatableType<Generator>("ca.hexagram.xmodal.dynamiclight", 1, 0, "Generator", "Cannot instanciate Generator.");
    qmlRegisterUncreatableType<Facade>("ca.hexagram.xmodal.dynamiclight", 1, 0, "Facade", "Cannot instanciate Facade.");
    qmlRegisterUncreatableType<SpikingNet>("ca.hexagram.xmodal.dynamiclight", 1, 0, "SpikingNet", "Cannot instanciate SpikingNet.");
    qmlRegisterUncreatableType<NeuronType>("ca.hexagram.xmodal.dynamiclight", 1, 0, "NeuronType", "Cannot instanciate NeuronType.");
    qmlRegisterType<CustomGeometry>("ca.hexagram.xmodal.dynamiclight", 1, 0, "CustomGeometry");

    // create generator list
    QSharedPointer<Generator> spikingNet = QSharedPointer<Generator>(new SpikingNet());
    QSharedPointer<QList<QSharedPointer<Generator>>> generators = QSharedPointer<QList<QSharedPointer<Generator>>>(new QList<QSharedPointer<Generator>>());
    generators.get()->append(spikingNet);

    // create generator facade list
    QSharedPointer<QList<QSharedPointer<Facade>>> generatorFacades = QSharedPointer<QList<QSharedPointer<Facade>>>(new QList<QSharedPointer<Facade>>());

    // create facades and link them
    for(QList<QSharedPointer<Generator>>::iterator it = generators.get()->begin(); it != generators.get()->end(); it++) {
        QSharedPointer<Generator> generator = *it;
        QSharedPointer<Facade> generatorFacade = QSharedPointer<Facade>(new Facade(generator.get()));

        // connect generator changes to facade
        QObject::connect(generator.data(), &Generator::valueChanged, generatorFacade.data(), &Facade::updateValueFromAlias, Qt::QueuedConnection);
        // connect facade changes to generator
        QObject::connect(generatorFacade.get(), &Facade::valueChanged, generator.get(), &Generator::updateValue, Qt::QueuedConnection);
        // add the newly constructed facade to the list
        generatorFacades.get()->append(generatorFacade);
    }

    // create compute thread
    QSharedPointer<QThread> computeThread = QSharedPointer<QThread>(new QThread());
    computeThread->start(QThread::TimeCriticalPriority);

    // create compute engine
    ComputeEngine computeEngine(generators);

    // move compute engine to compute thread
    computeEngine.moveToThread(computeThread.get());

    // move generators to compute thread
    for(QList<QSharedPointer<Generator>>::iterator it = generators.get()->begin(); it != generators.get()->end(); it++) {
        (*it)->moveToThread(computeThread.get());
    }

    // start compute engine
    computeEngine.start();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("generator", (*generatorFacades)[0].get());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [computeThread](){
        //qDebug() << "about to exit";
        computeThread->exit();
    });

    return app.exec();
}
