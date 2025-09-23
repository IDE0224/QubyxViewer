#ifndef GLOBALNAMES_H
#define	GLOBALNAMES_H

#include <QString>

extern const QString GLOBALDATA_CURRENT_DISPLAY_ID;
extern const QString GLOBALDATA_DEFAULT_PARAMS;
extern const QString GLOBALDATA_DISPLAY_DEFAULT_PARAMS;
extern const QString GLOBALDATA_LANGUAGE;

//global data from file
extern const QString GD_APPNAME;
extern const QString GD_APPLICATION_NAME;
extern const QString GLOBALDATA_APPLICATION_NAME;
extern const QString GLOBALDATA_REGISTRATION_APPLICATION_NAME;
extern const QString GD_REG_APPNAME;
extern const QString GLOBALDATA_APPLICATION_FILE_NAME;
extern const QString GLOBALDATA_APPLICATION_NAME_PATH;
extern const QString GLOBALDATA_ORGANIZATION_NAME;
extern const QString GLOBALDATA_COPYRIGHT_COMPANY_NAME;
extern const QString GLOBALDATA_VERSION_CURRENT;
extern const QString GLOBALDATA_VERSION_BUILD;
extern const QString GLOBALDATA_VERSIONPREFIX_CURRENT;
extern const QString GLOBALDATA_UPDATE_LINK;
extern const QString GLOBALDATA_DIRECTORY_NAME_TRANSLATE_FILES;
extern const QString GLOBALDATA_DIRECTORY_NAME_REGULATION_FILES;
extern const QString GLOBALDATA_DIRECTORY_NAME_PROFILE_FILES;
extern const QString GLOBALDATA_DIRECTORY_NAME_DATABASE;
extern const QString GLOBALDATA_DIRECTORY_NAME_RESERVE_COPY;
extern const QString GLOBALDATA_DIRECTORY_NAME_SENSORCORRECTION_FILES;
extern const QString GLOBALDATA_DIRECTORY_NAME_PRINTER_MEASURES_FILES;
extern const QString GLOBALDATA_SRC_DIR;

extern const QString GLOBALDATA_INFOLINK_LINK;
extern const QString GLOBALDATA_INFOLINK_TEXT;
extern const QString GLOBALDATA_SUPPORTLINK_LINK;
extern const QString GLOBALDATA_SUPPORTLINK_TEXT;
extern const QString GLOBALDATA_EMAIL;
extern const QString GLOBALDATA_WHATSNEW_LINK;
extern const QString GLOBALDATA_SHOP_LINK;
extern const QString GLOBALDATA_SHOP_LINK_DE;

extern const QString GLOBALDATA_SAVELUT_TOFILE;
extern const QString GLOBALDATA_SPECIAL_MESSAGE_PLACE;

extern const QString GLOBALDATA_REGISTER_LINK;
extern const QString GLOBALDATA_ACTIVATION_LINK;
extern const QString GLOBALDATA_ACTIVATION_API_LINK;

extern const QString GLOBALDATA_DEFAULT_SERIAL;

extern const QString GLOBALDATA_DEFAULT_SERIAL;

extern const QString GLOBALDATA_REGULATION_NAMES;
extern const QString GLOBALDATA_CLASSIFICATION_NAMES;
extern const QString GLOBALDATA_TESTPATTERNS;

extern const QString GLOBALDATA_START_PAGE_NUMBER;
extern const QString GLOBALDATA_COLOR_TEMPERATURES;

extern const QString GLOBALDATA_GUIVIEW;
extern const QString GLOBALDATA_GUIVIEW_HIDEDISP;
extern const QString GLOBALDATA_QA_FAST_RUN;
extern const QString GLOBALDATA_APPLICATION_TOOLS;

extern const QString GLOBALDATA_DISABLE_NONQUBYX_SENSORS;
extern const QString GLOBALDATA_DISABLE_GOSSEN_SENSORS;
extern const QString GLOBALDATA_DISABLE_CDLUX_SENSORS;

extern const QString GLOBALDATA_ALWAYS_SHOW_DELTAEL;

