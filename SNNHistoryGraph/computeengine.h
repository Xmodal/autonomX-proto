#ifndef COMPUTEENGINE_H
#define COMPUTEENGINE_H

#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include "generator.h"


class ComputeEngine : public QThread
{
private:
    Generator *generator;
    QElapsedTimer elapsedTimerCompute;
    double frequency = 30;
    bool firstFrame = true;
public:
    ComputeEngine(Generator *generator);
    void run() override;
};

#endif // COMPUTEENGINE_H
