import QtQuick 2.15
import QtQuick.Window 2.15
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Window {
    id: main
    visible: true
    width: 800
    height: 600
    title: "QubyxViewer"

    Repeater {
        model: displayInfo

        Viewer {
           Component.onCompleted: {
               main.signalOpenImage.connect(openImage);
               signalOpenImage.connect(main.signalOpenImage);
               main.signalOpenVideo.connect(openVideo);
               signalOpenVideo.connect(main.signalOpenVideo);
               signalOpenWorkingProfile.connect(main.signalOpenWorkingProfile);
               main.signalReload3dLut.connect(reload3dLUT);

               signalSelectedNewImage.connect(main.updateImageList);
               signalLeftButtonPressed.connect(main.showLeftImage);
               signalRightButtonPressed.connect(main.showRightImage);
           }
        }
    }

    Component.onCompleted: {
        updateImageList("file:///C:/Users/Public/Pictures/Sample Pictures/Desert.jpg");
    }

    Connections {
        target: SlideShowModel
        onSignalNextImage: showRightImage()
        onSignalEnableAllLuts: AllLutsModel.enableAllLuts(enabled)
    }

    function reload3dLut() {
        console.log("reload3dLut");
        signalReload3dLut();
    }

    function updateImageList(filePath) {
        console.log("updateImageList");
        Qt.listOfImages = fileReader.getListOfImageFiles(filePath);
        Qt.imageIndex = 0;
        if (Qt.listOfImages && Qt.listOfImages.length > 0)
            signalOpenImage(Qt.listOfImages[0]);
    }

    function showLeftImage() {
        showNextImage(-1);
    }

    function showRightImage() {
        showNextImage(1);
    }

    function showNextImage(shift) {
        if (Qt.listOfImages && Qt.listOfImages.length > 0) {
            Qt.imageIndex = (Qt.imageIndex + Qt.listOfImages.length + shift)%Qt.listOfImages.length;
            signalOpenImage(Qt.listOfImages[Qt.imageIndex]);
        }
    }

    signal signalOpenImage(string imagePath)
    signal signalOpenVideo(string videoPath)
    signal signalOpenWorkingProfile(string filePath)
    signal signalReload3dLut()
}
