#pragma once

#include <QObject>
#include <QVector3D>
#include <QQuick3DGeometry>

class CustomGeometry : public QQuick3DGeometry
{
    Q_OBJECT
public:
    CustomGeometry();

    void setProperty(const char *name, const QVector<double> &value);
private:
    void rebuildGeometry(const QVector<double> &points);
};
