pragma Singleton
import QtQuick 2.15

Item {
    property bool enableSlideShow: false
    property int slideShowInterval: 10

    property bool enableLutToggle: false
    property int lutToggleInterval: 5

    onEnableLutToggleChanged: {
        if (enableLutToggle)
            timerLutToggle.newLutStatus = true;
    }

    Timer {
        id: timerSlideShow
        interval: slideShowInterval*1000
        running: enableSlideShow
        repeat: true

        onTriggered: {
            signalNextImage();

            if (enableLutToggle)
            {
                signalEnableAllLuts(false);
                timerLutToggle.restart();
                timerLutToggle.newLutStatus = true;
            }
        }
    }

    Timer {
        id: timerLutToggle
        interval: lutToggleInterval*1000
        running: enableLutToggle
        repeat: true
        property bool newLutStatus: true

        onTriggered: {
            signalEnableAllLuts(newLutStatus);
            newLutStatus = !newLutStatus;
        }
    }

    signal signalEnableAllLuts(bool enabled)
    signal signalNextImage()
}
