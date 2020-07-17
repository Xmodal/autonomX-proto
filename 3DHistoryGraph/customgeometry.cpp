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
    // ?????????????
    // https://stackoverflow.com/questions/26841755/how-to-convert-qvectordouble-to-qbytearray
    // QByteArray vertices = QByteArray::fromRawData(reinterpret_cast<const char*>(points.data()), sizeof(double) * points.size());

    // i think we're supposed to populate this QByteArray too but i cannot understand for the life of me how OpenGL works
    QByteArray indices;

    setPrimitiveType(PrimitiveType::LineStrip);
    setVertexData(vertices);
    setIndexData(indices);
    setStride(sizeof(QVector3D));
    addAttribute(Attribute::Semantic::PositionSemantic, 0, Attribute::ComponentType::F32Type);
}
