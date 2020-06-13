#include "generator.h"
#include <math.h>
#include <QDebug>

Generator::Generator(QObject *parent) : QObject(parent), phase(0), frequency(1)
{

}

void Generator::computeOutput(double deltaTime) {
    phase += deltaTime * frequency;

    // update output
    output = 0.5 + 0.5 * cos(phase * 2.0 * M_PI);
    outputChanged(output);

//    qDebug() << deltaTime;
    QString bar = "";
    int maxLength = 16;
    for(int i = 0; i < output * maxLength; i++) {
        bar += "=";
    }
//    qDebug() << bar;
}

double Generator::readOutput(int index) {
    if(index != 0)
        return 0;
    return output;
}
