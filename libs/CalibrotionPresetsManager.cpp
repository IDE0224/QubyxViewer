#include "CalibrotionPresetsManager.h"
#include <QDebug>
#include <QFile>
#include <cmath>

int CalibrotionPresetsManager::presetBorder_ = 0;

CalibrotionPresetsManager::CalibrotionPresetsManager()
{
}

CalibrotionPresetsManager::~CalibrotionPresetsManager()
{
}

void CalibrotionPresetsManager::GetCalibrationTypes(std::vector<std::pair<QString, QString> >& typesData)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");
    QubyxConfiguration::iterator typesIt = conf.firstchild("caltypes");

    for (QubyxConfiguration::iterator typeIt = typesIt.firstchild("type"); !typeIt.IsNull(); ++typeIt)
        typesData.push_back(std::make_pair(typeIt.attributes()["name"], typeIt.attributes()["tagname"]));
}

void CalibrotionPresetsManager::GetCalibrationPresetsNames(std::vector<std::pair<QString, QString> >& presetsNames, const QString &path)
{
    QubyxConfiguration user_conf(path, USER_PRESETS_FILE_NAME+".xml");

    for (QubyxConfiguration::iterator presetIt = user_conf.firstchild("preset"); !presetIt.IsNull(); ++presetIt)
            presetsNames.push_back(std::make_pair(presetIt.attributes()["name"], presetIt.attributes()["tagname"]));
    
    presetBorder_ = presetsNames.size();
    
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");

    for (QubyxConfiguration::iterator presetIt = conf.firstchild("preset"); !presetIt.IsNull(); ++presetIt)
        presetsNames.push_back(std::make_pair(presetIt.attributes()["name"], presetIt.attributes()["tagname"]));
}

void CalibrotionPresetsManager::GetPresetInfoByTagName(QString tagName, SettingsPreset& info, const QString &path)
{
    qDebug()<<"GetPresetInfoByTagName!!!";
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");
    
    if (GetPresetInfo(conf, tagName, info))
        return;
    
     QubyxConfiguration userconf(path, USER_PRESETS_FILE_NAME+".xml");
     GetPresetInfo(userconf, tagName, info);
}

void CalibrotionPresetsManager::GetGamutPresetsNames(std::vector<std::pair<QString, QString> > &gamutNames)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");

    for (QubyxConfiguration::iterator gamutIt = conf.firstchild("gamut"); !gamutIt.IsNull(); ++gamutIt)
        gamutNames.push_back(std::make_pair(gamutIt.attributes()["name"], gamutIt.attributes()["tagname"]));
}

void CalibrotionPresetsManager::GetGamutInfoByTagName(QString tagName, std::vector<double> &info)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");

    for (QubyxConfiguration::iterator gamutIt = conf.firstchild("gamut"); !gamutIt.IsNull(); ++gamutIt)
    {
        qDebug()<<"Tag names compare - "<<gamutIt.attributes()["tagname"]<<"   "<<tagName;
        if (gamutIt.attributes()["tagname"] == tagName)
        {
            QubyxConfiguration::iterator targetIt = gamutIt.firstchild("target");
            while(!targetIt.IsNull())
            {
                info.push_back(targetIt.value().toDouble());
                ++targetIt;
            }
            break;
        }
    }
}

QString CalibrotionPresetsManager::GetGamutTagNameByInfo(std::vector<double> &info)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");

    for (QubyxConfiguration::iterator gamutIt = conf.firstchild("gamut"); !gamutIt.IsNull(); ++gamutIt)
    {
        QubyxConfiguration::iterator targetIt = gamutIt.firstchild("target");
        int i = 0;
        bool res = true;
        while(!targetIt.IsNull())
        {
            if (std::abs(info[i] - targetIt.value().toDouble())>1e-9)
            {
                res = false;
                break;
            }
            
            ++targetIt;
            i++;
        }
        
        if (res)
            return gamutIt.attributes()["tagname"];
    }
     
    return "";
}

