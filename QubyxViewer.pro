QT += core widgets gui quick multimedia multimediawidgets xml

CONFIG += c++14
CONFIG += qt

TARGET = QubyxViewer
TEMPLATE = app

# Graphics1 support is now provided via DLL

# Source files
SOURCES += \
    main.cpp \
    qubyxviewerdata.cpp \
    LutGenerator.cpp \
    filereader.cpp \
    pixelhandler.cpp \
    ProxyVideoSurface.cpp \
    qmediaplayerwrapper.cpp \
    MediaSourceConnector.cpp \
    viewerwindowinfo.cpp

# Header files
HEADERS += \
    qubyxviewerdata.h \
    LutGenerator.h \
    filereader.h \
    pixelhandler.h \
    ProxyVideoSurface.h \
    qmediaplayerwrapper.h \
    MediaSourceConnector.h \
    viewerwindowinfo.h

# Library source files
SOURCES += \
    Graphics1DisplayStaticWrapper.cpp \
    libs/CalibrotionPresetsManager.cpp \
    libs/CalParams.h \
    libs/CAT.cpp \
    libs/CLUT_stuffer.cpp \
    libs/Color.cpp \
    libs/ColorDistance.cpp \
    libs/ColorTemperature.cpp \
    libs/ColorUnitsConversion.cpp \
    libs/core.cpp \
    libs/CurrentDisplays.cpp \
    libs/DataBus.cpp \
    libs/DICOM.cpp \
    libs/DisplayEnumerator.cpp \
    libs/DisplayInfo.cpp \
    libs/displays.cpp \
    libs/EDIDParser.cpp \
    libs/filehandling.cpp \
    libs/globalnames.cpp \
    libs/GlobalData.cpp \
    libs/icclut.cpp \
    libs/inline.cpp \
    libs/LgMedicalUartConnectionInfo.cpp \
    libs/Matrix.cpp \
    libs/MatrixSolve.cpp \
    libs/mutex.cpp \
    libs/Optimization.cpp \
    libs/ProductParams.cpp \
    libs/profile.cpp \
    libs/ProfileCLUT_Equidistant.cpp \
    libs/QubyxConfiguration.cpp \
    libs/QubyxProfile.cpp \
    libs/qubyxprofilechain.cpp \
    libs/QubyxValue.cpp \
    libs/reg_exp.cpp \
    libs/Regression.cpp \
    libs/SerialPort.cpp \
    libs/SystemInfo.cpp \
    libs/UsefulTools.cpp \
    libs/WinRegistry.cpp \
    libs/qextserialenumerator_win.cpp \
    libs/qextserialenumerator_osx.cpp \
    libs/qextserialenumerator_unix.cpp \
    libs/qextserialport.cpp \

# Library header files
HEADERS += \
    Graphics1DisplayStaticWrapper.h \
    libs/BaseParams.h \
    libs/CalibrotionPresetsManager.h \
    libs/CAT.h \
    libs/CLUT_stuffer.h \
    libs/Color.h \
    libs/ColorDistance.h \
    libs/ColorTemperature.h \
    libs/ColorUnitsConversion.h \
    libs/core.h \
    libs/cppfunc.h \
    libs/CurrentDisplays.h \
    libs/DataBus.h \
    libs/DICOM.h \
    libs/DisplayEnumerator.h \
    libs/DisplayInfo.h \
    libs/displays.h \
    libs/EDIDParser.h \
    libs/filehandling.h \
    libs/GlobalData.h \
    libs/GlobalNames.h \
    libs/icclut.h \
    libs/inline.h \
    libs/Interpolation.h \
    libs/LgMedicalUartConnectionInfo.h \
    libs/MacTools.h \
    libs/mathconst.h \
    libs/Matrix.h \
    libs/MatrixSolve.h \
    libs/mutex.h \
    libs/Optimization.h \
    libs/PrintExpression.h \
    libs/PrintVariable.h \
    libs/ProductParams.h \
    libs/profile.h \
    libs/ProfileCLUT_Equidistant.h \
    libs/QbxDataDef.h \
    libs/QubyxConfiguration.h \
    libs/QubyxProfile.h \
    libs/qubyxprofilechain.h \
    libs/QubyxValue.h \
    libs/reg_exp.h \
    libs/Regression.h \
    libs/SerialPort.h \
    libs/SystemInfo.h \
    libs/UsefulTools.h \
    libs/WinRegistry.h \
    libs/qextserialenumerator.h \
    libs/qextserialport.h \
    libs/VideoCardInfo.h \
    libs/WTSEnumerateProcesses.h

