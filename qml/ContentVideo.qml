import QtQuick 2.15
import QtMultimedia 6.3

Item {
    id: root

    Video {
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