extern const QString GLOBALDATA_LUTLOADER_SILENT_ON_REMOTE;
extern const QString GLOBALDATA_IS_AUDI;
extern const QString GLOBALDATA_IS_OLORIN;

extern const QString GLOBALDATA_HIDE_AUTOSTAB;
extern const QString GLOBALDATA_SLIDER_WHITELEVEL;
extern const QString GLOBALDATA_WHITELEVEL_MIN;
extern const QString GLOBALDATA_WHITELEVEL_MIN_DICOM;
extern const QString GLOBALDATA_WHITELEVEL_MAX;

extern const QString GLOBALDATA_DISABLE_ALS_IN_ACCEPTANCE;

//not global data
extern const QString MANUAL_MODE_LUMINANCE;
extern const QString MANUAL_MODE_XYZ;
extern const QString MANUAL_MODE_AMBIENT;


extern const QString PERF_SETWHITELEVEL;
extern const QString PERF_WHITELEVEL;
extern const QString PERF_COMTYPE;
extern const QString PERF_TYPEOFDISPLAY;
extern const QString PERF_CALTYPE;
extern const QString PERF_HANDHELDSENSOR;
extern const QString PERF_DEFAMBIENTLIGHT;

extern const QString VERSION_SKIPPED;
extern const QString UPDATE_ERROR;

extern const QString DIRECTORY_NAME_REPORT_SAVE;
extern const QString DIRECTORY_EXPORT_SAVE;
extern const QString DIRECTORY_USER_EXPORT_SAVE;
extern const QString DIRECTORY_CHOOSE_IMAGE;
extern const QString LAST_CHOOSE_IMAGE;
extern const QString DIRECTORY_ICC_PROFILE;
extern const QString DIRECTORY_LOGO_IMAGE;
extern const QString DIRECTORY_ICC_EXPORT;

extern const QString PASSWORD_SETTINGS;
extern const QString PROTECT_SETTINGS_BY_PASSWORD;

extern const QString PERFECTLUMBINPATH_FOR_LUTLOADER;
extern const QString PERFECTLUM_BUNDLE_PATH;

extern const QString DIALOG_POSITION_TOP_LEFT;
extern const QString DIALOG_POSITION_TOP_CENTER;
extern const QString DIALOG_POSITION_TOP_RIGHT;
extern const QString DIALOG_POSITION_BOTTOM_LEFT;
extern const QString DIALOG_POSITION_BOTTOM_CENTER;
extern const QString DIALOG_POSITION_BOTTOM_RIGHT;
extern const QString DIALOG_POSITION_CENTER;
extern const QString DIALOG_POSITION_CENTER_LEFT;
extern const QString DIALOG_POSITION_CENTER_RIGHT;
extern const QString DIALOG_POSITION_CENTER_CIRCLE;
extern const QString DIALOG_POSITION_CENTER_CIRCLE_DIALOG_LEFT;
extern const QString DIALOG_POSITION_CENTER_CIRCLE_DIFFERENT_DISPLAY;
extern const QString DIALOG_POSITION_CENTER_DIFFERENT_DISPLAY;

extern const QString GEOMETRY_MAIN_WINDOW;
extern const QString GEOMETRY_MAIN_ISMAXSIZE;
extern const QString GEOMETRY_SETTINGS_WINDOW;

extern const QString SHARED_MEMORY_KEY_CALIBRATION;
extern const QString SHARED_MEMORY_KEY_SYNCHRONIZATION;
extern const QString SHARED_MEMORY_KEY_CANNONDETECT;
extern const QString SHARED_MEMORY_KEY_LUT;
extern const QString SHARED_MEMORY_KEY_APPCONNECTOR_WRITE;
extern const QString SHARED_MEMORY_KEY_DB;
extern const QString SHARED_MEMORY_KEY_SCHEDULING;
extern const QString SHARED_MEMORY_KEY_MASS_CALIBRATION;

extern const QString GEOMETRY_SETTINGS_ISMAXSIZE;

extern const QString IS_REMOTE_ON;
extern const QString REMOTE_ON;
extern const QString REMOTE_OFF;

