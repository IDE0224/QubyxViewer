import QtQuick 2.1

Item {
    property string source;
    id: imageOutput

    ShaderEffectSource {
        property string source;
        id: shaderSource
        sourceItem: srcImage
        width: imageOutput.width
        height: imageOutput.height
        visible: !applyShader

        Image {
            id: srcImage
            visible: false
            fillMode: Image.PreserveAspectFit
            width: imageOutput.width
            height: imageOutput.height
            smooth: true
            source: imageOutput.source
        }
    }

    ShaderEffect {
        id: lutShader
        width: imageOutput.width
        height: imageOutput.height
        visible: applyShader

        property variant source: shaderSource
        property variant lut3d: image3dLut
        fragmentShader: fileReader.fragmentShader();
    }

    function showPixelColor(x, y, itemHint)
    {
        shaderSource.grabToImage(function(result) {
            itemHint.setBeforeColorValue(pixelHandler.getColor(result, x, y) );
            result.destroy();
        });

        lutShader.grabToImage(function(result) {
            itemHint.setAfterColorValue(pixelHandler.getColor(result, x, y) );
            result.destroy();
        });
    }
}
