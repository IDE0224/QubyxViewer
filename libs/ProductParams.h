#ifndef PRODUCTPARAMS_H
#define	PRODUCTPARAMS_H

#include <QSettings>


class QSettings;
class QString;

class ProductParams
{
      ProductParams(const QString &commonFileName = "", const QString &specialFileName = "");
      bool load();
public: 
    virtual ~ProductParams();
    static bool LoadParams(const QString &commonFileName, const QString &specialFileName);
   
private:
    QSettings *iniCommonData_=nullptr;
    QSettings *iniSpecialData_=nullptr;

    bool LoadInGlobalData(QSettings* inFile);
};

#endif	/* _PRODUCTPARAMS_H */

