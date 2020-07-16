#include "customgeometry.h"

CustomGeometry::CustomGeometry()
{

}

void CustomGeometry::setProperty(const char *name, const QVector<double> &value)
{
    rebuildGeometry(value);
}

void CustomGeometry::rebuildGeometry(const QVector<double> &points)
{
    QByteArray vertices;
    QByteArray indices;
    setPrimitiveType(PrimitiveType::Points);
    setVertexData(vertices);
    setIndexData(indices);
    setStride(sizeof(QVector3D));
    addAttribute(Attribute::Semantic::PositionSemantic, 0, Attribute::ComponentType::F32Type);
}
