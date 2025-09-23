import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 6.3
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    id: root
    width: mainLayout.implicitWidth +20
    height: mainLayout.implicitHeight + 20
    border.color: "black"

    property int videoDuration: mediaPlayer.duration
    property int videoPosition: mediaPlayer.position
    property int volume: mediaPlayer.volume
    property bool showVideoSettings: false
    property bool hasWorkingProfile: false

    ColumnLayout {
        id: mainLayout
        anchors.margins: 10
        anchors.fill: parent

        RowLayout {
            visible: showVideoSettings
            Slider {
                id: sliderVideoPosition
                value: videoPosition
                from: 0
                to: videoDuration
                orientation: Qt.Horizontal
                stepSize: 0
                Layout.fillWidth: true

                onValueChanged: {
                    if (Math.abs(value - root.videoPosition) > 200)
                        mediaPlayer.position = value
                }
            }
            Label {
                id: labelDuration
                text: {
                    function timeToString(t) {
                        var h = Math.floor(t / 3600);
                        t -= 3600*h;
                        var m = Math.floor(t / 60);
                        var s = t - 60*m;

                        var res = (m >= 10 ? m : "0" + m) + ":" +
                                  (s >= 10 ? s : "0" + s);
                        if (h > 0)
                            res = h + ":" + res;

                        return res;
                    }

                    var t1 = Math.round(root.videoPosition/1000);
                    var t2 = Math.round(root.videoDuration/1000);
                    return timeToString(t1) + " / " + timeToString(t2);
                }
            }
            Slider {
                id: sliderVolume
                to: 100
                value: volume
                stepSize: 1
                Layout.maximumWidth: 100

                onValueChanged: {
                    if (value != root.volume)
                        mediaPlayer.volume = value;
                }

            }
            Label {
                text: root.volume + " %"
                Layout.minimumWidth: 40
            }

            Button {
                id: playButton
                text: "Play/Pause"
                onClicked: {
                    if (mediaPlayer.isPlaying())
                        mediaPlayer.pause();
                    else
                        mediaPlayer.play();
                }
            }
        }
        RowLayout {
            GridLayout {
                rows: displayInfo.length + 1
                columns: 3
                anchors.fill: parent
                flow: GridLayout.TopToBottom

                Label {
                    text: "Display"
                    font.bold: true
                }
                Repeater {
                    model: displayInfo
                    Label {
                        text: model.name
                    }
                }

                Label {
                    text: "Visible"
                    font.bold: true
                }
                Repeater {
                    model: displayInfo
                    CheckBox {
                        Component.onCompleted: checked = model.visible

                        Connections {
                            target: model
                            onVisibleChanged: checked = model.visible
                        }
                        onClicked: {
                            if (!visible)
                                return;

                            if (!checked) {
                                var cnt = 0;
                                for (var i = 0; i < displayInfo.length; i++)
                                    if (displayInfo[i].visible)
                                        cnt++;
                                if (cnt > 1)
                                    model.visible = checked;
                                else
                                    checked = true;
                            }
                            else
                                model.visible = checked;

                            if (model.isMainDisplay && !model.visible) {
                                model.isMainDisplay = false;

                                for (var i = 0; i < displayInfo.length; i++)
                                    if (displayInfo[i].visible) {
                                        displayInfo[i].isMainDisplay = true;
                                        break;
                                    }
                            }
                        }

                    }
                }

                RowLayout{

                    Label {
                        text: "Apply"
                        font.bold: true
                    }

                        Label {
                            text: "( for all"
                        }


                        CheckBox {
                            id: chboxApplyForAll
                            enabled: root.hasWorkingProfile
                            checked: false

                            Connections {
                                target: AllLutsModel
                                onAllLutsEnabledChanged: chboxApplyForAll.checked = AllLutsModel.allLutsEnabled
                            }

                            onClicked: AllLutsModel.enableAllLuts(checked)
                        }
                        Label {
                            text: ")"
                        }
                }


                Repeater {
                    model: displayInfo
                    CheckBox {
                        enabled: root.hasWorkingProfile
                        Component.onCompleted: checked = model.lutEnabled

                        Connections {
                            target: model
                            onLutEnabledChanged: checked = model.lutEnabled
                        }

                        onClicked: model.lutEnabled = checked
                    }
                }
            }

            ColumnLayout {
                Button {
                    text: "Working space profile"
                    Layout.fillWidth: true
                    onClicked: {
                        openWorkingProfileDialog.folder = "file:///" + fileReader.lastProfilePathDir();
                        openWorkingProfileDialog.open();
                    }
                }

                Button {
                    text: "Open Image/Video For All Displays"
                    Layout.fillWidth: true
                    onClicked: {
                        openFileDialog.folder = "file:///" + fileReader.lastMediaPathDir();
                        openFileDialog.openForMulty();
                    }
                }

                Button {
                    text: "Open Image/Video For This Display"
                    Layout.fillWidth: true
                    onClicked: {
                        openFileDialog.folder = "file:///" + fileReader.lastMediaPathDir();
                        openFileDialog.openForSingle();
                    }
                }

                Button {
                    text: "Quit"
                    Layout.fillWidth: true
                    onClicked: Qt.quit()
                }
            }

            GridLayout {
                rows: 3
                columns: 3
                flow: GridLayout.LeftToRight

                Label {} Label {} Label {}


                Label {
                   text: "Slide show:"
                }
                CheckBox {
                    id: chboxSlide

                    Component.onCompleted: checked = SlideShowModel.enableSlideShow
                    onClicked: SlideShowModel.enableSlideShow = checked
                    Connections {
                        target: SlideShowModel
                        onEnableSlideShowChanged: chboxSlide.checked = SlideShowModel.enableSlideShow
                    }
                }
                SpinBox {
                    id: spinboxSlideShow
                    enabled: chboxSlide.checked
                    from: 1
                    value: SlideShowModel.slideShowInterval

                    onValueChanged: SlideShowModel.slideShowInterval = value

                    Connections {
                        target: SlideShowModel
                        onSlideShowIntervalChanged: spinboxSlideShow.value = SlideShowModel.slideShowInterval
                    }
                }

                Label {
                    text: "Toggle auto:"
                }
                CheckBox {
                    id: chboxToggle
                    Component.onCompleted: checked = SlideShowModel.enableLutToggle
                    onClicked: SlideShowModel.enableLutToggle = checked
                    Connections {
                        target: SlideShowModel
                        onEnableLutToggleChanged: chboxToggle.checked = SlideShowModel.enableLutToggle
                    }
                }
                SpinBox {
                    id: spinboxToggleAuto
                    enabled: chboxToggle.checked
                    from: 1
                    value: SlideShowModel.lutToggleInterval
                    onValueChanged: SlideShowModel.lutToggleInterval = value
                    Connections {
                        target: SlideShowModel
                        onLutToggleIntervalChanged: spinboxToggleAuto.value = SlideShowModel.lutToggleInterval
                    }
                }
            }
        }
    }

    FileDialog {
        id: openFileDialog
        visible: false
        modality: Qt.WindowModal
        title: "Choose a image file"
        nameFilters: [ "Image/Video files (*.png *.jpg *.jpeg *.tif *.tiff *.bmp *.avi *.wmv *.mp4 *.mov)",
                       "Image files (*.png *.jpg *.jpeg *.tif *.tiff *.bmp)",
                       "Video files (*.avi *.wmv *.mp4 *.mov)",
                       "All files (*.*)"]
        property bool isSingle: false

        onAccepted: {
            var t = fileReader.fileType(fileUrl);
            if (t == "image")
                if(isSingle)
                    signalSelectedCurrentNewImage(fileUrl);
                else
                    signalSelectedNewImage(fileUrl);

                //signalOpenImage(fileUrl);
            else if (t == "video") {
                console.log("video file path: " + Qt.resolvedUrl(fileUrl));
                signalOpenVideo(fileUrl);
                mediaPlayer.setMediaFile(fileUrl);
                mediaPlayer.play();
            }
            else
                console.log("unknown file format");

            fileReader.setLastMediaPath(fileUrl);
        }

        function openForSingle(){
            isSingle = true;
            open();
        }

        function openForMulty(){
            isSingle = false;
            open();
        }
    }

    FileDialog {
        id: openWorkingProfileDialog
        visible: false
        modality: Qt.WindowModal
        title: "Choose a ICC profile file"
        nameFilters: [ "ICC Profiles (*.icc *.icm)" ]
        onAccepted: {
            signalOpenWorkingProfile(fileUrl);
            fileReader.setLastProfilePath(fileUrl);
        }
    }

    signal signalSelectedNewImage(string filePath)
    signal signalSelectedCurrentNewImage(string filePath)
}
