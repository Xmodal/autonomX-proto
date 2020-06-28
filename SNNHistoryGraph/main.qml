import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import com.dynamiclights 1.0

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
        graph2buffer.requestPaint();

        // update attempt #3
        historyGraph.addPoint(val);
        graph3.requestPaint();
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
            // this solution should technically be the most cost-effective
            // since it delegates all the work to the GPU
            // problem is, data isn't stored anywhere, and as a result,
            // its lifespan is severely limited. as soon as we disable or defocus the graph,
            // it has to regenerate itself from the bottom up, which is kind of inconvenient in most cases.
            Item {
                width: 300
                height: 150
                Layout.margins: 15

                Rectangle {
                    color: "crimson"
                    anchors.fill: parent
                }

                Canvas {
                    id: graph2buffer
                    anchors.fill: parent

                    // paint is requested every time a new value is received

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);

                        // get pixel data from graph2
                        var pixelData = graph2.getContext("2d").getImageData(0, 0, width, height);
                        // put pixel data in graph2 offset by a pixel
                        ctx.putImageData(pixelData, -70, 0);
                        // request paint from graph2
                        graph2.requestPaint();
                    }
                }

                Canvas {
                    id: graph2
                    anchors.fill: parent

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);
                        ctx.strokeStyle = "lightgray";

                        // when paint requested:
                        // - draw graph2buffer as image
//                        var pixelData = graph2buffer.getContext("2d").getImageData(0, 0, width, height);
//                        ctx.putImageData(pixelData, 0, 0);
                        // - draw subsequent line in remaining 1px-wide area
//                        ctx.beginPath();
//                        ctx.moveTo(width - 1, prevValue * height);
//                        ctx.lineTo(width, currValue * height);
//                        ctx.closePath();
//                        ctx.stroke();

                        ctx.rect(width - 20, 0, 20, 20);
                        ctx.fill();

                        var data = ctx.getImageData(0, 0, width, height);
                        ctx.drawImage(data.data, -50, 0);
                    }
                }
            }

            // attempt #3 (with circular_buffer)
            Item {
                id: graph3container
                width: 300; height: 150

                // circular buffer
                HistoryGraph {
                    id: historyGraph
                    width: graph3container.width
                }

                Rectangle {
                    color: "crimson"
                    anchors.fill: parent
                }

                Canvas {
                    id: graph3
                    anchors.fill: parent

                    // paint is requested every time a new value is received
                    onPaint: {
                        var ctx = getContext("2d");

                        ctx.strokeStyle = "lightgrey";
                        ctx.clearRect(0, 0, graph.width, graph.height);

                        var points = historyGraph.points;

                        ctx.beginPath();
                        for (var i = 0; i < points.length; i++) {
                            ctx.moveTo(i, points[i] * graph.height);
                            if (points[i]) ctx.lineTo(i + 1, points[i + 1] * graph.height);
                        }
                        ctx.closePath();
                        ctx.stroke();
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
