#include "GlobalNames.h"

#include <QObject>

const QString GLOBALDATA_CURRENT_DISPLAY_ID = "currentDisplayId";
const QString GLOBALDATA_DEFAULT_PARAMS    = "default preferences";
const QString GLOBALDATA_DISPLAY_DEFAULT_PARAMS   = "displaypreferences";
const QString GLOBALDATA_LANGUAGE = "language";

//global data from file
extern const QString GD_APPLICATION_NAME  = "ApplicationName";
extern const QString GLOBALDATA_APPLICATION_NAME  = GD_APPLICATION_NAME;
extern const QString GD_APPNAME  = GD_APPLICATION_NAME;
extern const QString GLOBALDATA_REGISTRATION_APPLICATION_NAME = "RegistrationApplicationName";
extern const QString GD_REG_APPNAME = GLOBALDATA_REGISTRATION_APPLICATION_NAME;
extern const QString GLOBALDATA_APPLICATION_FILE_NAME  = "ApplicationFileName";
extern const QString GLOBALDATA_APPLICATION_NAME_PATH  = "ApplicationNamePath";
extern const QString GLOBALDATA_ORGANIZATION_NAME = "CompanyName";
extern const QString GLOBALDATA_COPYRIGHT_COMPANY_NAME = "CopyrightCompanyName";
extern const QString GLOBALDATA_VERSION_CURRENT   = "version";
extern const QString GLOBALDATA_VERSION_BUILD     = "build";
extern const QString GLOBALDATA_VERSIONPREFIX_CURRENT   = "versionPrefix";
extern const QString GLOBALDATA_UPDATE_LINK       = "updatelink";
extern const QString GLOBALDATA_DIRECTORY_NAME_TRANSLATE_FILES  = "TranslateDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_REGULATION_FILES = "RegulationDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_PROFILE_FILES = "ProfileDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_DATABASE         = "DatabaseDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_RESERVE_COPY     = "ReserveCopyDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_SENSORCORRECTION_FILES  = "CorrectionsDirectoryName";
extern const QString GLOBALDATA_DIRECTORY_NAME_PRINTER_MEASURES_FILES  = "PrinterCalMeasurementsDirectoryName";
extern const QString GLOBALDATA_SRC_DIR = "SrcDirPath";

extern const QString GLOBALDATA_INFOLINK_LINK = "InfoLink";
extern const QString GLOBALDATA_INFOLINK_TEXT = "InfoLinkText";
extern const QString GLOBALDATA_SUPPORTLINK_LINK = "SupportLink";
extern const QString GLOBALDATA_SUPPORTLINK_TEXT = "SupportLinkText";
extern const QString GLOBALDATA_EMAIL = "Email";
extern const QString GLOBALDATA_WHATSNEW_LINK = "WhatsNewsLink";
extern const QString GLOBALDATA_SHOP_LINK = "ShopLink";
extern const QString GLOBALDATA_SHOP_LINK_DE = "ShopLink_de";

extern const QString GLOBALDATA_SPECIAL_MESSAGE_PLACE = "SpecialMessagePlace";

extern const QString GLOBALDATA_REGISTER_LINK = "RegisterLink";
extern const QString GLOBALDATA_ACTIVATION_LINK = "ActivationLink";
extern const QString GLOBALDATA_ACTIVATION_API_LINK = "ActivationAPI";

extern const QString GLOBALDATA_DEFAULT_SERIAL = "DefaultSerial";

extern const QString GLOBALDATA_REGULATION_NAMES = "Regulations";
extern const QString GLOBALDATA_CLASSIFICATION_NAMES = "Classifacations";
extern const QString GLOBALDATA_TESTPATTERNS = "TestPatterns";

extern const QString GLOBALDATA_START_PAGE_NUMBER = "StartPageNumber";
extern const QString GLOBALDATA_COLOR_TEMPERATURES = "ColorTemperatures";

extern const QString GLOBALDATA_GUIVIEW          = "GuiView";
extern const QString GLOBALDATA_GUIVIEW_HIDEDISP = "hidedisp";
extern const QString GLOBALDATA_QA_FAST_RUN = "QAFastRun";
extern const QString GLOBALDATA_APPLICATION_TOOLS = "Tools";

extern const QString GLOBALDATA_DISABLE_NONQUBYX_SENSORS = "DisableNonQubyxSensors";
extern const QString GLOBALDATA_DISABLE_GOSSEN_SENSORS = "DisableGossenSensors";
extern const QString GLOBALDATA_DISABLE_CDLUX_SENSORS = "DisableCDLuxSensors";

extern const QString GLOBALDATA_ALWAYS_SHOW_DELTAEL = "AlwaysShowDeltaEL";

extern const QString GLOBALDATA_LUTLOADER_SILENT_ON_REMOTE = "silentOnRemote";

extern const QString GLOBALDATA_IS_AUDI  = "isAudi";
extern const QString GLOBALDATA_IS_OLORIN  = "isOlorin";

