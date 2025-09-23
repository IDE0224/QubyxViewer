import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 6.3

Item {
    id: root
    property bool mainDisplay: model.isMainDisplay
    property bool showSettings: mainDisplay

    states: [
        State {
            name: "image"
            PropertyChanges {
                target: tabViewPreviewArea
                currentIndex: 0
            }
        },
        State {
            name: "video"
            PropertyChanges {
                target: tabViewPreviewArea
                currentIndex: 1
            }
        }
    ]


    Window {
        id: mainWindow
        flags: Qt.FramelessWindowHint | Qt.Window | Qt.WindowStaysOnTopHint
        width: model.geometry.width
        // try to avoid blinking in fullscreen mode on main display. https://bugreports.qt.io/browse/QTBUG-47247
        height: model.geometry.height + 1
        x: model.geometry.x
        y: model.geometry.y
        visible: model.visible
        color: "black"

        Image
        {
            id: image3dLut
            source: "image://LUT/" + model.displayId
            visible: false
        }

        SwipeView {
            id: tabViewPreviewArea
            anchors.fill: parent
            anchors.bottomMargin: 1
            currentIndex: root.state === "image" ? 0 : 1

            Item {
                id: contentImage
                ContentImage {
                    property bool applyShader: model.lutEnabled && root.state == "image"
                }
            }

            Item {
                id: contentVideo
                ContentVideo {
                    property bool applyShader: model.lutEnabled && root.state == "video"
                }
            }
        }

        MouseArea {
            id: windowMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            propagateComposedEvents: true
            z: 1

            onPositionChanged: {
                if (mainDisplay && !showSettings)
                    showSettings = true;
            }

            onClicked: {
                if ((mouse.modifiers & Qt.ControlModifier) && root.state == "image") {
                    console.log("left click " + mouse.x + " " + mouse.y);
                    contentImage.children[0].showPixelColor(mouse.x, mouse.y, colorHint);
                }
            }

            onDoubleClicked: {
                for (var i = 0; i < displayInfo.length; i++)
                    displayInfo[i].isMainDisplay = displayInfo[i].displayId === model.displayId;
                root.showSettings = true;
            }
        }

        ColorHint {
            id: colorHint
            anchors.horizontalCenter: tabViewPreviewArea.horizontalCenter
            anchors.verticalCenter: tabViewPreviewArea.verticalCenter
        }

        SettingsPanel {
            id: settingsPanel
            visible: showSettings && model.isMainDisplay
            anchors.bottom: tabViewPreviewArea.bottom
            anchors.horizontalCenter: tabViewPreviewArea.horizontalCenter

            showVideoSettings: root.state == "video"

            onSignalSelectedNewImage: root.signalSelectedNewImage(filePath)
            onSignalSelectedCurrentNewImage: root.openImage(filePath)
        }

        Rectangle {
            color: model.lutEnabled ? "green" : "red"
            border.color: "black"
            width: 30
            height: 30
            x: 10
            y: 10
            radius: 15

        }

        MouseArea {
            id: settingsMouseArea
            anchors.fill: settingsPanel
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
            propagateComposedEvents: true

            onEntered: {
                hideSettingsTimer.stop();
                showSettings = true;
            }
            onExited: {
                hideSettingsTimer.restart();
                showSettings = false;
            }
        }

        Component.onCompleted: {
            //mainWindow.height = model.geometry.height;
            x = model.geometry.x;
            y = model.geometry.y;
        }

        Item {
            focus: true
            Keys.onEscapePressed: Qt.quit()

            Keys.onLeftPressed: {
                console.log("left");
                if (root.state == "image")
                    signalLeftButtonPressed();
            }

            Keys.onRightPressed: {
                console.log("right");
                if (root.state == "image")
                    signalRightButtonPressed();
            }
        }
    }

    onShowSettingsChanged: {
        if (showSettings)
            hideSettingsTimer.restart();
    }

    Timer {
        id: hideSettingsTimer
        interval: 5000
        running: false
        repeat: false

        onTriggered: showSettings = false
    }

    signal signalOpenImage(string filePath)
    signal signalOpenVideo(string filePath)
    signal signalOpenWorkingProfile(string filePath)
    signal signalSelectedNewImage(string filePath)
    signal signalLeftButtonPressed();
    signal signalRightButtonPressed();

    function openImage(filePath) {
        stopVideo();
        root.state = "image";

        if(settingsPanel.openOnCurrent_)
        {
            if(mainDisplay){
                contentImage.children[0].source = filePath;
            }
        }
        else
            contentImage.children[0].source = filePath;

        colorHint.hide();
    }

    function openVideo(filePath) {
        stopVideo();
        root.state = "video";
        //contentVideo.item.mediaSource = filePath;
        //contentVideo.item.play();
        colorHint.hide();
    }

    function reload3dLUT() {

        console.log("reload3dLUT");
        image3dLut.source = "image://LUT/" + model.displayId + "&" + Date.now();
        settingsPanel.hasWorkingProfile = true;
    }

    function stopVideo() {
        mediaPlayer.stop();
    }
}
