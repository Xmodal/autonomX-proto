#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include "QDebug"

class Generator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double frequency READ getFrequency WRITE setFrequency NOTIFY frequencyChanged)
private:
    double phase;
    double frequency;
    double output;
public:
    explicit Generator(QObject *parent = nullptr);

    void computeOutput(double deltaTime);
    double readOutput(int index);
    void writeInput(double value, int index);

    double getFrequency() const {
        return frequency;
    };
    void setFrequency(double frequency) {
        this->frequency = frequency;
        emit frequencyChanged(frequency);
    };
signals:
    void frequencyChanged(double);
};

#endif // GENERATOR_H
