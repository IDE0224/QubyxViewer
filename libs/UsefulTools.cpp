/* 
 * File:   UsefulTools.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on May 5, 2011, 11:55 AM
 */

#include "UsefulTools.h"
#include "CurrentDisplays.h"

#include <QPainter>
#include <QPointF>
#include <QDebug>

#include "md5.h"
#include <QUrl>

#include <cmath>
#include <algorithm>

// Helper function to calculate MD5 hash using the existing ICCProfLib MD5 implementation
static QString calcMD5(const QString& input)
{
    MD5_CTX context;
    unsigned char digest[16];
    char hex[33];
    
    icMD5Init(&context);
    QByteArray data = input.toUtf8();
    icMD5Update(&context, reinterpret_cast<unsigned char*>(data.data()), data.size());
    icMD5Final(digest, &context);
    
    // Convert to hex string
    for (int i = 0; i < 16; i++) {
        sprintf(hex + i * 2, "%02x", digest[i]);
    }
    hex[32] = '\0';
    
    return QString(hex);
}

#include "GlobalData.h"
#include "GlobalNames.h"
#include "ProductParams.h"
#include "curves/CurvesFactory.h"
#include "ColorTemperature.h"
#include "Interpolation.h"
#include "CalParams.h"
#include "CalibrotionPresetsManager.h"

#include <QApplication>
#include <QFile>
#include "reg_exp.h"
#include "inline.h"
#include <QByteArray>
#include <QScreen>
#include <QWindow>
#include <QDialog>
#include <QDateTimeEdit>
#include "QubyxConfiguration.h"
#include "UsefulTools.h"

std::map<int, QString> UsefulTools::customCurves_;

UsefulTools::UsefulTools() {
}


UsefulTools::~UsefulTools() {
}

PerfectLumVersion UsefulTools::appVersion()
{
    PerfectLumVersion version;
    auto version_current = GlobalData::GetValue <QString> (GLOBALDATA_VERSION_CURRENT).split('.');
    version.major = version_current.first().toInt();
    version.minor = version_current.length() > 1 ? version_current.at(1).toInt() : 0;
    version.build = GlobalData::GetValue <QString> (GLOBALDATA_VERSION_BUILD).toInt();
    return version;
}

QString UsefulTools::NameLimiter(QString fullName, int limitLength, QString nameTail)
{
    if (fullName.length() > limitLength && nameTail.length() < limitLength)
    {
        fullName.chop(fullName.length() - limitLength + nameTail.length());

        return fullName + nameTail;
    }
    else
        return fullName;
}

QString UsefulTools::fixNonXMLSymbols(QString s, bool *changed/*=nullptr*/)
{
    QString res = s;
    bool f = false;
    for (int i = 0; i < res.length(); ++i)
        if (res[i] < QChar(0x20) ||
           (res[i] > QChar(0xD7FF) && res[i] < QChar(0xE000)) ||
            res[i] > QChar(0xFFFD)) {
            res[i] = '?';
            f = true;
        }

    if (changed)
        *changed = f;

    return res;
}

void UsefulTools::DrawTarget(QPainter *P, double xCoord, double yCoord, double height, double width)
{ 
    qDebug()<<"Darw Target - "<<xCoord<<" "<<yCoord<<" "<<height<<" "<<width;
    
    //QPainter P(this);
    P->setPen(QPen(Qt::white, 10, Qt::SolidLine));
    P->setBrush(QBrush(Qt::white, Qt::NoBrush));
    
    if (height <= 0 || width <= 0)
        return;
    
    QPointF center;
    
    center.setX(xCoord);
    center.setY(yCoord);
    
    qDebug()<<"Central point - "<<center.x()<<"  "<<center.y();
    
    double radius;
    
    if (width < height)
        radius = width/2;
    else
        radius = height/2;
    
    P->drawEllipse(center, radius,radius);
    P->setBrush(QBrush(Qt::white, Qt::SolidPattern));
    P->drawEllipse(center, radius*0.1, radius*0.1);
    P->setPen(QPen(QColor(90, 170, 255), 2, Qt::SolidLine));
    P->setBrush(QBrush(Qt::white, Qt::NoBrush));
    
    P->drawEllipse(center, radius+5,radius+5);
    P->drawEllipse(center, radius-5,radius-5);
    P->drawEllipse(center, radius*0.1+4, radius*0.1+4);
}

