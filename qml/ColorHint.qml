import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: layout.implicitWidth + 10
    height: layout.implicitHeight + 10

    property bool hasBefore: false
    property bool hasAfter: false

    visible: hasBefore && hasAfter

    GridLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 5
        columns: 2
        property int rectSize: 20

        Rectangle {
            id: rectBefore
            color: 'red'
            border.width: 1
            border.color: 'black'
            Layout.fillWidth: true
            Layout.minimumWidth: layout.rectSize
            Layout.preferredWidth: layout.rectSize
            Layout.minimumHeight: layout.rectSize
            Layout.preferredHeight: layout.rectSize
        }

        Label {
            id: labelBefore
            text: "Before:"
            Layout.preferredWidth: 130
        }

        Rectangle {
            id: rectAfter
            color: 'green'
            border.width: 1
            border.color: 'black'
            Layout.fillWidth: true
            Layout.minimumWidth: layout.rectSize
            Layout.preferredWidth: layout.rectSize
            Layout.minimumHeight: layout.rectSize
            Layout.preferredHeight: layout.rectSize
        }

        Label {
            id: labelAfter
            text: "After:"
        }
    }

    function setBeforeColorValue(beforeColor) {
        labelBefore.text = "Before: " + colorToText(beforeColor);
        rectBefore.color = beforeColor;
        rectBefore.color.a = 1.0;
        hasBefore = true;
    }

    function setAfterColorValue(afterColor) {
        labelAfter.text = "After: " + colorToText(afterColor);
        rectAfter.color = afterColor;
        rectAfter.color.a = 1.0;
        hasAfter = true;
    }

    function hide() {
        hasBefore = false;
        hasAfter = false;
    }

    function colorToText(c) {
        return Math.round(c.r*255) + ", " + Math.round(c.g*255) + ", " + Math.round(c.b*255);
    }
}
