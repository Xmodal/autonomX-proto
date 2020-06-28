#ifndef HISTORYGRAPH_H
#define HISTORYGRAPH_H

#include <QObject>
#include <QVariant>

#include "circular_buffer.cpp"

class HistoryGraph : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int width READ getWidth WRITE writeWidth NOTIFY widthChanged)
    Q_PROPERTY(QVariantList points READ getPoints NOTIFY pointsChanged)
public:
    explicit HistoryGraph(QObject *parent = nullptr);

    Q_INVOKABLE void addPoint(double point);

    int getWidth() const;
    QVariantList getPoints();

    void writeWidth(int width);

private:
    circular_buffer<double> points;
    QVariantList p;
    int width = 10;

signals:
    void widthChanged(int);
    void pointsChanged(QVariantList);
};

#endif // HISTORYGRAPH_H