void UsefulTools::DrawColorBar(QPainter* P, std::vector<QRect> &rects, QColor& color)
{
    P->setPen(QPen(color, 2, Qt::SolidLine));
    P->setBrush(QBrush(color));
    
    for (unsigned i = 0; i < rects.size(); i++)
        P->drawRect(rects[i]);
}

QString UsefulTools::GetRemoteServerHash(const QString &serverAddress)
{
    if (!serverAddress.isEmpty()){
        QUrl url(serverAddress);
        //QUrl:: url(source.toUtf8().data());
        // TODO MD5
        return calcMD5(url.host());
    }

    return "";
}

void UsefulTools::getColorTemperatures(std::map<int, int> &colorIndex, std::map<int, QString> &colorName)
{
    std::map<int, int> temps;
    std::map<int, QString> names;
    temps[5000] = 0;        names[5000] = "CIE D50";
    temps[5500] = 1;        names[5500] = "CIE D55";
    temps[6500] = 2;        names[6500] = "CIE D65";
    temps[7500] = 3;        names[7500] = "CIE D75";
    temps[9300] = 4;        names[9300] = "CIE P93";
    
    colorIndex.clear();
    colorName.clear();
    
    qDebug() << "Color temperature element:" << GlobalData::getString(GLOBALDATA_COLOR_TEMPERATURES);
    std::list<QString> color_list = qbx::toStdList(GlobalData::getString(GLOBALDATA_COLOR_TEMPERATURES).split("|"));
    for (std::list<QString>::iterator itr = color_list.begin(); itr != color_list.end();++itr)
    {
        qDebug() << "Color temperature element:" << *itr;
        bool ok;
        int t;
        t = itr->toInt(&ok);
        if (!ok)
            continue;            
        if (temps.find(t)==temps.end())
            continue;
        
        colorIndex[t] = temps[t];
        colorName[t] = names[t];
    }
}

std::vector<QString> UsefulTools::getTargetCurves()
{
    std::vector<std::pair<QString, QString> > typesData;
    CalibrotionPresetsManager::GetCalibrationTypes(typesData);
    
    std::vector<QString> res;
    for (std::vector<std::pair<QString, QString> >::iterator itr = typesData.begin();itr!=typesData.end();++itr)
        res.push_back(itr->second);
    
    return res;
}

QString UsefulTools::calibrationCurveName(QString curve, double gamma)
{
    if (curve.toLower().contains("custom_"))
    {
        qDebug() << "custom curve name:" << curve;
        int id = curve.mid(curve.lastIndexOf("_")+1).toInt();
        qDebug() << "id:" << id << "(" <<curve.mid(curve.lastIndexOf("_")+1).toInt() << curve.lastIndexOf("_") << ")";
        if (customCurves_.find(id)!=customCurves_.end())
        {
            qDebug() << "found:" << customCurves_[id];
            return customCurves_[id];
        }

        return curve;
    }

    return CurvesFactory::curveName(curve, gamma);
}


QString UsefulTools::calibrationShortDetails(QString curve, double gamma, bool isColor, double targetx, double targety, double temp, bool isIcc, bool isUniformity, bool isGamut)
{
    QString res = "(";
    res += UsefulTools::calibrationCurveName(curve, gamma);
    
    if (isColor)
    {
        res += "+";
        if (fabs(targetx)>1e-6 && fabs(targety)>1e-6)
        {
            if (temp == 0)
                temp = ColorTemperature::xytoT(targetx, targety);
            int temperature = temp;//ColorTemperature::xytoT(targetx, targety);
            
            static std::map<int, QString> colorNames;
            if (colorNames.empty())
            {
                std::map<int, int> colorIndex;
                getColorTemperatures(colorIndex, colorNames);
            }
            
            bool found = false;
            for (std::map<int, QString>::const_iterator itr=colorNames.begin();itr!=colorNames.end() && !found;++itr)
                if (abs(itr->first-temperature)<15)
                {
                    found = true;
                    res += itr->second;
                }
            
            if (!found)
            {
                res += QString::number(temperature);
                res += "K";
            }
        }
        else
            res += QObject::tr("Color");
    }
    
    if (isIcc)
        res += "+ICC";
    
    if (isUniformity)
        res += "+" + QObject::tr("Uniformity");
    
    if (isGamut)
        res += "+" + QObject::tr("Gamut");
    
    res += ")";   
    return res;
}