void CalibrotionPresetsManager::SavePreset(SettingsPreset &info, const QString &savePath)
{
    qDebug()<<"SavePreset - "<<savePath<<"/"<<USER_PRESETS_FILE_NAME+".xml";
    
    QubyxConfiguration conf(savePath, USER_PRESETS_FILE_NAME+".xml");
    
    if (conf.GetContent().isEmpty())
        conf.CreateEmpty("calpresets");
    
    QubyxConfiguration::iterator presetIt;
    
    for (presetIt = conf.firstchild("preset"); !presetIt.IsNull(); ++presetIt)
           if (presetIt.attributes()["tagname"] == info.presetName_)
               break;

    if (presetIt.IsNull())
    {
       QubyxConfiguration::iterator rootIt = conf.GetStartElement();
       presetIt = rootIt.addchild("preset");
       presetIt.setattribute("name", info.presetName_);
       presetIt.setattribute("tagname", info.presetName_);
    }

    QubyxConfiguration::iterator calTypeIt = presetIt.firstchild("caltype");
    if (calTypeIt.IsNull())
        calTypeIt = presetIt.addchild("caltype");

    calTypeIt.setattribute("name", info.calibrationType_);

    if (!info.gammaValue_.isEmpty())
    {
        QubyxConfiguration::iterator gammaIt = presetIt.firstchild("gamma");
        if (gammaIt.IsNull())
            gammaIt = presetIt.addchild("gamme");

        gammaIt.setattribute("name", info.gammaValue_);
    }

    if (!info.colorTemperatureAdjust_.isEmpty())
    {
        QubyxConfiguration::iterator colTemprIt = presetIt.firstchild("colortempr");
        if (colTemprIt.IsNull())
            colTemprIt = presetIt.addchild("colortempr");

        colTemprIt.setattribute("name", info.colorTemperatureAdjust_);
    }
    
    if (!info.whiteLevelValue_.isEmpty())
    {
        QubyxConfiguration::iterator whiteLevelIt = presetIt.firstchild("whitelevel");
        if (whiteLevelIt.IsNull())
            whiteLevelIt = presetIt.addchild("whitelevel");

        whiteLevelIt.setattribute("value", info.whiteLevelValue_);
    }

    if (info.gamutName_.isEmpty())
    {
        QubyxConfiguration::iterator gamutIt = presetIt.firstchild("gamut");
        if (gamutIt.IsNull())
            gamutIt = presetIt.addchild("gamut");

        gamutIt.setattribute("gamut", info.gamutName_);

        QubyxConfiguration::iterator gamutTargetIt = gamutIt.firstchild("target");

        for (const auto& gamutValues : info.gamutValues_)
        {
            if (gamutTargetIt.IsNull())
                    gamutTargetIt = gamutIt.addchild("target");

             gamutTargetIt.setValue(QString::number(gamutValues));
             ++gamutTargetIt;
        }
    }
    
    presetBorder_++;
    SavePresetContent(conf.GetContent(), savePath);
}

bool CalibrotionPresetsManager::GetPresetInfo(QubyxConfiguration &conf, QString tagName, SettingsPreset &info)
{
    for (QubyxConfiguration::iterator presetIt = conf.firstchild("preset"); !presetIt.IsNull(); ++presetIt)
    {
        qDebug()<<"Tag names compare - "<<presetIt.attributes()["tagname"]<<"   "<<tagName;
        if (presetIt.attributes()["tagname"] == tagName)
        {
            QubyxConfiguration::iterator adjustPerfContrIt = presetIt.firstchild("perfcontrast");
            if (!adjustPerfContrIt.IsNull())
                info.adjustPerfectContrast_ = "true";

            QubyxConfiguration::iterator calTypeIt = presetIt.firstchild("caltype");
            if (!calTypeIt.IsNull())
            {
                qDebug()<<"Calibration Type = "<<calTypeIt.attributes()["name"];
                info.calibrationType_ = calTypeIt.attributes()["name"];
            }
            
            QubyxConfiguration::iterator gammaIt = presetIt.firstchild("gamma");
            if (!gammaIt.IsNull())
            {
                qDebug()<<"Gamma = "<< gammaIt.attributes()["name"];
                info.gammaValue_ = gammaIt.attributes()["name"];
            }

            QubyxConfiguration::iterator colTemprIt = presetIt.firstchild("colortempr");
            if (!colTemprIt.IsNull())
            {
                qDebug()<<"Color temperature = "<<colTemprIt.attributes()["name"];
                info.colorTemperatureAdjust_ = colTemprIt.attributes()["name"];
            }

            QubyxConfiguration::iterator whiteLevelIt = presetIt.firstchild("whitelevel");
            if (!whiteLevelIt.IsNull())
            {
                qDebug()<<"White level - "<<whiteLevelIt.attributes()["value"];
                info.whiteLevelValue_ = whiteLevelIt.attributes()["value"];
            }
            
            QubyxConfiguration::iterator blaclevelIt = presetIt.firstchild("blacklevel");
            if (!whiteLevelIt.IsNull())
            {
                qDebug()<<"Black level - "<<blaclevelIt.attributes()["value"];
                info.blackLevelValue_ = blaclevelIt.attributes()["value"];
            }
            
            QubyxConfiguration::iterator gamutIt = presetIt.firstchild("gamut");
            if (!gamutIt.IsNull())
            {
                qDebug()<<"Gamut - "<<gamutIt.attributes()["name"];
                info.gamutName_ = gamutIt.attributes()["name"];
                
                QubyxConfiguration::iterator targetIt = gamutIt.firstchild("target");
                while(!targetIt.IsNull())
                {
                    info.gamutValues_.push_back(targetIt.value().toDouble());
                    ++targetIt;
                }   
            }

            return true;
        }
    }
     
    return false;
}

