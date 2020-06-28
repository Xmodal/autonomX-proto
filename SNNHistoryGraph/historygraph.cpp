#include "historygraph.h"


HistoryGraph::HistoryGraph(QObject *parent) : QObject(parent), points(300)
{

}

void HistoryGraph::addPoint(double point)
{
    points.put(point);
}

int HistoryGraph::getWidth() const
{
    return width;
}

QVariantList HistoryGraph::getPoints()
{
    // clear QVariantList member property
    p.clear();

    // tail pointer is offset by 1
    // so that data is always populated from right to left
    points.get();
    // circular buffer is regenerated as a QVariantList for QML to use it
    for (size_t i = 0; i < points.capacity(); i++) {
        p.append(QVariant::fromValue(points.get()));
    }

    // return QVariantList object
    return p;
}

void HistoryGraph::writeWidth(int width)
{
    this->width = width;
}
