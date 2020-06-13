import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    width: 640
    height: 480
    title: qsTr("SNN History Graph prototype")
    visible: true

    property real sineValue;

    // manage generator delegation here
    Connections {
        target: generator
        onOutputChanged: sineValue = generator.output
    }

    background: Rectangle {
        color: "lightblue"
    }

    RowLayout {
        Label {
            color: "black"
            font.pointSize: 20
            text: sineValue
        }
    }
}