bool UsefulTools::getCalibrationTargets(QbxDataMap &targets, QString &curve, double &gamma, bool &isColor, double &targetx, double &targety, double &colorTemperature, bool &isIcc, bool &isUniformity, bool &isGamut)
{
    curve = targets[base::PARAM_CALTYPE];
    gamma = targets[base::PARAM_GAMMAVALUE].toDouble();
    isColor = targets[base::PARAM_ADJUSTCOLOR]=="true";
    targetx = targets[base::PARAM_TARGETX].toDouble();
    targety = targets[base::PARAM_TARGETY].toDouble();
    colorTemperature = targets[base::PARAM_TEMPERATURE].toDouble();
    isIcc = targets[base::PARAM_CREATEICC]=="true";
    isUniformity = targets[base::PARAM_ADJUSTUNIFORMITY]=="true";
    isGamut = targets[base::PARAM_ADJUST_GAMUT]=="true";
       
    return !curve.isEmpty();
}

double UsefulTools::getAutoTargetTemperature(Color white)
{
    std::map<int, int> colorIndex;
    std::map<int, QString> colorName;
    UsefulTools::getColorTemperatures(colorIndex, colorName);
    
    double cT = ColorTemperature::xytoT(white.x(), white.y());
    
    qDebug() << "getAutoTargetTemperature. white" << cT << "(" << white.x() << white.y() << "-" << white.X() << white.Y() << white.Z() << ")";
    double cE = 1e10;
    double res = 5000;
    for (std::map<int, int>::const_iterator itr=colorIndex.begin();itr!=colorIndex.end();++itr)
        if (fabs(cT - itr->first)<cE)
        {
            cE = fabs(cT - itr->first);
            res = itr->first;
        }
    
    return res;
}

QString UsefulTools::getVersionNumber(){
    return GlobalData::getString(GLOBALDATA_VERSION_CURRENT);
}

QString UsefulTools::getFullVersionNumber(bool usebuild/*=true*/)
{
    QString buildVersion = GlobalData::getString(GLOBALDATA_VERSION_BUILD);
    
    if (!buildVersion.isEmpty())
        buildVersion = " ("+ buildVersion +")";
    
    QString versionPrefix = GlobalData::getString(GLOBALDATA_VERSIONPREFIX_CURRENT);
    QString version = GlobalData::getString(GLOBALDATA_VERSION_CURRENT);
    if (!versionPrefix.isEmpty())
    {
//        QStringList sl = version.split(".");
//        if (sl.size())
//            version = sl[sl.size()-1];
//        version = versionPrefix + "."+ version;
        version = versionPrefix;
    }
    
    if (usebuild)
        version += buildVersion;
    
    return version;
}

QString UsefulTools::GetStyleData(const QString &stylePath, const QString &dataMapPath)
{
  QFile stylef(stylePath);
  if (stylef.open(QIODevice::ReadOnly))
  {
      QString styleData = QString::fromUtf8(stylef.readAll());
      QubyxConfiguration conf(dataMapPath);
      QubyxConfiguration::iterator startElement = conf.GetStartElement();
      
      for (QubyxConfiguration::iterator colIt = startElement.firstchild("colors").firstchild("color"); !colIt.IsNull(); ++colIt)
      {
          styleData.replace("[?"+colIt.attributes()["name"]+"?]", colIt.value());
      }
      
      for (QubyxConfiguration::iterator colIt = startElement.firstchild("pathes").firstchild("path"); !colIt.IsNull(); ++colIt)
      {
          styleData.replace("[?"+colIt.attributes()["name"]+"?]", colIt.value());
      }
      return styleData;
  }
  else
      return "";
}

