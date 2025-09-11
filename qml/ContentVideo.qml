import QtQuick 2.1
import QtMultimedia 5.0

Item {
    id: root

    VideoOutput {
        id: videoOutput
        source: proxyVideoSurface.getMediaSource(model.displayId)
        smooth: true
        anchors.fill: parent

        objectName: "viewer"
        layer.enabled: applyShader
        layer.effect: ShaderEffect {
            id: lutShader
            property variant lut3d: image3dLut
            fragmentShader: fileReader.fragmentShader();
        }
    }
}