# Curves source files
SOURCES += \
    libs/curves/CIELCurve.cpp \
    libs/curves/CurvesFactory.cpp \
    libs/curves/CustomCurve.cpp \
    libs/curves/DICOMCurve.cpp \
    libs/curves/EPD3Curve.cpp \
    libs/curves/EPDCurve.cpp \
    libs/curves/GammaCurve.cpp \
    libs/curves/rec709_bt1886.cpp \
    libs/curves/Rec709Curve.cpp \
    libs/curves/sRGBCurve.cpp \
    libs/curves/TargetCurve.cpp

# Curves header files
HEADERS += \
    libs/curves/CIELCurve.h \
    libs/curves/CurvesFactory.h \
    libs/curves/CustomCurve.h \
    libs/curves/DICOMCurve.h \
    libs/curves/EPD3Curve.h \
    libs/curves/EPDCurve.h \
    libs/curves/GammaCurve.h \
    libs/curves/rec709_bt1886.h \
    libs/curves/Rec709Curve.h \
    libs/curves/sRGBCurve.h \
    libs/curves/TargetCurve.h

# ICCProfLib source files
SOURCES += \
    libs/ICCProfLib/IccApplyBPC.cpp \
    libs/ICCProfLib/IccCmm.cpp \
    libs/ICCProfLib/IccConvertUTF.cpp \
    libs/ICCProfLib/IccEval.cpp \
    libs/ICCProfLib/IccIO.cpp \
    libs/ICCProfLib/iccMmodTag.cpp \
    libs/ICCProfLib/IccMpeACS.cpp \
    libs/ICCProfLib/IccMpeBasic.cpp \
    libs/ICCProfLib/IccMpeFactory.cpp \
    libs/ICCProfLib/IccPrmg.cpp \
    libs/ICCProfLib/IccProfile.cpp \
    libs/ICCProfLib/IccTagBasic.cpp \
    libs/ICCProfLib/IccTagDict.cpp \
    libs/ICCProfLib/IccTagFactory.cpp \
    libs/ICCProfLib/IccTagLut.cpp \
    libs/ICCProfLib/IccTagMPE.cpp \
    libs/ICCProfLib/IccTagProfSeqId.cpp \
    libs/ICCProfLib/IccUtil.cpp \
    libs/ICCProfLib/IccVcgtTag.cpp \
    libs/ICCProfLib/IccXformFactory.cpp \
    libs/ICCProfLib/md5.cpp

# ICCProfLib header files
HEADERS += \
    libs/ICCProfLib/IccApplyBPC.h \
    libs/ICCProfLib/IccCmm.h \
    libs/ICCProfLib/IccConvertUTF.h \
    libs/ICCProfLib/IccDefs.h \
    libs/ICCProfLib/IccEval.h \
    libs/ICCProfLib/IccIO.h \
    libs/ICCProfLib/iccMmodTag.h \
    libs/ICCProfLib/IccMpeACS.h \
    libs/ICCProfLib/IccMpeBasic.h \
    libs/ICCProfLib/IccMpeFactory.h \
    libs/ICCProfLib/IccPrmg.h \
    libs/ICCProfLib/IccProfile.h \
    libs/ICCProfLib/IccTag.h \
    libs/ICCProfLib/IccTagBasic.h \
    libs/ICCProfLib/IccTagDict.h \
    libs/ICCProfLib/IccTagFactory.h \
    libs/ICCProfLib/IccTagLut.h \
    libs/ICCProfLib/IccTagMPE.h \
    libs/ICCProfLib/IccTagProfSeqId.h \
    libs/ICCProfLib/IccUtil.h \
    libs/ICCProfLib/IccVcgtTag.h \
    libs/ICCProfLib/IccXformFactory.h \
    libs/ICCProfLib/icProfileHeader.h \
    libs/ICCProfLib/IccProfLibConf.h \
    libs/ICCProfLib/IccProfLibVer.h \
    libs/ICCProfLib/MainPage.h \
    libs/ICCProfLib/md5.h

# QML files
RESOURCES += resources.qrc

# Include paths
INCLUDEPATH += libs
INCLUDEPATH += libs/ICCProfLib
# Graphics1 headers are now in the DLL
INCLUDEPATH += libs/Eigen

# Qt Multimedia includes
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtMultimedia
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtMultimediaWidgets

# Windows specific settings
win32 {
    LIBS += -lsetupapi -ladvapi32 -luser32 -lgdi32 -lwinmm -lole32 -loleaut32 -luuid -lshell32 -lversion -lpsapi -lpowrprof -lwtsapi32 -lmscms
    
    DEFINES += WIN32_LEAN_AND_MEAN
    DEFINES += NOMINMAX
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += WINDOWS_IGNORE_PACKING_MISMATCH
    
    # Qt Multimedia specific
    DEFINES += QT_MULTIMEDIA_LIB
}

# macOS specific settings
macx {
    LIBS += -framework IOKit -framework ApplicationServices -framework CoreFoundation -framework CoreGraphics -framework QuartzCore
}

# Linux specific settings
unix:!macx {
    LIBS += -lpthread -ldl
}