QString UsefulTools::HideUnhideString(const QString &str, bool hide)
{
    QString newStr;
    
    const QString key = "hackme";
    const int kl = key.toUtf8().length();

    if (hide)
    {
        QByteArray arr = str.toUtf8();
        for (int i=0;i<arr.length();++i)
          arr.data()[i] ^= key.toUtf8().data()[i%kl];

        newStr = arr.toHex();
    }
    else
    {
        QByteArray arr = QByteArray::fromHex(str.toUtf8());
        for (int i=0;i<arr.length();++i)
          arr.data()[i] ^= key.toUtf8().data()[i%kl];
        newStr = QString::fromUtf8(arr);
    }
    
    return newStr;
}

QString UsefulTools::makeHash(QString str)
{
    str += "mneoxc2df35fv40el436vc2zxcv";
    for (int i=0;i<5;++i)
        // TODO MD5
        str = calcMD5(str);
    str = str.left(9);
    return str;
}

QString UsefulTools::GetExpiredText()
{
    QString linkColor = "#2d6981";

    QString arg2 = "<a href=\""+getShopLink()+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\">"+" "+QObject::tr("this link")+" "+"</span></a>";
    QString arg3 = "<a href=\"mailto:"+GlobalData::getString(GLOBALDATA_EMAIL)+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\">"+GlobalData::getString(GLOBALDATA_EMAIL)+"</span></a>";

    return QObject::tr("Your demo period has expired. To continue using a full range of functions, please purchase a license online at %1"
                                " or contact us at %2").arg(arg2).arg(arg3);
}

QString UsefulTools::GetExpiredSoonText()
{
    QString linkColor = "#2d6981";

    QString arg2 = "<a href=\""+getShopLink()+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\"> "+QObject::tr("Buy Link")+" </span></a>";
    QString arg3 = "<a href=\"mailto:"+GlobalData::getString(GLOBALDATA_EMAIL)+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\">"+GlobalData::getString(GLOBALDATA_EMAIL)+"</span></a>";

    return QObject::tr("Your demo period is about to end. If you are interested in obtaining a fully functional license, please contact us at %1 or visit our online shop at %2. Note that after the demo period has expired, the calibration will not be applied to your display.").arg(arg3).arg(arg2);
}

QString UsefulTools::GetExpiredTextPL4()
{
    QString linkColor = "#2d6981";

    QString arg2 = "<a href=\""+getShopLink()+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\">"+" "+QObject::tr("online store")+" "+"</span></a>";

    return QObject::tr("Your demo period has expired. To continue using a full range of functions, please purchase a license from the \"License Activation\" tab in %1 or visit our %2.").arg(GlobalData::getString(GLOBALDATA_APPLICATION_NAME)).arg(arg2);
}

QString UsefulTools::GetExpiredSoonTextPL4()
{
    QString linkColor = "#2d6981";

    QString arg2 = "<a href=\""+getShopLink()+"\"><span style=\" text-decoration: underline; color:"+linkColor+";\"> "+QObject::tr("online store")+" </span></a>";

    return QObject::tr("Your demo period is about to end. If you are interested in obtaining a fully functional license, please purchase from the \"License Activation\" tab in %1 or visit our %2. Note that after the demo period has expired, the calibration will not be applied to your display. ").arg(GlobalData::getString(GLOBALDATA_APPLICATION_NAME)).arg(arg2);
}

QString UsefulTools::getShopLink()
{
    if (GlobalData::getString(GLOBALDATA_LANGUAGE)=="Deutsch" && !GlobalData::getString(GLOBALDATA_SHOP_LINK_DE).isEmpty())
        return GlobalData::getString(GLOBALDATA_SHOP_LINK_DE);

    return GlobalData::getString(GLOBALDATA_SHOP_LINK);
}

void UsefulTools::setCustomCurveName(int id, QString customCurveName)
{
    customCurves_[id] = customCurveName;
}

QColor UsefulTools::RGBAtoRGB(QColor from, QColor background)
{
    double a = from.alphaF();
    return QColor::fromRgbF( (1.0 - a)*background.redF() + a*from.redF(),
                             (1.0 - a)*background.greenF() + a*from.greenF(),
                             (1.0 - a)*background.blueF() + a*from.blueF() );
}

