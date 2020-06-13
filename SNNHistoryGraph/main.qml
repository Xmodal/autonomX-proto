import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    width: 800
    height: 300
    title: qsTr("SNN History Graph prototype")
    visible: true

    property real currValue: 0
    property real prevValue: 0
    property variant values: []
    readonly property real timeSpan: 10

    // manage generator delegation here
    Connections {
        target: generator
        onOutputChanged: updateSineValue(generator.output)
    }

    function updateSineValue(val) {
        // update attempt #1
        values.push(val);
        if (values.length > graph.width) values.shift();
        graph.requestPaint();

        // update attempt #2
        prevValue = currValue;
        currValue = val;
        graph2.requestPaint();
    }

    background: Rectangle {
        color: "lightgrey"
    }

    ColumnLayout {
        spacing: 30

        Label {
            text: "History graph prototype with Canvas"
            font: {
                pointSize: 20
                color: "crimson"
            }
        }

        RowLayout {
            spacing: 30

            // attempt #1
            // this solution is extremely inefficient because it requires an unnecessarily high amount of array accesses
            // it would be wiser to cache the canvas output and shift it by a time frame on every paint
            // but idk how possible it could be without using too much GPU power
            Item {
                width: 300
                height: 150
                Layout.margins: 15

                Rectangle {
                    color: "crimson"
                    anchors.fill: parent
                }

                Canvas {
                    id: graph
                    anchors.fill: parent

                    // paint is requested every time a new value is received
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.strokeStyle = "lightgrey";
                        ctx.clearRect(0, 0, graph.width, graph.height);

                        ctx.beginPath();
                        ctx.moveTo(graph.width - values.length, values[0] * graph.height);
                        for (var i = 1; i < values.length; i++) {
                            ctx.lineTo(graph.width - values.length + i, values[i] * graph.height);
                            ctx.moveTo(graph.width - values.length + i, values[i] * graph.height);
                        }
                        ctx.closePath();
                        ctx.stroke();
                    }
                }
            }

            // attempt #2
            Item {
                width: 300
                height: 150
                Layout.margins: 15

                Rectangle {
                    color: "crimson"
                    anchors.fill: parent
                }

                Canvas {
                    id: graph2
                    property int frameCount: 0
                    property bool initialized: false
                    anchors.fill: parent

                    // paint is requested every time a new value is received
                    onPaint: {
                        var ctx = getContext("2d");
                    }
                }
            }

            Item {
                width: 50
                height: 50

                Rectangle {
                    anchors.fill: parent
                    color: Qt.tint("crimson", Qt.rgba(0, 0, 0, currValue))
                    radius: width * 0.5
                }
            }
        }
    }
}
