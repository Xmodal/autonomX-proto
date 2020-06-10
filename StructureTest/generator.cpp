#include "generator.h"
#include <math.h>
#include <QDebug>

Generator::Generator(QObject *parent) : QObject(parent), time(0), frequency(1)
{

}

void Generator::computeOutput(double deltaTime) {
    time += deltaTime;
    output = cos(time * frequency * 2.0 * M_PI);
    qDebug() << deltaTime;
}

double Generator::readOutput(int index) {
    if(index != 0)
        return 0;
    return output;
}