std::vector<std::vector<double>> UsefulTools::transformMeasurementsForMonochrome(const std::vector<std::vector<double>>& meas)
{
    if (meas.empty())
        return meas;
    if (meas[0].size()<6)        //rgb,xyz is minimum understandable format
        return meas;

    std::vector<std::vector<double>> res;
    std::vector<double> t(4,0);
    unsigned elCount = meas[0].size()-3;
    if (elCount==3)
    {
        //rgb case
        for (auto v : meas)
            if (v[0]==v[1] && v[0]==v[2])
            {
                t[0] = v[0];
                for (unsigned i=0;i<3;i++)
                    t[i+1] = v[i+3];
                res.push_back(t);
            }
    }
    else
    {
        //cmyk and cmykN cases
        double max=0;
        for (auto el: meas)
            max =std::max(max, *std::max_element(el.begin(),el.begin()+elCount) );

        for (auto v : meas)
        {
            bool isgray = true;
            for (unsigned i=0;i<elCount && isgray;++i)
                if (i!=3 && v[i]>1e-12)
                    isgray = false;

            if (isgray)
            {
                t[0] = max-v[3];                //for CMYK we need to revert understanding of gray
                for (unsigned i=0;i<3;i++)
                    t[i+1] = v[i+elCount];
                res.push_back(t);
            }
        }
    }

    return res;
}

bool UsefulTools::setQDialogScreenByGeometry(QDialog *dialog)
{
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    if (!qApp)
        return false;

    if (!dialog || !dialog->windowHandle())
        return false;

    QRect r = dialog->geometry();

    auto screens = qApp->screens();
    QScreen* scr=nullptr;
    for (QScreen* screen : screens)
    {
        int x = screen->geometry().x()+screen->geometry().width()/2;
        int y = screen->geometry().y()+screen->geometry().height()/2;

        if (x>r.left() && x<r.right()
        && y>r.top() && y<r.bottom())
        {
            scr = screen;
            break;
        }
    }
    if (!scr)
        return false;

    dialog->windowHandle()->setScreen(scr);
    return true;
#else
    return true;
#endif
}

bool UsefulTools::setQDialogScreenByPoint(QDialog *dialog, QPoint pt)
{
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    if (!qApp)
        return false;

    if (!dialog || !dialog->windowHandle())
        return false;

    QRect r = dialog->geometry();

    auto screens = qApp->screens();
    QScreen* scr=nullptr;
    for (QScreen* screen : screens)
    {
        QRect r = screen->geometry();

        if (pt.x()>r.left() && pt.x()<r.right()
        && pt.y()>r.top() && pt.y()<r.bottom())
        {
            scr = screen;
            break;
        }
    }
    if (!scr)
        return false;

    dialog->windowHandle()->setScreen(scr);
    return true;
#else
    return true;
#endif
}

std::vector<std::vector<int> > UsefulTools::makeSameLUTWithAnotherSizes(std::vector<std::vector<int> > &lut, unsigned oldSize, unsigned oldMaxValue,
                                                                        unsigned newSize, unsigned newMaxValue)
{
    std::vector<std::vector<int>> res(lut.size(), std::vector<int>(newSize, 0));

    std::vector<double> x1(oldSize, 0), x2(newSize, 0);
    for (unsigned i=0;i<oldSize;++i)
        x1[i] = i/(oldSize-1.);
    for (unsigned i=0;i<newSize;++i)
        x2[i] = i/(newSize-1.);

    for (unsigned i=0;i<lut.size();++i)
    {
        Interpolation::SplineAkima2<double, int>(x1, lut[i], x2, res[i]);
        for (auto& v : res[i])
            v = double(v)/(oldMaxValue)*newMaxValue + .5;
    }

    return res;
}

bool UsefulTools::patchQDateTimeEditFormat(QDateTimeEdit *w)
{
    if (!w)
        return false;

    w->setDisplayFormat(QLocale::system().dateFormat(QLocale::ShortFormat));
    return true;
}

bool UsefulTools::patchAllQDateTimeEditsFormats(QWidget *parent)
{
    auto list = parent->findChildren<QDateTimeEdit*>();
    int patched = 0;
    for (auto w : list)
        patched += patchQDateTimeEditFormat(w);
    return patched>0;
}

bool UsefulTools::validEmailAddress(QString str)
{
    return str.length() >= 5 &&
            str.count('@')==1 &&
            str.contains(".") &&
            *str.rbegin() != '@' &&
            *str.rbegin() != '.';
}