void CalibrotionPresetsManager::SavePresetContent(QString content, const QString &savePath)
{
    QFile file(savePath+"/"+USER_PRESETS_FILE_NAME+".xml");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(content.toUtf8());
        file.close();
    }
}

void CalibrotionPresetsManager::RemovePreset(QString presetName, const QString &savePath)
{
    QubyxConfiguration conf(savePath, USER_PRESETS_FILE_NAME+".xml");
    
    if (conf.GetContent().isEmpty())
        conf.CreateEmpty("calpresets");
    
    QubyxConfiguration::iterator presetIt;
    
    for (presetIt = conf.firstchild("preset"); !presetIt.IsNull(); ++presetIt)
           if (presetIt.attributes()["tagname"] == presetName)
           {
               std::vector<std::pair<QString, QString> > attributes;
               std::pair<QString, QString> el;
               el.first = "tagname";
               el.second = presetName;
               attributes.push_back(el);
      
               conf.GetStartElement().removeChilds("preset", attributes);
               presetBorder_--;
           }
                       
    SavePresetContent(conf.GetContent(), savePath);
}

void CalibrotionPresetsManager::GetCMYKCurvesPresetsNames(std::vector<std::pair<QString, QString> > &curveNames)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");

    curveNames.clear();
    for (QubyxConfiguration::iterator gamutIt = conf.firstchild("cmykCurve"); !gamutIt.IsNull(); ++gamutIt)
        curveNames.push_back(std::make_pair(gamutIt.attributes()["name"], gamutIt.attributes()["tagname"]));
}

void CalibrotionPresetsManager::GetCMYKCurvesInfoByTagName(QString tagName, std::map<QString, double> &info)
{
    QubyxConfiguration conf(":/rc", "CalibrationPresets.xml");
    info.clear();
    for (QubyxConfiguration::iterator gamutIt = conf.firstchild("cmykCurve"); !gamutIt.IsNull(); ++gamutIt)
    {
        if (gamutIt.attributes()["tagname"] == tagName)
        {
            QubyxConfiguration::iterator targetIt;

            targetIt = gamutIt.firstchild("blackMin");
            info["BlackStart"] = targetIt.IsNull() ? 0.0 : targetIt.value().toDouble();

            targetIt = gamutIt.firstchild("blackMax");
            info["BlackMaximum"] = targetIt.IsNull() ? 100.0 : targetIt.value().toDouble();

            targetIt = gamutIt.firstchild("blackGamma");
            info["BlackCurve"] = targetIt.IsNull() ? 2 : targetIt.value().toDouble();

            targetIt = gamutIt.firstchild("blackWidth");
            info["BlackCurveWidth"] = targetIt.IsNull() ? 100.0 : targetIt.value().toDouble();

            targetIt = gamutIt.firstchild("TIC");
            info["BlackTotalInk"] = targetIt.IsNull() ? 400.0 : targetIt.value().toDouble();

            break;
        }
    }
}