extern const QString SYNCH_GROUP_ID;

extern const QString APP_CONNECTOR_LICENSE;
extern const QString APP_CONNECTER_DISP_REFRESH;
extern const QString APP_CONNECTER_LUTLOADER_DISABLE;
extern const QString APP_CONNECTER_LUTLOADER_RESTART;
extern const QString APP_CONNECTER_LUTLOADER_LUT_CHANGE;
extern const QString APP_CONNECTER_LUTLOADER_IS_LOCKED;
extern const QString APP_CONNECTER_LUTLOADER_LOCK_STATUS;
extern const QString APP_CONNECTER_BCG_CALIBRATION_DELAY;
extern const QString APP_CONNECTER_RAISE_MAIN_WINDOW;

extern const QString APP_CONNECTOR_REGISTRATION_INFO;
extern const QString APP_CONNECTOR_REGISTRATION_ANSWER;

//build specific
extern const QString GLOBALDATA_BUILD_NOQA;
extern const QString GLOBALDATA_BUILD_NOICC;

extern const QString GLOBALDATA_BLOCK_BACKGROUND_CALIBRATION;

#define QA_REQUIREMENT  QObject::tr("Minimum Requirement")
#define QA_RECOMMENDED  QObject::tr("Recommended Performance")
#define QA_HIGH_REQUIREMENT  QObject::tr("High Quality")

enum DiplayCommFlags
  {
      DISP_COMM_TYPE_GBOARD = 0x01,
      DISP_COMM_TYPE_DDCCI  = 0x02,
      DISP_COMM_TYPE_SERIAL = 0x04,
      DISP_COMM_TYPE_USB    = 0x08,
      DISP_COMM_TYPE_FIRST  = 0x10,
  };

extern const QString QUBYX_LICENSE_CHANGED;
extern const QString MANUAL_SENSOR_INFORMATION;

extern const QString CHECKED_DISPLAYS;
extern const QString CURRENT_PATTERN;
extern const QString IMPORT_PROFILE_PATH;
extern const QString PRINTER_SAVE_PROFILE_PATH;
extern const QString CAMERA_SAVE_PROFILE_PATH;
extern const QString PRINTER_SAVE_COLOR_CHART;
extern const QString PRINTER_SAVE_COLOR_CHART_DIR;

extern const QString GAMUT_ADJUST;
extern const QString GAMUT_NAME;
extern const QString GAMUT_RED_TARGETX;
extern const QString GAMUT_RED_TARGETY;
extern const QString GAMUT_RED_TARGET_LUM;
extern const QString GAMUT_GREEN_TARGETX;
extern const QString GAMUT_GREEN_TARGETY;
extern const QString GAMUT_GREEN_TARGET_LUM;
extern const QString GAMUT_BLUE_TARGETX;
extern const QString GAMUT_BLUE_TARGETY;
extern const QString GAMUT_BLUE_TARGET_LUM;

extern const QString REMOTE_SERVER_NAME;


//PP
extern const QString GLOBALDATA_CURRENT_PRINTER_ID;
extern const QString GLOBALDATA_CAMERA_PRINTER_ID;

extern const QString GLOBALDATA_SENSOR_NUMBER;
extern const QString GLOBALDATA_SENSOR_PORT;

extern const QString GLOBALDATA_LUT_FIRST_RUN;

extern const QString DOCTOR_NAME;
extern const QString VIEWER_NAME;
extern const QString IMAGE_DESCRIPTION;
extern const QString LUX_SENSOR_VENDOR;
extern const QString LUX_SENSOR_SERIAL;
extern const QString LUX_SENSOR_MODEL;

extern const QString LAST_PAGE_MAIN_WINDOW;

extern const QString ADDITIONAL_MEAS_DELAY;
extern const QString ICC_GRID_SIZE;

extern const QString GLOBALDATA_IGNORE_WHITELEVEL;
extern const QString GLOBALDATA_COLOR_TEMPERATURE_BRIGHTNESS;


#endif	/* GLOBALNAMES_H */