extern const QString GLOBALDATA_HIDE_AUTOSTAB = "HideAutoWhiteLevel";
extern const QString GLOBALDATA_SLIDER_WHITELEVEL = "WhiteLevelAsSlider";
extern const QString GLOBALDATA_WHITELEVEL_MIN = "WhiteLevelMin";
extern const QString GLOBALDATA_WHITELEVEL_MIN_DICOM = "WhiteLevelDicomMin";
extern const QString GLOBALDATA_WHITELEVEL_MAX = "WhiteLevelMax";

extern const QString GLOBALDATA_DISABLE_ALS_IN_ACCEPTANCE = "DisableAlsInAcceptance";

//not global data
extern const QString MANUAL_MODE_LUMINANCE  = "luminance";
extern const QString MANUAL_MODE_XYZ        = "xyz";
extern const QString MANUAL_MODE_AMBIENT    = "ambient";


extern const QString PERF_SETWHITELEVEL    = "SetWhiteLevel";
extern const QString PERF_WHITELEVEL       = "WhiteLevel";
extern const QString PERF_COMTYPE       = "CommunicationType";
extern const QString PERF_TYPEOFDISPLAY       = "TypeOfDisplay";
extern const QString PERF_CALTYPE       = "CalibrationType";
extern const QString PERF_HANDHELDSENSOR       = "HandheldSensor";
extern const QString PERF_DEFAMBIENTLIGHT       = "AmbientLight";

extern const QString VERSION_SKIPPED   = "SkippedVersion";
extern const QString UPDATE_ERROR      = "updateError";

extern const QString DIRECTORY_NAME_REPORT_SAVE      = "ReportSavePath";
extern const QString DIRECTORY_EXPORT_SAVE           = "ExportSavePath";
extern const QString DIRECTORY_USER_EXPORT_SAVE      = "ExportUserSavePath";
extern const QString DIRECTORY_CHOOSE_IMAGE          = "ChooseImagePath";
extern const QString LAST_CHOOSE_IMAGE               = "LastImagePath";
extern const QString DIRECTORY_ICC_PROFILE           = "ICCProfileDirectory";
extern const QString DIRECTORY_LOGO_IMAGE            = "logoImageDirectory";
extern const QString DIRECTORY_ICC_EXPORT            =  "ICCExportDirectory";

extern const QString PASSWORD_SETTINGS               = "SettingsPassword";
extern const QString PROTECT_SETTINGS_BY_PASSWORD    = "ProtectSettingsByPassword";

extern const QString PERFECTLUMBINPATH_FOR_LUTLOADER = "PerfectLUM";
extern const QString PERFECTLUM_BUNDLE_PATH = "PerfectLum_bundle";

extern const QString DIALOG_POSITION_TOP_LEFT = "top-left";
extern const QString DIALOG_POSITION_TOP_CENTER = "top-center";
extern const QString DIALOG_POSITION_TOP_RIGHT = "top-right";
extern const QString DIALOG_POSITION_BOTTOM_LEFT = "bottom-left";
extern const QString DIALOG_POSITION_BOTTOM_CENTER = "bottom-center";
extern const QString DIALOG_POSITION_BOTTOM_RIGHT = "bottom-right";
extern const QString DIALOG_POSITION_CENTER = "center";
extern const QString DIALOG_POSITION_CENTER_LEFT = "center-left";
extern const QString DIALOG_POSITION_CENTER_RIGHT = "center-right";
extern const QString DIALOG_POSITION_CENTER_CIRCLE = "center-center";
extern const QString DIALOG_POSITION_CENTER_CIRCLE_DIALOG_LEFT = "center-center-dlg-left";
extern const QString DIALOG_POSITION_CENTER_CIRCLE_DIFFERENT_DISPLAY = "center-center-diff-display";
extern const QString DIALOG_POSITION_CENTER_DIFFERENT_DISPLAY = "center-center-diff-display";

extern const QString GEOMETRY_MAIN_WINDOW = "geometrymainwindow";
extern const QString GEOMETRY_MAIN_ISMAXSIZE = "mainwindowismaximize";
extern const QString GEOMETRY_SETTINGS_WINDOW = "geometrysettingswindow";

extern const QString SHARED_MEMORY_KEY_CALIBRATION = "calibrationprocess";
extern const QString SHARED_MEMORY_KEY_SYNCHRONIZATION = "synchronizationprocess";
extern const QString SHARED_MEMORY_KEY_CANNONDETECT = "canondetectprocess";
extern const QString SHARED_MEMORY_KEY_LUT = "lut";
extern const QString SHARED_MEMORY_KEY_APPCONNECTOR_WRITE = "appconnectorwrite";
extern const QString SHARED_MEMORY_KEY_DB = "dbcreate";
extern const QString SHARED_MEMORY_KEY_SCHEDULING = "schedulingprocess";
extern const QString SHARED_MEMORY_KEY_MASS_CALIBRATION = "masscalprocess";

