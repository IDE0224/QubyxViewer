#include <QStringList>
#include "ProductParams.h"
#include "GlobalData.h"
#include "GlobalNames.h"

#include <QDebug>


#define PRODUCT_GROUR_INTEGER "integers"
#define PRODUCT_GROUP_DOUBLE "doubles"
#define PRODUCT_GROUP_STRING "strings"
#define PRODUCT_GROUP_PREFERENCES "preferences"
#define PRODUCT_GROUP_DISPLAY "display"

ProductParams::ProductParams(const QString &commonFileName, const QString &specialFileName):iniCommonData_(nullptr),iniSpecialData_(nullptr)
{
    if (commonFileName.length())
        iniCommonData_  = new QSettings(commonFileName,  QSettings::IniFormat);
    
    if (iniCommonData_)
        qDebug() << "iniCommonData_ status:" << iniCommonData_->status();
    else
        qDebug() << "iniCommonData_ is nullptr";

    if (specialFileName.length())
        iniSpecialData_ = new QSettings(specialFileName, QSettings::IniFormat);

    qDebug() << "ProductParams::ProductParams end";
}

ProductParams::~ProductParams()
{
    if (iniCommonData_)
        delete iniCommonData_;

    if (iniSpecialData_)
        delete iniSpecialData_;
}

bool ProductParams::LoadInGlobalData(QSettings* inFile)
{
   QStringList allGroups = inFile->childGroups();
   QStringList allChild;

   std::map<QString, QString> displayPreferences, DefaultPrefs;

   DefaultPrefs = GlobalData::GetValue<std::map<QString, QString> >(GLOBALDATA_DEFAULT_PARAMS);
   displayPreferences = GlobalData::GetValue<std::map<QString, QString> >(GLOBALDATA_DISPLAY_DEFAULT_PARAMS);
   
   for (int i = 0; i < allGroups.size(); i++)
   {
        inFile->beginGroup(allGroups[i]);
        allChild = inFile->childKeys();
      
        for (int j = 0; j < allChild.size(); j++)
        {
            if (allGroups[i] == PRODUCT_GROUR_INTEGER)
            {
                if (!GlobalData::SetValue<int>(allChild[j],inFile->value(allChild[j]).toInt()))
                    return false;
            }

            if (allGroups[i] == PRODUCT_GROUP_DOUBLE)
            {
                if (!GlobalData::SetValue<double>(allChild[j],inFile->value(allChild[j]).toDouble()))
                    return false;
            }
            
            if (allGroups[i] == PRODUCT_GROUP_STRING)
            {
                if (!GlobalData::SetValue<QString>(allChild[j],inFile->value(allChild[j]).toString()))
                    return false;
            }

            if (allGroups[i] == PRODUCT_GROUP_PREFERENCES)
            {
                DefaultPrefs[allChild[j]] = inFile->value(allChild[j]).toString();
            }

            if (allGroups[i] == PRODUCT_GROUP_DISPLAY)
            {
                displayPreferences[allChild[j]] = inFile->value(allChild[j]).toString();
            }
         

        }
        inFile->endGroup();
   }

   GlobalData::SetValue<std::map<QString, QString> >(GLOBALDATA_DEFAULT_PARAMS, DefaultPrefs);
   GlobalData::SetValue<std::map<QString, QString> >(GLOBALDATA_DISPLAY_DEFAULT_PARAMS,displayPreferences);

   allChild.clear();
   allChild = inFile->childKeys();
   for (int i = 0; i < allChild.size(); i++)
   {
        if (!GlobalData::SetValue<QString>(allChild[i],inFile->value(allChild[i]).toString()))
                    return false;
        qDebug()<<"add other - "<<GlobalData::getString(allChild[i]);
   }
   
   return true;
}

bool ProductParams::load()
{
    if (iniCommonData_)
        if (!LoadInGlobalData(iniCommonData_))
            return false;

    if (iniSpecialData_)
        if (!LoadInGlobalData(iniSpecialData_)) 
            return false;
   
        return true;
}

bool ProductParams::LoadParams(const QString& commonFileName, const QString& specialFileName)
{
    ProductParams  productParams(commonFileName,specialFileName);
    return productParams.load();
}
