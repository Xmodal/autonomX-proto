#include "computeengine.h"
#include "algorithm"
#include <QDebug>

ComputeEngine::ComputeEngine(Generator *generator)
{
    this->generator = generator;
}

void ComputeEngine::run() {
    // this breaks after a while if the application isn't interacted with (ie is not the operating system's currently active app)

    qint64 millisCompute;
    qint64 millisLastFrame;

    if(firstFrame) {
        firstFrame = false;
        millisLastFrame = 1.0 / frequency * 1000.0;
    } else {
        millisLastFrame = elapsedTimerCompute.nsecsElapsed() / 1000000.0;
    }

    elapsedTimerCompute.restart();
    elapsedTimerCompute.start();

    generator->computeOutput(millisLastFrame / 1000.0);

    millisCompute = elapsedTimerCompute.nsecsElapsed() / 1000000.0;

    QTimer timer;
    timer.singleShot((int) std::min<double>(1.0 / frequency * 1000.0, std::max<double>(1.0 / frequency * 1000.0 - millisCompute, 0)), this, &ComputeEngine::run);
}
