#include "computeengine.h"
#include "algorithm"
#include <QDebug>

ComputeEngine::ComputeEngine(Generator *generator)
{
    this->generator = generator;
}

void ComputeEngine::run() {
    // this solution is not very robust if CPU resources are in high demand by other programs
    // this breaks after a while if the application isn't interacted with (ie is not the operating system's currently active app)

    qint64 millisCompute;   // time in nanoseconds taken by computation
    qint64 millisLastFrame; // time in nanoseconds since last frame

    // compute time since last frame, with exception if this is the first frame
    if(firstFrame) {
        firstFrame = false;
        millisLastFrame = 1.0 / frequency * 1000.0;
    } else {
        millisLastFrame = elapsedTimerCompute.nsecsElapsed() / 1000000.0;
    }

    // restart the timer to measure frame time and compute time
    elapsedTimerCompute.restart();
    elapsedTimerCompute.start();

    // do the computation
    generator->computeOutput(millisLastFrame / 1000.0);

    // measure the time used to do the computation
    millisCompute = elapsedTimerCompute.nsecsElapsed() / 1000000.0;

    // schedule a new function call at the appropriate time
    // this bit maybe could be improved?
    // QTimer only guarantees the timing interval is AT LEAST equal to the duration we ask it for.
    // if there are other things using up CPU, this can fail very hard.
    // on my development machine (Simon), it can drop out spectacularly (going suddenly from 30FPS to 0.1FPS)
    QTimer timer;
    timer.singleShot((int) std::min<double>(1.0 / frequency * 1000.0, std::max<double>(1.0 / frequency * 1000.0 - millisCompute, 0)), this, &ComputeEngine::run);
}
