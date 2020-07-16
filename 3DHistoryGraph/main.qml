import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick3D 1.14

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: qsTr("3D History Graph Prototype")
    color: "#848895"

    Connections {
        target: generator
        onOutputMonitorChanged: console.log(generator.outputMonitor)
    }

    View3D {
        anchors.fill: parent
        camera: camera

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 200, -300)
            rotation: Qt.vector3d(30, 0, 0)
        }

        Model {
            position: Qt.vector3d(0, 0, 0)
            scale: Qt.vector3d(2, 2, 2)
            source: "#Sphere"
            materials: [
                ExampleMaterial {
                    id: exampleMaterial
                    time: 0.5
                    amplitude: 5
                }
            ]
        }
    }
}