extern const QString GEOMETRY_SETTINGS_ISMAXSIZE = "settingsismaxsize";

extern const QString IS_REMOTE_ON = "remoteOn";
extern const QString REMOTE_ON = "on";
extern const QString REMOTE_OFF = "off";

extern const QString SYNCH_GROUP_ID = "synchgroupid";

extern const QString APP_CONNECTOR_LICENSE               = "LicenseChange";
extern const QString APP_CONNECTER_DISP_REFRESH          = "DisplayRefresh";
extern const QString APP_CONNECTER_LUTLOADER_DISABLE     = "lutdisable";
extern const QString APP_CONNECTER_LUTLOADER_RESTART     = "lutrestart";
extern const QString APP_CONNECTER_LUTLOADER_LUT_CHANGE  = "lutchange";
extern const QString APP_CONNECTER_LUTLOADER_IS_LOCKED   = "is_locked";
extern const QString APP_CONNECTER_LUTLOADER_LOCK_STATUS = "lock_status";
extern const QString APP_CONNECTER_BCG_CALIBRATION_DELAY = "delay_display";
extern const QString APP_CONNECTER_RAISE_MAIN_WINDOW     = "raise_main_window";

extern const QString APP_CONNECTOR_REGISTRATION_INFO     = "registerInfo";
extern const QString APP_CONNECTOR_REGISTRATION_ANSWER   = "registerAnswer";

//build specific
extern const QString GLOBALDATA_BUILD_NOQA = "GD_build_NoQA";
extern const QString GLOBALDATA_BUILD_NOICC = "GD_build_NoICC";

extern const QString GLOBALDATA_BLOCK_BACKGROUND_CALIBRATION = "BlockBackgroundCalibration";

extern const QString QUBYX_LICENSE_CHANGED = "license_changed";
extern const QString MANUAL_SENSOR_INFORMATION = "sensorInformation";

extern const QString CHECKED_DISPLAYS = "checked_displays";
extern const QString CURRENT_PATTERN = "current_pattern";
extern const QString IMPORT_PROFILE_PATH = "import_profile_path";
extern const QString PRINTER_SAVE_PROFILE_PATH = "printer_save_profile_path";
extern const QString CAMERA_SAVE_PROFILE_PATH = "camera_save_profile_path";
extern const QString PRINTER_SAVE_COLOR_CHART = "printer_save_color_chart";
extern const QString PRINTER_SAVE_COLOR_CHART_DIR = "printer_save_color_chart_dir";

extern const QString GAMUT_ADJUST = "AdjustGamut";
extern const QString GAMUT_NAME = "gamut_name";
extern const QString GAMUT_RED_TARGETX = "gamut_red_x";
extern const QString GAMUT_RED_TARGETY = "gamut_red_y";
extern const QString GAMUT_RED_TARGET_LUM = "gamut_red_lum";
extern const QString GAMUT_GREEN_TARGETX = "gamut_green_x";
extern const QString GAMUT_GREEN_TARGETY = "gamut_green_y";
extern const QString GAMUT_GREEN_TARGET_LUM = "gamut_green_lum";
extern const QString GAMUT_BLUE_TARGETX = "gamut_blue_x";
extern const QString GAMUT_BLUE_TARGETY = "gamut_blue_y";
extern const QString GAMUT_BLUE_TARGET_LUM = "gamut_blue_lum";

extern const QString REMOTE_SERVER_NAME = "server_name";

extern const QString GLOBALDATA_SAVELUT_TOFILE = "SaveLutToFile";

//PP
extern const QString GLOBALDATA_CURRENT_PRINTER_ID = "currentPrinterId";
extern const QString GLOBALDATA_CAMERA_PRINTER_ID = "currentCameraId";

extern const QString GLOBALDATA_SENSOR_NUMBER = "sensor_number";
extern const QString GLOBALDATA_SENSOR_PORT = "sensor_port";

extern const QString GLOBALDATA_LUT_FIRST_RUN = "lutFirstRun";

extern const QString DOCTOR_NAME          = "doctor_name";
extern const QString IMAGE_DESCRIPTION    = "image description";
extern const QString VIEWER_NAME          = "viewer_name";
extern const QString LUX_SENSOR_VENDOR    ="lux vendor";
extern const QString LUX_SENSOR_SERIAL    ="lux serial";
extern const QString LUX_SENSOR_MODEL    ="lux model";

extern const QString LAST_PAGE_MAIN_WINDOW = "last_page";

extern const QString ADDITIONAL_MEAS_DELAY = "AdditionalMeasDelay";
extern const QString ICC_GRID_SIZE = "IccGridSize";

extern const QString GLOBALDATA_IGNORE_WHITELEVEL = "IgnoreWhiteLevel";
extern const QString GLOBALDATA_COLOR_TEMPERATURE_BRIGHTNESS = "AdjustWhiteLevelForEachColorTemperature";
