#ifndef BASEPARAMS_H
#define	BASEPARAMS_H
#include <QString>

namespace base
{
    const QString PARAM_PATCHCOUNT       = "patchcount";
    const QString PARAM_MEASURETIME      =   "measuretime";
    const QString PARAM_MINLIMIT         =      "minlimit";
    const QString PARAM_MAXLIMIT         =      "maxlimit";
    const QString PARAM_STEP             =          "step";
    const QString PARAM_PROGRESSDIVIDER  =   "progressevery";
    const QString PARAM_TARGET_DE        =    "targetde";
    const QString PARAM_ENOUGH_DE       =     "enoughde";
    const QString PARAM_ACCEPT_DE =    "acceptde";
    const QString PARAM_INCREASEPROGRESS =     "increase";
    const QString PARAM_CRITICALDIST = "criticaldist";
    const QString PARAM_ISCOLOR =      "iscolor";
    const QString PARAM_SILENT =       "silent";
    const QString PARAM_NOCLEAN =      "noclean";
    const QString PARAM_CLEAN_BUFFER = "cleanbuffer";
    const QString PARAM_POINTS =       "points";

    const QString PARAM_MEASURE_REPEAT =   "repeat measurements";

    const QString PARAM_NAME =         "name";
    const QString PARAM_ADDITIONAL =   "additional";
    const QString PARAM_PROCESSTYPE =  "type of process";

    const QString PARAM_TITLE =        "title";
    const QString PARAM_SHOWDETAILS =  "showdetails";
    const QString PARAM_HIDEICC =      "hideicc";

    const QString PARAM_GAMUT_MAME =       "gamut name";
    const QString PARAM_CALTYPE =          "calibration type";
    const QString PARAM_GAMMAVALUE =       "gamma value";
    const QString PARAM_ADJUSTCOLOR =      "adjust color";
    const QString PARAM_TARGETX =          "color targetx";
    const QString PARAM_TARGETY =          "color targety";
    const QString PARAM_TEMPERATURE =      "color temperature";
    const QString PARAM_ADJUST_GAMUT =     "adjust gamut";
    const QString PARAM_RED_TARGETX =      "red targetx";
    const QString PARAM_RED_TARGETY =      "red targety";
    const QString PARAM_RED_TARGET_LUM =    "red target lum";
    const QString PARAM_GREEN_TARGETX =    "green targetx";
    const QString PARAM_GREEN_TARGETY =    "green targety";
    const QString PARAM_GREEN_TARGET_LUM = "green target lum";
    const QString PARAM_BLUE_TARGETX =     "blue targetx";
    const QString PARAM_BLUE_TARGETY =     "blue targety";
    const QString PARAM_BLUE_TARGET_LUM =  "blue target lum";
    const QString PARAM_ADJUSTWHITE =      "adjust white";
    const QString PARAM_WHITELEVEL =        "white value";
    // for multi kostec only (multi LUT)
    const QString PARAM_MAX_WHITELEVEL =   "max white level";
    const QString PARAM_MIN_WHITELEVEL =   "min white level";
    const QString PARAM_MASTER_COLORTYPE_WHITELEVEL = "use master color type white level";

    const QString PARAM_WHITELEVEL_COEF =  "whitelevel coef";
    const QString PARAM_ADJUSTUNIFORMITY = "adjust uniformity";
    const QString PARAM_UNIFORMITY =       "uniformity value";
    const QString PARAM_ADJUSTCONTRAST =   "adjust contrast";
    const QString PARAM_CREATEICC =         "create icc profile";
    const QString PARAM_CREATEICCTRIAL =   "create icc profile in trial";
    const QString PARAM_AMBIENT =          "ambient";
    const QString PARAM_EXPRESSCAL =       "express calibration";
    const QString PARAM_EXPRESSCALID =     "express calibration history id";
    const QString PARAM_SENSORONDIST =     "distant sensor";
    const QString PARAM_ICC_CHROMATYPE =   "icc chromatic adaptation type";
    const QString PARAM_ICC_GRIDSIZE =     "icc grid size";
    const QString PARAM_BLACKLEVEL =       "black value";
    const QString PARAM_INCREASE =         "increase";
    const QString PARAM_NOSKIP =           "noskip";
    const QString PARAM_ONLYDISPLAY =      "onlydisplay";
    const QString PARAM_MASTERSET =        "masterset";
    const QString PARAM_MASTERCOLOR =      "mastercolor";
    const QString PARAM_INITVCGT =         "initVCGT";
    const QString PARAM_LUT_INDEX =        "lut index";

    const QString PARAM_LUMINANCE_UNITS =   "luminance units";

    const QString PARAM_AUTO_RUN =          "auto run";

    const QString PARAM_DISPLAY_TYPE =      "display type";
    const QString PARAM_DISPLAY_TECHNOLOGY = "display technology";
    const QString PARAM_DISPLAY_COMMTYPE =  "communication type";
    const QString PARAM_DISPLAY_SPECIAL_REGISTRATION =  "display special registration";

    const QString PARAM_MULTILUT =          "multiple lut";
    const QString PARAM_IS_ALS   =          "isALS";

    const QString PARAM_SKIP_MEASURE_DELAY= "skipmeasuredelay";
    const QString PARAM_IF_ICC_IMPOSSIBLE =       "if_icc_impossible";
    const QString PARAM_PARAMONLY =       "paramonly";

    const QString PARAM_CHECK_DISPLAY_WARMED = "check_display_warmed";

    const QString PARAM_DEVICE_INFO = "device_info";

    const QString PARAM_HISTORY_ID  = "historyId";
    const QString PARAM_ICC_VERSION  = "icc_version";
    const QString PARAM_CONFORMANCE_POINTS = "conformance points";
    const QString PARAM_IS_CONFORMANCE = "isconformance";
    const QString PARAM_PRINTER_MEDIA_WEDGE = "media wedge";
    const QString PARAM_PRINTER_MEDIA_WEDGE_TITLE = "media wedge title";

    const QString PARAM_MASS_CALIBRATION_START = "mass calibration run";

    const QString PARAM_ADDITIONAL_MEAS_DELAY = "AdditionalMeasDelay";

    const QString PARAM_WHITELEVEL_FOR_EACH_CT = "WhiteLevelForEachCT";

    const QString PARAM_IS_ACCEPTANCE = "isAcceptance";

    const QString PARAM_ADJUST_PRESET = "AdjustPreset";

    const QString PARAM_DISABLE_ALS_IN_ACCEPTANCE = "DisableAlsInAcceptance";
}

#endif	/* BASEPARAMS_H */