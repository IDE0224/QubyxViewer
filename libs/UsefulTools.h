#ifndef USEFULTOOLS_H
#define	USEFULTOOLS_H
#include <QString>
#include <QRect>
#include <QColor>
#include <QPoint>
#include <vector>
#include <map>
#include <cstdint>

#include "Color.h"
#include "QbxDataDef.h"

class QPainter;
class QDialog;
class QDateTimeEdit;
class QWidget;

struct PerfectLumVersion {
    int major;
    int minor;
    int build;
};

class UsefulTools
{
public:
    virtual ~UsefulTools();
    /**
     * This function helped to create small string from large. 
     * @param fullName - large (input) string.
     * @param limitLength - hold the limit length of out string.
     * @param nameTail - hold suffix of out string.
     * @return 
     */

    static PerfectLumVersion appVersion();
//    static QDateTime fromUTCtime(QDateTime dTime);
    static QString NameLimiter(QString fullName, int limitLength, QString nameTail);

    static QString fixNonXMLSymbols(QString s, bool *changed=nullptr);

    static void DrawTarget(QPainter *P, double xCoord, double yCoord, double height, double width);
    static void DrawColorBar(QPainter *P, std::vector<QRect> &rects, QColor &color);
    static QString GetRemoteServerHash(const QString &serverAddress);
    
    static void getColorTemperatures(std::map<int, int> &colorIndex, std::map<int, QString> &colorName);
    static std::vector<QString> getTargetCurves();
    
    static QString calibrationCurveName(QString curve, double gamma);

    static QString calibrationShortDetails(QString curve, double gamma, bool isColor, double targetx, double targety, double temp, bool isIcc, bool isUniformity, bool isGamut);
    static bool getCalibrationTargets(QbxDataMap &targets, QString &curve, double &gamma, bool &isColor, double &targetx, double &targety, double &colorTemperature, bool &isIcc, bool &isUniformity, bool &isGamut);
    
    static double getAutoTargetTemperature(Color white);
    
    static QString getVersionNumber();
    static QString getFullVersionNumber(bool usebuild=true);
    static QString GetStyleData(const QString &stylePath, const QString &dataMapPath);
    
    static QString HideUnhideString(const QString &str, bool hide = true);

    static QString makeHash(QString str);

    static QString GetExpiredText();
    static QString GetExpiredSoonText();

    static QString GetExpiredTextPL4();
    static QString GetExpiredSoonTextPL4();

    static QString getShopLink();
    
    template<class T>
    static QString pointerToString(const T *pointer)
    {
        return QString("0x%1").arg((uintptr_t)pointer, QT_POINTER_SIZE * 2, 16, QChar('0'));
    }
    
    template<class T>
    static T* stringToPointer(QString str)
    {
        return reinterpret_cast<T*>(str.toULongLong(0, 16) );
    }

    static void setCustomCurveName(int id, QString customCurveName);
    
    static QColor RGBAtoRGB(QColor from, QColor background);

    static std::vector<std::vector<double>> transformMeasurementsForMonochrome(const std::vector<std::vector<double>>& meas);

    static bool setQDialogScreenByGeometry(QDialog* dialog);
    static bool setQDialogScreenByPoint(QDialog* dialog, QPoint pt);

    static std::vector<std::vector<int>> makeSameLUTWithAnotherSizes(std::vector<std::vector<int> > &lut, unsigned oldSize, unsigned oldMaxValue,
                                                                     unsigned newSize, unsigned newMaxValue);

    static bool patchQDateTimeEditFormat(QDateTimeEdit *w);
    static bool patchAllQDateTimeEditsFormats(QWidget* parent);

    static bool validEmailAddress(QString str);
      
private:

    UsefulTools();

    static std::map<int, QString> customCurves_;
};

#endif	/* USEFULLTOOLS_H */

