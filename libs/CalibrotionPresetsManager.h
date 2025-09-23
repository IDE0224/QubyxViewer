#ifndef CALIBROTIONPRESMANAGER_H
#define	CALIBROTIONPRESMANAGER_H
#include <map>
#include <QString>
#include <vector>
#include "QubyxConfiguration.h"

const QString USER_PRESETS_FILE_NAME = "UserPresets";

struct SettingsPreset
{
    SettingsPreset()
    {
        calibrationType_ = "";
        gammaValue_ = "";
        colorTemperatureAdjust_ = "";
        temperatureValue_ = "";
        whiteLevelValue_ = "";
        blackLevelValue_ = "";
        uniformityIndex_ = "";
        adjustPerfectContrast_ = "";
        presetName_ = "";
        gamutName_ = "";
    }

    QString calibrationType_;
    QString gammaValue_;
    QString colorTemperatureAdjust_;
    QString temperatureValue_;
    QString whiteLevelValue_;
    QString blackLevelValue_;
    QString uniformityIndex_;
    QString adjustPerfectContrast_;
    QString gamutName_;
    std::vector<double> gamutValues_;
    
    QString presetName_;//for save
};

class CalibrotionPresetsManager {
    
public:
    virtual ~CalibrotionPresetsManager();
    static void GetCalibrationTypes(std::vector<std::pair<QString, QString> > &typesData);
    static void GetCalibrationPresetsNames(std::vector<std::pair<QString, QString> > &presetsNames, const QString &path);
    static void GetPresetInfoByTagName(QString tagName, SettingsPreset &info, const QString &path);
    static void GetGamutPresetsNames(std::vector<std::pair<QString, QString> > &gamutNames);
    static void GetGamutInfoByTagName(QString tagName, std::vector<double> &info);
    static QString GetGamutTagNameByInfo(std::vector<double> &info);
    static void SavePreset(SettingsPreset &info, const QString &savePath);
    static void RemovePreset(QString presetName, const QString &savePath);

    //cmyk black curve presets
    static void GetCMYKCurvesPresetsNames(std::vector<std::pair<QString, QString> > &curveNames);
    static void GetCMYKCurvesInfoByTagName(QString tagName, std::map<QString, double> &info);
    
    static int presetBorder_;
    
private:
    CalibrotionPresetsManager();
    static bool GetPresetInfo(QubyxConfiguration &conf, QString tagName, SettingsPreset &info);
    static void SavePresetContent(QString content, const QString &savePath);
    
    
    

    

};

#endif	/* CALIBROTIONPRESMANAGER_H */

