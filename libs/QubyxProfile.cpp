#include "QubyxProfile.h"

#include "ICCProfLib/IccTagBasic.h"
#include "ICCProfLib/IccUtil.h"
#include "ICCProfLib/IccTagLut.h"
#include "ICCProfLib/IccEval.h"
#include "ICCProfLib/IccPrmg.h"
#include "icclut.h"
#include "CAT.h"

#include "Interpolation.h"
#include "GlobalData.h"
#include "GlobalNames.h"
#include "ColorDistance.h"
#include "ColorTemperature.h"
#include "profile.h"
#include "qubyxprofilechain.h"

#include <QtCore/qglobal.h>
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <algorithm>
#include <cmath>
#include <vector>
#include <type_traits>

#include "filehandling.h"
#include <QDate>

#define TRCSIZE 1024

QubyxProfile::QubyxProfile()
{
    devDim_ = 3;
    maxY_ = 1;
    savable_ = true;
    
    for (int i=0;i<9;++i)
    {
        convertChroma_[i] = 0;
        convertRevChroma_[i] = 0;
    }
        
    inColorSpace_ = icSigRgbData;
    outColorSpace_ = icSigXYZData;

    spec_ = ICCSpec::ICCv4;
    skipDescrType_ = false;
    
    InitProfileCatalog();
}

QubyxProfile::QubyxProfile(QString profilePath)
{
    devDim_ = 3;
    maxY_ = 1;
    savable_ = true;
    setFileName(profilePath);
    
    for (int i=0;i<9;++i)
    {
        convertChroma_[i] = 0;
        convertRevChroma_[i] = 0;
    }
        
    inColorSpace_ = icSigRgbData;
    outColorSpace_ = icSigXYZData;

    spec_ = ICCSpec::ICCv4;
    skipDescrType_ = false;
    
    InitProfileCatalog();
}

QubyxProfile::QubyxProfile(const QubyxProfile &other)
    : devDim_(other.devDim_),
      whiteXYZ_(other.whiteXYZ_),
      inColorSpace_(other.inColorSpace_),
      outColorSpace_(other.outColorSpace_),
      maxY_(other.maxY_),
      optDescription_(other.optDescription_),
      skipDescrType_(other.skipDescrType_),
      spec_(other.spec_),
      filename_(other.filename_),
      savable_(other.savable_),
      profile_(other.profile_),
      deviceName_(other.deviceName_)
{
   std::copy(other.convertChroma_, other.convertChroma_+9, convertChroma_);
   std::copy(other.convertRevChroma_, other.convertRevChroma_+9, convertRevChroma_);
}

QubyxProfile::~QubyxProfile()
{
}

QubyxProfile &QubyxProfile::operator=(const QubyxProfile &other)
{
    if (this==&other)
        return *this;

    devDim_ = other.devDim_;
    whiteXYZ_ = other.whiteXYZ_ ;
    inColorSpace_ = other.inColorSpace_ ;
    outColorSpace_ = other.outColorSpace_ ;
    maxY_ = other.maxY_ ;
    optDescription_ = other.optDescription_ ;
    skipDescrType_ = other.skipDescrType_ ;
    spec_ = other.spec_ ;
    filename_ = other.filename_ ;
    savable_ = other.savable_ ;
    profile_ = other.profile_ ;
    deviceName_ = other.deviceName_;

    std::copy(other.convertChroma_, other.convertChroma_+9, convertChroma_);
    std::copy(other.convertRevChroma_, other.convertRevChroma_+9, convertRevChroma_);

    return *this;
}

void QubyxProfile::setFileName(QString filename)
{
    filename_ = filename;
}

QString QubyxProfile::profilePath()
{
    return filename_;
}

void QubyxProfile::InitProfileCatalog()
{  
    QString catalog = OSProfile::profilepath();
#ifdef Q_OS_MAC
    catalog += QString("/")+GlobalData::getString(GLOBALDATA_ORGANIZATION_NAME);
    
    QDir profileDir;
    profileDir.mkpath(catalog);
    OSUtils::setFullPermissions(catalog, true);
#endif
    
    QDir dir(catalog); 
    QStringList flist;
    flist<<"*.icc";
    flist<<"*.icm";
    for (QFileInfo f: dir.entryInfoList(flist))
    {
        OSUtils::setFullPermissions(f.absolutePath(), true);
    }
    
}

QString QubyxProfile::makeProfileTitle(QString type)
{
    qDebug() << "makeProfileTitle" << type << optDescription_;
    if (!optDescription_.isEmpty())
    {
        if (!skipDescrType_)
            return optDescription_ + " " + type;
        else
            return optDescription_ + " " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    }
 
    QString res="";
    if (!deviceName_.isEmpty())
        res += deviceName_ + " -";

    if (!skipDescrType_)
        res += " " + type;
    res += " " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    return res;
}

bool QubyxProfile::isLabProfile()
{
    return profile_.m_Header.colorSpace == icSigLabData || profile_.m_Header.pcs == icSigLabData
            || inColorSpace_ == icSigLabData || outColorSpace_ == icSigLabData;
}

CIccTag *QubyxProfile::getIccTag(icTagSignature signature)
{
    return profile_.FindTag(signature);
}

void QubyxProfile::calcChromaticAdaptation(icFloatNumber* PCS, icFloatNumber* src, icFloatNumber* resMatrix, ChromaType type/*=ChromaBradford*/)
{
    // linearized Bradford transformation, see ICC spec Annex E
    icFloatNumber fwdLinBfd[] = {  (icFloatNumber)0.8951,  (icFloatNumber)0.2664,  (icFloatNumber)-0.1614,
                                         (icFloatNumber)-0.7502, (icFloatNumber)1.7135,  (icFloatNumber)0.0367,
                                         (icFloatNumber)0.0389,  (icFloatNumber)-0.0685, (icFloatNumber) 1.0296 };
    // inverse is from Mathematica since the spec doesn't provide it
    icFloatNumber invLinBfd[] = {  (icFloatNumber)0.9869929,    (icFloatNumber)-0.1470543, (icFloatNumber)0.1599627,
                                         (icFloatNumber)0.4323053,    (icFloatNumber)0.5183603,   (icFloatNumber)0.0492912,
                                         (icFloatNumber)-0.00852866, (icFloatNumber)0.0400428, (icFloatNumber)0.9684867 };
    
    // CIECAM/CAT02 forward matrix
    icFloatNumber fwdCAT02[] = {         (icFloatNumber)0.7328,  (icFloatNumber)0.4296,  (icFloatNumber)-0.1624,
                                         (icFloatNumber)-0.7036, (icFloatNumber)1.6975,  (icFloatNumber)0.0061,
                                         (icFloatNumber)0.0030,  (icFloatNumber)0.0136, (icFloatNumber) 0.9834 };
    // inverse is from MatLab
    icFloatNumber invCAT02[] = {         (icFloatNumber)1.096123820835514,    (icFloatNumber)-0.278869000218287, (icFloatNumber)0.182745179382773,
                                         (icFloatNumber)0.454369041975359,    (icFloatNumber)0.473533154307412,   (icFloatNumber)0.072097803717229,
                                         (icFloatNumber)-0.009627608738429, (icFloatNumber)-0.005698031216113, (icFloatNumber)1.015325639954543 };
    
    //von Kries transformation
    icFloatNumber fwdvKr[] = {  (icFloatNumber)0.4002400,  (icFloatNumber)0.7076,  (icFloatNumber)-0.08081,
                                         (icFloatNumber)-0.2263, (icFloatNumber)1.16532,  (icFloatNumber)0.0457,
                                         (icFloatNumber)0.,  (icFloatNumber)0., (icFloatNumber) 0.91822 };
    // inversed von Kries matrix
    icFloatNumber invvKr[] = {  (icFloatNumber)1.8599364,    (icFloatNumber)-1.1293816, (icFloatNumber)0.2198974,
                                         (icFloatNumber)0.3611914,    (icFloatNumber)0.6388125,   (icFloatNumber)-0.0000064,
                                         (icFloatNumber)0., (icFloatNumber)0., (icFloatNumber)1.0890636 };
    
    icFloatNumber* fwd = fwdLinBfd;
    icFloatNumber* inv = invLinBfd;
    
    switch(type)
    {
    case ChromaBradford:
        fwd = fwdLinBfd;
        inv = invLinBfd;
        break;
    case ChromaCAT02:
        fwd = fwdCAT02;
        inv = invCAT02;
        break;
    case ChromaVonKries:
        fwd = fwdvKr;
        inv = invvKr;
        break;
    }
    

    icFloatNumber rhoPCS = fwd[0] * PCS[0] + fwd[1] * PCS[1] + fwd[2] * PCS[2];
    icFloatNumber gamPCS = fwd[3] * PCS[0] + fwd[4] * PCS[1] + fwd[5] * PCS[2];
    icFloatNumber betPCS = fwd[6] * PCS[0] + fwd[7] * PCS[1] + fwd[8] * PCS[2];
    icFloatNumber rhoSrc = fwd[0] * src[0] + fwd[1] * src[1] + fwd[2] * src[2];
    icFloatNumber gamSrc = fwd[3] * src[0] + fwd[4] * src[1] + fwd[5] * src[2];
    icFloatNumber betSrc = fwd[6] * src[0] + fwd[7] * src[1] + fwd[8] * src[2];
    icFloatNumber scaling[9];
    scaling[0] = rhoPCS / rhoSrc;
    scaling[1] = 0;
    scaling[2] = 0;
    scaling[3] = 0;
    scaling[4] = gamPCS / gamSrc;
    scaling[5] = 0;
    scaling[6] = 0;
    scaling[7] = 0;
    scaling[8] = betPCS / betSrc;
    icFloatNumber tmp[9];
    icMatrixMultiply3x3(tmp, scaling, fwd);
    icMatrixMultiply3x3(resMatrix, inv, tmp);
}

void QubyxProfile::normalizeColor(icFloatNumber *normalized, Color &color, Color &refcolor, Color *black)
{
    if (!black)
    {
        normalized[0] = color.X()/refcolor.Y();
        normalized[1] = color.Y()/refcolor.Y();
        normalized[2] = color.Z()/refcolor.Y();
        return;
    }
    
    long double ref = (refcolor.Y() - black->Y());
    normalized[0] = (color.X() - black->X())/ref;
    normalized[1] = (color.Y() - black->Y())/ref;
    normalized[2] = (color.Z() - black->Z())/ref;
}

void QubyxProfile::perceptualTransform(icFloatNumber *point, icFloatNumber *white, icFloatNumber *black)
{
    for (int i=0;i<3;i++)
        point[i] = point[i]*(1-black[i]/white[i])+black[i];
}

void QubyxProfile::setColorSpaces(icColorSpaceSignature inColorSpace, icColorSpaceSignature outColorSpace)
{
    inColorSpace_ = inColorSpace;
    outColorSpace_ = outColorSpace;
}

void QubyxProfile::fillProfileHeader()
{
    profile_.InitHeader();    
    
#ifdef Q_OS_WIN
    profile_.m_Header.platform = icSigMicrosoft;
#elif defined(Q_OS_MAC)
    profile_.m_Header.platform = icSigMacintosh;
#endif

    //profile_.m_Header.attributes = static_cast<icUInt64Number>(icTransparency);
    //profile_.m_Header.renderingIntent = icRelativeColorimetric;
    profile_.m_Header.attributes = icReflective | icGlossy;
    profile_.m_Header.renderingIntent = icPerceptual;
    
    profile_.m_Header.colorSpace = inColorSpace_;
    profile_.m_Header.pcs = outColorSpace_;

    profile_.m_Header.creator = icSignature(0x71756279);
#ifdef Q_OS_MAC
    profile_.m_Header.cmmId = icSigApple;
#else
    profile_.m_Header.cmmId = icSigAdobe;
#endif       
    profile_.m_Header.version = (spec_==ICCSpec::ICCv4 ? icVersionNumberV4 : icVersionNumberV2_1);

    profile_.m_Header.model = icSigDisplayClass;
#ifdef Q_OS_WIN
    profile_.m_Header.manufacturer = icSigMicrosoft;
#endif

    QDateTime now = QDateTime::currentDateTime();
    profile_.m_Header.date.year = now.date().year();
    profile_.m_Header.date.month = now.date().month();
    profile_.m_Header.date.day = now.date().day();
    profile_.m_Header.date.hours = now.time().hour();
    profile_.m_Header.date.minutes = now.time().minute();
    profile_.m_Header.date.seconds = now.time().second();

    QString company = "QUBYX Software Technologies LTD HK";
    if (GlobalData::getString(GLOBALDATA_APPLICATION_NAME).toLower().contains("globell"))
        company = "Globell B.V.";
    if (GlobalData::getString(GLOBALDATA_APPLICATION_NAME).toLower().contains("quantum"))
        company = "Quantum Spectra";
    setTextTag(icSigCopyrightTag, "(c) " + company + " 2008-" + QString::number(QDate::currentDate().year()));
}

void QubyxProfile::setDeviceName(QString deviceName)
{
    deviceName_ = deviceName;
    skipDescrType_ = false;
}

void QubyxProfile::addPointTag(icFloatNumber *point, icSignature tag, CAT* cat)
{
    icFloatNumber flare[3] = { 0, 0, 0 };
    CIccTagXYZ* PointTag = new CIccTagXYZ;
    icFloatNumber adaptedMedia[3];
    if (cat)
    {
        IccCLUT::measuredXYZToAdaptedXYZ(adaptedMedia, point, flare, 1, cat);
        /*if (adaptedMedia[0]<0 || adaptedMedia[1]<0 || adaptedMedia[2]<0)
            for (int i=0;i<3;i++)
                adaptedMedia[i] = point[i];*/
    }
    else
        for (int i=0;i<3;i++)
            adaptedMedia[i] = point[i];

    for (int i=0;i<3;i++)
        if (adaptedMedia[i]<0)
           adaptedMedia[i] = 0;
/*
    if (tag==icSigRedMatrixColumnTag || tag==icSigGreenMatrixColumnTag || tag==icSigBlueMatrixColumnTag)
    {
        icFloatNumber icD50Black[3];
        icD50Black[0] = 0.003357;
        icD50Black[1] = 0.003479;
        icD50Black[2] = 0.002869;

        perceptualTransform(adaptedMedia, icD50XYZ, icD50Black);
    }
  */  

    (*PointTag)[0].X = icDtoF(adaptedMedia[0]);
    (*PointTag)[0].Y = icDtoF(adaptedMedia[1]);
    (*PointTag)[0].Z = icDtoF(adaptedMedia[2]);
    profile_.DeleteTag(tag);
    profile_.AttachTag(tag, PointTag);
}

void QubyxProfile::addPointTag(icInt32Number *point, icSignature tag)
{
    CIccTagXYZ* PointTag = new CIccTagXYZ;

    (*PointTag)[0].X = point[0];
    (*PointTag)[0].Y = point[1];
    (*PointTag)[0].Z = point[2];
    profile_.DeleteTag(tag);
    profile_.AttachTag(tag, PointTag);
}

bool QubyxProfile::readPointTag(icFloatNumber *point, icSignature tag)
{
    icInt32Number intPt[3];

    if (!readPointTag(intPt, tag))
        return false;

    for (int i=0;i<3;++i)
        point[i] = icFtoD(intPt[i]);

    return true;
}

bool QubyxProfile::readPointTag(icInt32Number *point, icSignature tag)
{
    CIccTagXYZ* xyzTag = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(tag));
    if (!xyzTag)
        return false;
    if (!xyzTag->GetSize())
        return false;

    icXYZNumber xyz = (*xyzTag)[0];
    point[0] = xyz.X;
    point[1] = xyz.Y;
    point[2] = xyz.Z;

    return true;
}

void QubyxProfile::addTRCtag(icSignature tagname, std::vector<double> &values)
{
    int sz = values.size();
    std::vector<double> x;
    for (int i=0;i<sz;i++)
        x.push_back(i);

    addTRCtag(tagname, x, values);
}

void QubyxProfile::addTRCtag(icSignature tagname, std::vector<double> &x, std::vector<double> &values)
{
    if (x.size()<1 || x.size()!=values.size())
        return;

    double maxx = x[0], maxy = values[0];
    for (unsigned i=1;i<x.size();i++)
    {
        maxx = std::max<double>(maxx, x[i]);
        maxy = std::max<double>(maxy, values[i]);
    }

    std::vector<double> ax, ay, y;
    for (int i=0;i<TRCSIZE;i++)
        ax.push_back(i*maxx/(TRCSIZE-1));

    y = values;
    for (unsigned i=0;i<y.size();i++)
        y[i] /= maxy;

    Interpolation::SplineAkima2<double, double>(x, y, ax, ay);

    CIccTagCurve* TRCTag = new CIccTagCurve(TRCSIZE);
    for (int i=0;i<TRCSIZE; ++i)
        (*TRCTag)[i] = ay[i];
    profile_.DeleteTag(tagname);
    profile_.AttachTag(tagname, TRCTag);
}

void QubyxProfile::addTRCtagDirect(icSignature tagname, const std::vector<long double> &values)
{
    CIccTagCurve* TRCTag = new CIccTagCurve(values.size());
    for (unsigned i=0;i<values.size(); ++i)
        (*TRCTag)[i] = values[i];
    profile_.DeleteTag(tagname);
    profile_.AttachTag(tagname, TRCTag);
}

void QubyxProfile::readProfileColors(Color &black, Color &white, Color &red, Color &green, Color &blue, ChromaType type)
{
    Matrix ch(3,3), tmp(3,1);
    CIccTagS15Fixed16* chroma = dynamic_cast<CIccTagS15Fixed16*>(profile_.FindTag(icSigChromaticAdaptationTag));
    if (chroma)
    {
        for (int i=0;i<9;i++)
            ch.at(i/3, i%3) = icFtoD((*chroma)[i]);
    }
    else
        for (int i=0;i<3;++i)
            for (int j=0;j<3;j++)
                ch.at(i,j) = (i==j ? 1 : 0 );

    ch = Matrix::Inverse(ch);
    CIccTagXYZ* col;
    
    qDebug() << "inverted chad:";
    for (int i=0;i<3;++i)
        qDebug() << (double)ch.at(i, 0) << (double)ch.at(i, 1) << (double)ch.at(i,2);
    
    white = Color::FromXYZ(0.964220, 1.0000, 0.825210);
    {
        tmp.at(0,0) = white.X();
        tmp.at(1,0) = white.Y();
        tmp.at(2,0) = white.Z();
        tmp = ch*tmp;
        
        white = Color::FromXYZ(tmp.at(0,0), tmp.at(1,0), tmp.at(2,0));

        //recalc chad by current type
        icFloatNumber nwhite[3], chromatic[9];
        for (int i=0;i<3;++i)
            nwhite[i] = tmp.at(i,0)/tmp.at(1,0);

        calcChromaticAdaptation(icD50XYZ, nwhite, chromatic, type);
        for (int i=0;i<9;i++)
            ch.at(i/3, i%3) = chromatic[i];
        ch = Matrix::Inverse(ch);
    }
    long double coef = white.Y();

    
    black = Color::FromXYZ(0*coef,0*coef,0*coef);
    col = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(icSigMediaBlackPointTag));
    if (col)
    {
        tmp.at(0,0) = icFtoD((*col)[0].X);
        tmp.at(1,0) = icFtoD((*col)[0].Y);
        tmp.at(2,0) = icFtoD((*col)[0].Z);
        tmp = ch*tmp;
        
        black = Color::FromXYZ(tmp.at(0,0), tmp.at(1,0), tmp.at(2,0));
    }
    
      
    red = Color::FromXYZ(0.436075*coef, 0.222504*coef, 0.013932*coef);
    col = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(icSigRedMatrixColumnTag));
    if (col)
    {
        tmp.at(0,0) = icFtoD((*col)[0].X);
        tmp.at(1,0) = icFtoD((*col)[0].Y);
        tmp.at(2,0) = icFtoD((*col)[0].Z);
        tmp = ch*tmp;
        
        red = Color::FromXYZ(tmp.at(0,0), tmp.at(1,0), tmp.at(2,0));
    }
    
    green = Color::FromXYZ(0.385065*coef, 0.716879*coef, 0.097105*coef);
    col = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(icSigGreenMatrixColumnTag));
    if (col)
    {
        tmp.at(0,0) = icFtoD((*col)[0].X);
        tmp.at(1,0) = icFtoD((*col)[0].Y);
        tmp.at(2,0) = icFtoD((*col)[0].Z);
        tmp = ch*tmp;
        
        green = Color::FromXYZ(tmp.at(0,0), tmp.at(1,0), tmp.at(2,0));
    }
    
    blue = Color::FromXYZ(0.143080*coef, 0.060617*coef, 0.714173*coef);
    col = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(icSigBlueMatrixColumnTag));
    if (col)
    {
        tmp.at(0,0) = icFtoD((*col)[0].X);
        tmp.at(1,0) = icFtoD((*col)[0].Y);
        tmp.at(2,0) = icFtoD((*col)[0].Z);
        tmp = ch*tmp;
        
        blue = Color::FromXYZ(tmp.at(0,0), tmp.at(1,0), tmp.at(2,0));
    }
    
    //fix other format (not from 0 to 1)
    
    white = Color::FromXYZ(white.X()/coef, white.Y()/coef, white.Z()/coef);
    black = Color::FromXYZ(black.X()/coef, black.Y()/coef, black.Z()/coef);
    red = Color::FromXYZ(red.X()/coef, red.Y()/coef, red.Z()/coef);
    green = Color::FromXYZ(green.X()/coef, green.Y()/coef, green.Z()/coef);
    blue = Color::FromXYZ(blue.X()/coef, blue.Y()/coef, blue.Z()/coef);
    
    qDebug() << "white" << white.X() << white.Y() << white.Z();
    qDebug() << "black" << black.X() << black.Y() << black.Z();
    qDebug() << "red" << red.X() << red.Y() << red.Z();
    qDebug() << "green" << green.X() << green.Y() << green.Z();
    qDebug() << "blue" << blue.X() << blue.Y() << blue.Z();
}

bool QubyxProfile::isQubyxProfile()
{
    if (profile_.m_Header.creator!=icSignature(0x71756279)) //creator == qbx
        return false;

    if (!getTextTag(icSigProfileDescriptionTag).toLower().contains("qubyx")
            && !getTextTag(icSigProfileDescriptionTag).toLower().contains("globell")
            && !getTextTag(icSigProfileDescriptionTag).toLower().contains("quantum"))
        return false;
    if (!getTextTag(icSigCopyrightTag).toLower().contains("qubyx")
            && !getTextTag(icSigCopyrightTag).toLower().contains("globell")
            && !getTextTag(icSigCopyrightTag).toLower().contains("quantum"))
        return false;
             
    return true;
}

bool QubyxProfile::applyChromaticAdaptation(long double m[9], Color &meas)
{
    long double X, Y, Z;
    X = m[0]*meas.X()+m[1]*meas.Y()+m[2]*meas.Z();
    Y = m[3]*meas.X()+m[4]*meas.Y()+m[5]*meas.Z();
    Z = m[6]*meas.X()+m[7]*meas.Y()+m[8]*meas.Z();

//    qDebug() << "Chroma " << meas.X() << meas.Y() << meas.Z() <<
//            "->" << double(X) << double(Y) << double(Z);

    meas = Color::FromXYZ(X, Y, Z); 
    
    return true;
}

template <class T>
bool QubyxProfile::applyChromaticAdaptation(long double m[9], T XYZ[3])
{
    T X, Y, Z;
    X = m[0]*XYZ[0] + m[1]*XYZ[1] + m[2]*XYZ[2];
    Y = m[3]*XYZ[0] + m[4]*XYZ[1] + m[5]*XYZ[2];
    Z = m[6]*XYZ[0] + m[7]*XYZ[1] + m[8]*XYZ[2];

    XYZ[0] = X;
    XYZ[1] = Y;
    XYZ[2] = Z;

    return true;
}

template <typename T>
bool QubyxProfile::applyChromaticAdaptation(const std::vector<double> &m, T XYZ[3])
{
    if (m.size()<9)
        return false;

    T X, Y, Z;
    X = m[0]*XYZ[0] + m[1]*XYZ[1] + m[2]*XYZ[2];
    Y = m[3]*XYZ[0] + m[4]*XYZ[1] + m[5]*XYZ[2];
    Z = m[6]*XYZ[0] + m[7]*XYZ[1] + m[8]*XYZ[2];

    XYZ[0] = X;
    XYZ[1] = Y;
    XYZ[2] = Z;

    return true;
}

template <typename T>
bool QubyxProfile::applyChromaticAdaptation(const std::vector<double> &m, std::vector<T> &XYZ)
{
    if (m.size()<9)
        return false;

    T X, Y, Z;
    X = m[0]*XYZ[0] + m[1]*XYZ[1] + m[2]*XYZ[2];
    Y = m[3]*XYZ[0] + m[4]*XYZ[1] + m[5]*XYZ[2];
    Z = m[6]*XYZ[0] + m[7]*XYZ[1] + m[8]*XYZ[2];

    XYZ[0] = X;
    XYZ[1] = Y;
    XYZ[2] = Z;

    return true;
}

bool QubyxProfile::applyChromaticAdaptation(const std::vector<double> &m, Color &meas)
{
    if (m.size()<9)
        return false;

    long double X, Y, Z;
    X = m[0]*meas.X()+m[1]*meas.Y()+m[2]*meas.Z();
    Y = m[3]*meas.X()+m[4]*meas.Y()+m[5]*meas.Z();
    Z = m[6]*meas.X()+m[7]*meas.Y()+m[8]*meas.Z();

//    qDebug() << "Chroma " << meas.X() << meas.Y() << meas.Z() <<
//            "->" << double(X) << double(Y) << double(Z);

    meas = Color::FromXYZ(X, Y, Z);

    return true;
}


bool QubyxProfile::applyPerceptualAdaptation(double blackPCS[3], Color &meas)
{
    //d50 black
    const long double nblack[] = {0.003357, 0.003479, 0.002869};
    long double X, Y, Z;
    X = (meas.X()-blackPCS[0])*(icD50XYZ[0]-nblack[0])/(icD50XYZ[0]-blackPCS[0])+nblack[0];
    Y = (meas.Y()-blackPCS[1])*(icD50XYZ[1]-nblack[1])/(icD50XYZ[1]-blackPCS[1])+nblack[1];
    Z = (meas.Z()-blackPCS[2])*(icD50XYZ[2]-nblack[2])/(icD50XYZ[2]-blackPCS[2])+nblack[2];

    if (X<0)
        X = 0;
    if (X>1)
        X = 1;
    if (Y<0)
        Y = 0;
    if (Y>1)
        Y = 1;
    if (Z<0)
        Z = 0;
    if (Z>1)
        Z = 1;

    meas = Color::FromXYZ(X, Y, Z);
    
    return true;
}

bool QubyxProfile::applyChromaticAdaptationToAll(std::vector<std::vector<std::vector<Color> > > &meas)
{
    CIccTagS15Fixed16* chroma = dynamic_cast<CIccTagS15Fixed16*>(profile_.FindTag(icSigChromaticAdaptationTag));
    if (!chroma || chroma->GetSize()<9)
    {
        qDebug() << "applyChromaticAdaptationToAll grid. failed to load chroma";
        return false;
    }
    
    
    long double m[9];
    for (int i=0;i<9;i++)
        m[i] = icFtoD((*chroma)[i]);
    
    
    //qDebug() << double(m[0]) << double(m[1]) << double(m[2]);
    //qDebug() << double(m[3]) << double(m[4]) << double(m[5]);
    //qDebug() << double(m[6]) << double(m[7]) << double(m[8]);
    
    
    for (unsigned r=0;r<meas.size();r++)
        for (unsigned g=0;g<meas[r].size();g++)
            for (unsigned b=0;b<meas[r][g].size();b++)
                applyChromaticAdaptation(m, meas[r][g][b]);

    return true;
}

bool QubyxProfile::applyPerceptualAdaptationToAll(std::vector<std::vector<std::vector<Color> > > &meas, Color black)
{             
    double blackPCS[3];
    if (meas.size() && meas[0].size() && meas[0][0].size())
    {
        blackPCS[0] = black.X();
        blackPCS[1] = black.Y();
        blackPCS[2] = black.Z();
    }
    
    
    for (unsigned r=0;r<meas.size();r++)
        for (unsigned g=0;g<meas[r].size();g++)
            for (unsigned b=0;b<meas[r][g].size();b++)
                applyPerceptualAdaptation(blackPCS, meas[r][g][b]);
    
    return true;
}

void QubyxProfile::fillCurveTag(CIccTagCurve* curve, Curve &values)
{
    unsigned sz = values.size();
    if (sz>2 && curve)
    {
        curve->SetSize(sz, icInitNone);
        for (unsigned j=0;j<sz;j++)
            (*curve)[j] = values[j];
    }
}

void QubyxProfile::fillCurveTagLinear(CIccTagCurve* curve)
{
    if (curve)
    {
        curve->SetSize(2);
        (*curve)[0]=0;
        (*curve)[1]=1;
    }
}

void QubyxProfile::fillParamCurveTag(CIccTagParametricCurve *curve, const QubyxProfile::ParamCurve &values)
{
    curve->SetFunctionType(values.type);
    for (unsigned i=0;i<values.params.size();++i)
        (*curve)[i] = values.params[i];
}

void QubyxProfile::fillParamCurveTagLinear(CIccTagParametricCurve *curve)
{
    curve->SetFunctionType(0);
    (*curve)[0] = 1;
}

template<class Tag>
void QubyxProfile::AddFull3dLUTTag(icTagSignature signature, Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &matr, ProfileCLUT_Equidistant &lut)
{
    Tag *a2b = new Tag;
    unsigned grids = lut.gridCount();
    unsigned in = lut.inChannels();
    unsigned out = lut.outChannels();
           
    Curves ta=aCurve, tm=mCurve, tb=bCurve;
    
    unsigned curvesIn = in;
    unsigned curvesOut = out;  

    a2b->Init(curvesIn,curvesOut);
    if (signature==icSigAToB0Tag || 
       signature==icSigAToB1Tag || 
       signature==icSigAToB2Tag)
    {
        a2b->SetColorSpaces(inColorSpace_, outColorSpace_);                
    }
    else
    {
        if (!std::is_same<Tag, CIccTagLut16>::value)
            std::swap(curvesIn, curvesOut);

        a2b->SetColorSpaces(outColorSpace_, inColorSpace_);

    }
    CIccCLUT *clut = a2b->NewCLUT(grids);
    
    LPIccCurve *c = a2b->NewCurvesB();
    LPIccCurve *acurves = a2b->NewCurvesA();
    LPIccCurve *mcurves = ((in==3 && out==3 && spec_!=ICCSpec::ICCv2) || !mCurve.curves.empty()) ? a2b->NewCurvesM() : nullptr;
    CIccCurve *pcurve;
    
    //lut
    std::vector<ProfileCLUT_Equidistant::Values> lutv = lut.lut();
    
    for (unsigned i=0;i<lutv.size();i++)
        for (unsigned j=0;j<out;j++)
            (*clut)[i*out+j] = lutv[i][j];
    
    //matrix
    if (in==3 && out==3)
        if (spec_==ICCSpec::ICCv4
            || (spec_==ICCSpec::ICCv2
                && mcurves))
    {
        CIccMatrix *m = a2b->NewMatrix();            
        m->m_bUseConstants = false;
        int count = (spec_==ICCSpec::ICCv2 ? 9 : 12);
        for (int i=0;i<count;i++)
            m->m_e[i] = 0;
        for (int i=0;i<3;i++)
            m->m_e[i*3+i] = 1;

        for (int i=0;i<matr.rows();i++)
            for (int j=0;j<matr.columns() && j<3;j++)
                if (i*3+j<count)
                    m->m_e[i*3+j] = matr.at(i, j);

        if (matr.columns()>3 && count>=12)
        {
            m->m_bUseConstants = true;
            for (int i=0;i<matr.rows();i++)
                m->m_e[9+i] = matr.at(i, 3);
        }
    }

    if (std::is_same<Tag, CIccTagLut16>::value)
    {
        std::swap(acurves, c);
    }
       
    //curves
    for (unsigned i=0;i<curvesIn;i++)
    {
        pcurve = ta.parametric.size()>i && ta.parametric[i] ? static_cast<CIccCurve *>(new CIccTagParametricCurve) : static_cast<CIccCurve *>(new CIccTagCurve);
        //pcurve->SetGamma(1.);
        acurves[i] = pcurve;
    }
    
    for (unsigned i=0;i<curvesOut;i++)
    {
        pcurve = tb.parametric.size()>i && tb.parametric[i] ? static_cast<CIccCurve *>(new CIccTagParametricCurve) : static_cast<CIccCurve *>(new CIccTagCurve);
        //pcurve->SetGamma(1.);
        c[i] = pcurve;
        
        if (mcurves)
        {
            pcurve = tm.parametric.size()>i && tm.parametric[i] ? static_cast<CIccCurve *>(new CIccTagParametricCurve) : static_cast<CIccCurve *>(new CIccTagCurve);
            //pcurve->SetGamma(1.);
            mcurves[i] = pcurve;
        }
    }
    
    for (unsigned i=0;i<curvesIn;i++)
        if (ta.parametric.size()>i && ta.parametric[i])
        {
            if (i<ta.paramCurves.size())
                fillParamCurveTag(dynamic_cast<CIccTagParametricCurve*>(acurves[i]), ta.paramCurves[i]);
            else
                fillParamCurveTagLinear(dynamic_cast<CIccTagParametricCurve*>(acurves[i]));
        }
        else
        {
            if (i<ta.curves.size())
                fillCurveTag(dynamic_cast<CIccTagCurve*>(acurves[i]), ta.curves[i]);
            else
                fillCurveTagLinear(dynamic_cast<CIccTagCurve*>(acurves[i]));
        }
            
    if (mcurves)
        for (unsigned i=0;i<curvesOut;i++)
            if (tm.parametric.size()>i && tm.parametric[i])
            {
                if (i<tm.paramCurves.size())
                    fillParamCurveTag(dynamic_cast<CIccTagParametricCurve*>(mcurves[i]), tm.paramCurves[i]);
                else
                    fillParamCurveTagLinear(dynamic_cast<CIccTagParametricCurve*>(mcurves[i]));
            }
            else
            {
                if (i<tm.curves.size())
                    fillCurveTag(dynamic_cast<CIccTagCurve*>(mcurves[i]), tm.curves[i]);
                else
                    fillCurveTagLinear(dynamic_cast<CIccTagCurve*>(mcurves[i]));
            }
    
    for (unsigned i=0;i<curvesOut;i++)
        if (tb.parametric.size()>i && tb.parametric[i])
        {
            if (i<tb.paramCurves.size())
                fillParamCurveTag(dynamic_cast<CIccTagParametricCurve*>(c[i]), tb.paramCurves[i]);
            else
                fillParamCurveTagLinear(dynamic_cast<CIccTagParametricCurve*>(c[i]));
        }
        else
        {
            if (i<tb.curves.size())
                fillCurveTag(dynamic_cast<CIccTagCurve*>(c[i]), tb.curves[i]);
            else
                fillCurveTagLinear(dynamic_cast<CIccTagCurve*>(c[i]));
        }
    
    
//    std::string descr;
//    a2b->Describe(descr);
//    qDebug() << "-----------------------";
//    qDebug() << QString::fromStdString(descr);
//    qDebug() << "-----------------------";
       
    profile_.DeleteTag(signature);
    profile_.AttachTag(signature, a2b);
}

void QubyxProfile::AddAToBTag(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature AToBTag/*=icSigAToB0Tag*/)
{   
    switch(spec_)
    {
    case ICCSpec::ICCv2:
        AddFull3dLUTTag<CIccTagLut16>(AToBTag, aCurve, mCurve, bCurve, m, clut);
        break;
    case ICCSpec::ICCv4:
        AddFull3dLUTTag<CIccTagLutAtoB>(AToBTag, aCurve, mCurve, bCurve, m, clut);
        break;
    }
}

void QubyxProfile::AddAToBTagPrinter(QubyxProfile::Curves &aCurve, QubyxProfile::Curves &mCurve, QubyxProfile::Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature AToBTag)
{
    AddFull3dLUTTag<CIccTagLut16>(AToBTag, aCurve, mCurve, bCurve, m, clut);
}

void QubyxProfile::AddBToATag(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature BToATag/*=icSigBToA0Tag*/)
{
    switch(spec_)
    {
    case ICCSpec::ICCv2:
        AddFull3dLUTTag<CIccTagLut16>(BToATag, aCurve, mCurve, bCurve, m, clut);
        break;
    case ICCSpec::ICCv4:
        AddFull3dLUTTag<CIccTagLutBtoA>(BToATag, aCurve, mCurve, bCurve, m, clut);
        break;
    }

}

void QubyxProfile::AddBToATagPriner(QubyxProfile::Curves &aCurve, QubyxProfile::Curves &mCurve, QubyxProfile::Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature BToATag)
{
    AddFull3dLUTTag<CIccTagLut16>(BToATag, aCurve, mCurve, bCurve, m, clut);
}

//template<class Tag>
//ProfileCLUT_Equidistant QubyxProfile::get3dLUTTag(icTagSignature signature, bool applyChromatic, bool applyLuminance)
//{
//    double luminance = 1.0;
//    if (applyChromatic)
//        initChromaticAdaptation(RIAbsoluteColorimetric);
//    if (applyLuminance)
//        getLuminance(luminance);
//    
//    Tag *tag = dynamic_cast<Tag*>(profile_.FindTag(signature) );
//    CIccCLUT *clut = tag->GetCLUT();
//    int inSize = tag->InputChannels(), outSize = tag->OutputChannels();
//    int grid = clut->GridPoints();
//    int points = clut->NumPoints();
//    
//    //qDebug() << "QubyxProfile::get3dLUTTag" << inSize << outSize << grid << points;
//    
//    ProfileCLUT_Equidistant res(grid, inSize, outSize);
//    
//    std::vector<long double> index(inSize);
//    for (int i=0;i<points;i++)
//    {
//        std::vector<long double> value;
//        for (int j=0;j<outSize;j++)
//            value.push_back((*clut)[i*outSize + j] );
//        //qDebug() << i << (double)value[0] << (double)value[1] << (double)value[2];
//        
//        if (applyChromatic)
//        {
//            Color col = Color::FromXYZ(value[0], value[1], value[2]);
//            applyChromaticAdaptation(convertRevChroma_, col);
//            value[0] = col.X();
//            value[1] = col.Y();
//            value[2] = col.Z();
//        }
//        if (applyLuminance)
//        {
//            for (auto &a : value)
//                a*=luminance;
//        }
//                    
//        
//        for (int j=inSize-1, idx = i;j>=0;j--,idx/=grid)
//            index[j] = idx%grid;
//        
//        res.setGridValue(index, value);
//    }
//        
//    return res;
//}

//ProfileCLUT_Equidistant QubyxProfile::getAToBTag(icTagSignature signature, bool applyChromatic/* = false*/, bool applyLuminance/* = false*/)
//{
//    return get3dLUTTag<CIccTagLutAtoB>(signature, applyChromatic, applyLuminance);
//}
//
//ProfileCLUT_Equidistant QubyxProfile::getBToATag(icTagSignature signature)
//{
//    return get3dLUTTag<CIccTagLutBtoA>(signature, false, false);
//}

bool QubyxProfile::SaveToFile()
{
    if (!savable_)
        return true;
    
    int pos = filename_.lastIndexOf('/');
    if (pos != -1)
    {
        QDir dir(filename_.left(pos));
        if (!dir.exists())
            dir.mkdir(".");
    }
    
    fillProfileHeader();
            
    CIccFileIO out;
    bool res = false;
#ifdef Q_OS_WIN
    res = out.Open(filename_.toStdWString().c_str(), (wchar_t*)L"w+");
#elif defined(Q_OS_MAC)
    res = out.Open(filename_.toUtf8().data(), "w+");
#endif
    if (!res)
    {
        qDebug() << "Can't open file for profile saving - " << filename_;
    }
    else
    {
        res = profile_.Write(&out, icAlwaysWriteID);
        out.Close();
    }
    
    bool permRes = OSUtils::setFullPermissions(filename_);
    qDebug()<<"Set profilePermission "<<filename_<<" result - "<<permRes;
    qDebug()<<"Save profile result "<<res;

    //res = res && validate();

    return res;
}

bool QubyxProfile::SaveToFileWithTry(int trycount)
{
    QString originalName = filename_;
    
    for (int i = 0; i < trycount; i++)
    {
        if (!SaveToFile())
        {
            filename_ = originalName;
            int ci = filename_.lastIndexOf('.');
            filename_.insert(ci-1, QString::number(i));
            qDebug()<<"Try save to - "<<filename_;
        }
        else
                return true;
    }
    
    return false;
}

bool QubyxProfile::LoadFromFile()
{
    CIccFileIO in; 
    bool isOpen = false;
#ifdef Q_OS_WIN
    isOpen = in.Open(filename_.toStdWString().c_str(), (wchar_t*)L"r");
#elif defined(Q_OS_MAC)
    isOpen = in.Open(filename_.toUtf8().data(), "r");
#endif
    if (isOpen)
    {
        bool res = profile_.Read(&in);
        in.Close();

        //res = res && validate();
        qDebug() << "?? Load profile " << filename_ << "result=" << res;

        inColorSpace_ = profile_.m_Header.colorSpace;
        outColorSpace_ = profile_.m_Header.pcs;

        spec_ = (profile_.m_Header.version<icVersionNumberV4) ? ICCSpec::ICCv2 : ICCSpec::ICCv4;

        return res;
    }
    else
        return false;
}

bool QubyxProfile::LoadFromMemory(unsigned char *buf, size_t size)
{
    qDebug() << "QubyxProfile::LoadFromMemory" << size;
    CIccMemIO in;
    in.Attach(buf, size, false);
    bool res = profile_.Read(&in);
    
    qDebug() << "?? Load profile from memory" << "result=" << res;
    
    inColorSpace_ = profile_.m_Header.colorSpace;
    outColorSpace_ = profile_.m_Header.pcs;
    
    return res;
}

bool QubyxProfile::SaveToMemory(unsigned char* &buf, size_t &size)
{
    size = profile_.m_Header.size;
    qDebug() << "QubyxProfile::SaveToMemory" << size;
    buf = new unsigned char [size];
    CIccMemIO out;
    out.Attach(buf, size, true);
    
    bool res = profile_.Write(&out);
    
    qDebug() << "Save profile to memory result=" << res;
    
    return res;
}

bool QubyxProfile::validate()
{
#ifdef QT_DEBUG
    std::string validationReport;
    icValidateStatus validationStatus = profile_.Validate(validationReport);

    qDebug() << "QubyxProfile validation:";
    switch (validationStatus)
    {
      case icValidateOK:
	qDebug() << "Profile ok";
        break;

      case icValidateWarning:
	qDebug() << "Profile validation warning " << QString::fromStdString(validationReport);
        break;

      case icValidateNonCompliant:
        qDebug() <<"Profile non compliancy " << QString::fromStdString(validationReport);
        break;

      case icValidateCriticalError:
      default:
          qDebug() <<"Profile error " << QString::fromStdString(validationReport);
    }

    return validationStatus==icValidateOK || validationStatus==icValidateWarning;
#endif

    return true;
}

int QubyxProfile::deviceColorDimention()
{
    return icGetSpaceSamples(profile_.m_Header.colorSpace);
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!

CIccMinMaxEval::CIccMinMaxEval()
{
  minDE1 = minDE2 = 10000;
  maxDE1 = maxDE2 = -1;
  sum1 = sum2 = 0;
  num1 = num2 = 0;

  memset(&maxLab1[0], 0, sizeof(maxLab1));
  memset(&maxLab2[0], 0, sizeof(maxLab2));
  
  memset(&maxPixel1[0], 0, sizeof(maxPixel1));
  memset(&maxPixel2[0], 0, sizeof(maxPixel2));
}

void CIccMinMaxEval::Compare(icFloatNumber *pixel, icFloatNumber *deviceLab, icFloatNumber *lab1, icFloatNumber *lab2)
{
  icFloatNumber DE1 = icDeltaE(deviceLab, lab1);
  icFloatNumber DE2 = icDeltaE(lab1, lab2);

  if (DE1<minDE1) {
    minDE1 = DE1;
  }

  if (DE1>maxDE1) {
    maxDE1 = DE1;
    memcpy(&maxLab1[0], deviceLab, sizeof(maxLab1));
    memcpy(&maxPixel1[0], pixel, sizeof(maxPixel1));
  }

  if (DE2<minDE2) {
    minDE2 = DE2;
  }

  if (DE2>maxDE2) {
    maxDE2 = DE2;
    memcpy(&maxLab2[0], deviceLab, sizeof(maxLab2));
    memcpy(&maxPixel2[0], pixel, sizeof(maxPixel2));
  }

  sum1 += DE1;
  num1 += 1.0;

  sum2 += DE2;
  num2 += 1.0;
}
//!!!!!!!!!!!
bool QubyxProfile::roundTripData(CIccMinMaxEval &eval)
{
    bool bUseMPE = false;
    icRenderingIntent nIntent = icPerceptual;

    CIccInfo info;
    QString report;

    icStatusCMM stat = eval.EvaluateProfile(&profile_, 0, nIntent, icInterpTetrahedral, bUseMPE);

    if (stat!=icCmmStatOk) 
    {
      qDebug() << "  Unable to perform round trip";
      return false;
    }

    return true;
}

QString QubyxProfile::roundTrip()
{
  bool bUseMPE = false;
  icRenderingIntent nIntent = icPerceptual;
    
  CIccInfo info;
  QString report;
  CIccMinMaxEval eval;

  if (bUseMPE)
    report += QString("MPE Rendering Intent: ") + QString::fromLatin1(info.GetRenderingIntentName(nIntent)) + "\n";
  else
    report += QString("Rendering Intent: ") + QString::fromLatin1(info.GetRenderingIntentName(nIntent))  + "\n";

  icStatusCMM stat = eval.EvaluateProfile(&profile_, 0, nIntent, icInterpTetrahedral, bUseMPE);

  if (stat!=icCmmStatOk) 
  {
    report += "  Unable to perform round trip";
    report += "\n";

    return report;
  }

  CIccPRMG prmg;

  stat = prmg.EvaluateProfile(&profile_, nIntent, icInterpTetrahedral, bUseMPE);

  if (stat!=icCmmStatOk) 
  {
    report += "  Unable to perform PRMG analysis";
    report += "\n";

    return report;
  }

  if (nIntent!=icRelativeColorimetric)
    report += QString("Specified Rendering Intent Gamut:  ") + (prmg.m_bPrmgImplied ? "Perceptual Reference Medium Gamut" : "Not Specified") + "\n";

  report += ("\n   Round Trip 1\n");
  report += (  "  ------------\n");
  report += QString("   Min DeltaE:    ") + QString::number(eval.minDE1) + "\n";
  report += QString("   Average DeltaE:   ") + QString::number(eval.GetMean1()) + "\n";
  report += QString("   Max DeltaE:    ") + QString::number(eval.maxDE1) + "\n\n";

  report += QString("   Max L, a, b:   ") + QString::number(eval.maxLab1[0]) + ", " + QString::number(eval.maxLab1[1]) + ", " +QString::number( eval.maxLab1[2])+ "\n";
  report += QString("   Max Pixel:   ") + QString::number(eval.maxPixel1[0]) + ", " + QString::number(eval.maxPixel1[1]) + ", " +QString::number( eval.maxPixel1[2])+ "\n";
  

  report += ("\n   Round Trip 2\n");
  report += (  "   ------------\n");
  report += QString("   Min DeltaE:    ") + QString::number(eval.minDE2) + "\n";
  report += QString("   Average DeltaE:   ") + QString::number(eval.GetMean2()) + "\n";
  report += QString("   Max DeltaE:    ") + QString::number(eval.maxDE2) + "\n\n";

  report += QString("   Max L, a, b:   ") + QString::number(eval.maxLab2[0]) + ", " + QString::number(eval.maxLab2[1]) + ", " +QString::number( eval.maxLab2[2])+ "\n";
  report += QString("   Max Pixel:   ") + QString::number(eval.maxPixel2[0]) + ", " + QString::number(eval.maxPixel2[1]) + ", " +QString::number( eval.maxPixel2[2])+ "\n";
  
    if (prmg.m_nTotal) 
    {
    report += ("\n   PRMG Interoperability - Round Trip Results\n");
    report += (  "   ------------------------------------------------------\n");

    report += QString("   DE <= 1.0 (") + QString::number(prmg.m_nDE1) + "): "+QString::number((float)prmg.m_nDE1/(float)prmg.m_nTotal*100.0)+"f%%\n"; 
    report += QString("   DE <= 2.0 (") + QString::number(prmg.m_nDE2) + "): "+QString::number((float)prmg.m_nDE2/(float)prmg.m_nTotal*100.0)+"f%%\n"; 
    report += QString("   DE <= 3.0 (") + QString::number(prmg.m_nDE3) + "): "+QString::number((float)prmg.m_nDE3/(float)prmg.m_nTotal*100.0)+"f%%\n"; 
    report += QString("   DE <= 5.0 (") + QString::number(prmg.m_nDE5) + "): "+QString::number((float)prmg.m_nDE5/(float)prmg.m_nTotal*100.0)+"f%%\n"; 
    report += QString("   DE <= 10.0 (") + QString::number(prmg.m_nDE10) + "): "+QString::number((float)prmg.m_nDE10/(float)prmg.m_nTotal*100.0)+"f%%\n"; 
    report += QString("   Total     (")+QString::number(prmg.m_nTotal)+")\n";
    
    //report += "\nBad points:\n";
    //report += QString::fromStdString(prmg.bad_points);
  }
  report += "\n";

  return report;
}

void QubyxProfile::denySaving(bool deny/*=true*/)
{
    savable_ = !deny;
}

bool QubyxProfile::addGammutTag(int gridCount)
{
    const double limit = 5;    
    const unsigned in = 3, out = 1; //lab->bool or xyz->bool
    
    CIccTagLut16 *tag = new CIccTagLut16;
    //CIccTagLutAtoB *tag = new CIccTagLutAtoB;
    tag->Init(in, out);    
    tag->SetColorSpaces(profile_.m_Header.pcs, icSigGrayData);
    
    CIccCLUT* clut =  tag->NewCLUT(gridCount);
        
    QubyxProfileChain lab2d, d2lab;
    lab2d.setRenderingIntent(QubyxProfileChain::RI::AbsoluteColorimetric);
    d2lab.setRenderingIntent(QubyxProfileChain::RI::AbsoluteColorimetric);
    lab2d.setTransformationType(QubyxProfileChain::SpaceType::Lab, QubyxProfileChain::SpaceType::DeviceSpecific);
    d2lab.setTransformationType(QubyxProfileChain::SpaceType::DeviceSpecific, QubyxProfileChain::SpaceType::Lab);
    lab2d.addProfile(this);
    d2lab.addProfile(this);

    Color labin1, labin2, labout1, labout2;
    std::vector<double> dev, lab(3);
    double dE1, dE2;
    
    bool isLab = (profile_.m_Header.pcs==icSigLabData);
    //cycle
    for (int i=0;i<gridCount;++i)
        for (int j=0;j<gridCount;++j)
            for (int k=0;k<gridCount;++k)
            {
                //init labin1
                if (isLab)
                    labin1 = Color::FromLab(100.*i/(gridCount-1), 255.*j/(gridCount-1)-128, 255.*k/(gridCount-1)-128);
                else
                    labin1 = Color::FromXYZ(1.*i/(gridCount-1), 1.*j/(gridCount-1), 1.*k/(gridCount-1));
                lab[0] = labin1.L();
                lab[1] = labin1.a();
                lab[2] = labin1.b();
                lab2d.transform(lab, dev);
                d2lab.transform(dev, lab);
                labout1 = Color::FromLab(lab[0], lab[1], lab[2]);
                labin2 = labout1;

                lab2d.transform(lab, dev);
                d2lab.transform(dev, lab);
                labout2 = Color::FromLab(lab[0], lab[1], lab[2]);

                dE1 = ColorDistance::dE1976Lab(labin1, labout1);
                dE2 = ColorDistance::dE1976Lab(labin2, labout2);
                
                dE1 = std::min(100., std::max(0., dE1-limit));
                //qDebug() << "gammut." << i << j << k << "->" << dE1/100;
                (*clut)[(i*gridCount+j)*gridCount+k] = dE1/100;
            }
    
    //
    LPIccCurve *c = tag->NewCurvesB();
    LPIccCurve *acurves = tag->NewCurvesA();
    LPIccCurve *mcurves = tag->NewCurvesM();
    CIccTagCurve *pcurve;
       
    //curves
    for (unsigned i=0;i<out;i++)
    {
        pcurve = new CIccTagCurve;
        pcurve->SetSize(2, icInitIdentity);
        //pcurve->SetGamma(1.);
        acurves[i] = pcurve;
    }
    
    for (unsigned i=0;i<in;i++)
    {
        pcurve = new CIccTagCurve;
        pcurve->SetSize(2, icInitIdentity);
        //pcurve->SetGamma(1.);
        c[i] = pcurve;
        
        pcurve = new CIccTagCurve;
        pcurve->SetSize(2, icInitIdentity);
        //pcurve->SetGamma(1.);
        mcurves[i] = pcurve;
    }
    
    
    profile_.DeleteTag(icSigGamutTag);
    profile_.AttachTag(icSigGamutTag, tag);    
    
    return true;
}

void QubyxProfile::setLuminance(double lum)
{
    CIccTagXYZ* PointTag = new CIccTagXYZ;
    
    (*PointTag)[0].X = icDtoF(0.0);
    (*PointTag)[0].Y = icDtoF(lum);
    (*PointTag)[0].Z = icDtoF(0.0);
    profile_.DeleteTag(icSigLuminanceTag);
    profile_.AttachTag(icSigLuminanceTag, PointTag);
}

bool QubyxProfile::getLuminance(double &resLum) const
{

    CIccTagXYZ* col = dynamic_cast<CIccTagXYZ*>(profile_.FindTag(icSigLuminanceTag));
    if (!col)
        return false;
    
    resLum = icFtoD( (*col)[0].Y );
    qDebug() << "QubyxProfile::getLuminance: " << resLum;
    return true;
}

Matrix QubyxProfile::calcChromaticAdaptation(Color src, ChromaType type /*= ChromaBradford*/)
{
    icFloatNumber chromatic[9];
    icFloatNumber nIllum[3] = {(icFloatNumber)src.X(), (icFloatNumber)src.Y(), (icFloatNumber)src.Z() };
    
    calcChromaticAdaptation(icD50XYZ, nIllum, chromatic, type);
    Matrix res(3, 3);
    for (int i=0;i<9;i++)
        res.at(i/3, i%3) = chromatic[i];
    
    return res;
}

Matrix QubyxProfile::getChromaticAdaptation() const
{
    CIccTagS15Fixed16* chroma = dynamic_cast<CIccTagS15Fixed16*>(profile_.FindTag(icSigChromaticAdaptationTag));
    if (!chroma || chroma->GetSize()<9)
        return Matrix();
    
    Matrix chad(3, 3);
    for (int i=0;i<9;i++)
        chad.at(i/3, i%3) = icFtoD((*chroma)[i]);
    
    return chad;
}

std::vector<double> QubyxProfile::getChromaticAdaptationAsVector() const
{
    CIccTagS15Fixed16* chroma = dynamic_cast<CIccTagS15Fixed16*>(profile_.FindTag(icSigChromaticAdaptationTag));
    if (!chroma || chroma->GetSize()<9)
        return std::vector<double>();

    std::vector<double> res(9);

    for (int i=0;i<9;i++)
        res[i] = icFtoD((*chroma)[i]);

    return res;
}

std::vector<double> QubyxProfile::getRevertChromaticAdaptationAsVector() const
{
    Matrix m = getChromaticAdaptation();
    m = m.inverse();
    if (m.isNull() || m.rows()<3 || m.columns()<3)
        return std::vector<double>();

    std::vector<double> res(9);
    for (int i=0;i<9;i++)
        res[i] = m.at(i/3, i%3);

    return res;
}

bool QubyxProfile::setChromaticAdaptation(Matrix chad)
{
    if (chad.rows()!=3 || chad.columns()!=3)
        return false;
    
    profile_.DeleteTag(icSigChromaticAdaptationTag);
    
    CIccTagS15Fixed16* tag = new CIccTagS15Fixed16(9);
    for (int i=0;i<9;i++)
      (*tag)[i] = icDtoF(chad.at(i/3, i%3));
    profile_.AttachTag(icSigChromaticAdaptationTag, tag);
    
    return true;
}

bool QubyxProfile::haveTag(icSignature tagName)
{
    return (profile_.FindTag(tagName)!=nullptr);
}

bool QubyxProfile::haveTagInCLUT(icSignature clutName, ClutElement element)
{
    CIccMBB* a2b = dynamic_cast<CIccMBB*>(profile_.FindTag(clutName));
    if (!a2b)
        return false;
    
    switch(element)
    {
        case ClutElement::curveA:
        {
            CIccCurve **cc = a2b->GetCurvesA();
            if (!cc)
                return false;
            CIccTagCurve *c = dynamic_cast<CIccTagCurve*>(*cc);
            if (!c)
                return false;
            return (c->GetSize()>1);
            break;
        }
        case ClutElement::curveB:
        {
            CIccCurve **cc = a2b->GetCurvesB();
            if (!cc)
                return false;
            CIccTagCurve *c = dynamic_cast<CIccTagCurve*>(*cc);
            if (!c)
                return false;
            return (c->GetSize()>1);
            break;
        }
        case ClutElement::curveM:
        {
            CIccCurve **cc = a2b->GetCurvesM();
            if (!cc)
                return false;
            CIccTagCurve *c = dynamic_cast<CIccTagCurve*>(*cc);
            if (!c)
                return false;
            return (c->GetSize()>1);
            break;
        }
        case ClutElement::Matrix:
            return (a2b->GetMatrix()!=nullptr);
            break;
        case ClutElement::Lut:
            return (a2b->GetCLUT()!=nullptr);
            break;
    }
    return false;
}

QString QubyxProfile::getDefaultProfilePath()
{
    QString catalog = OSProfile::profilepath();
#ifdef Q_OS_MAC
    catalog += QString("/")+GlobalData::getString(GLOBALDATA_ORGANIZATION_NAME);
#endif
    
    return catalog;
}

void QubyxProfile::setOptionalDescription(QString text, bool useType)
{
    qDebug() << "setOptionalDescription" << text;
    optDescription_ = text;
    skipDescrType_ = !useType;
}

void QubyxProfile::setICCspecification(QubyxProfile::ICCSpec spec)
{
    spec_ = spec;
}

QubyxProfile::ICCSpec QubyxProfile::ICCspecification()
{
    return spec_;
}

QString QubyxProfile::ICCspecificationAsString()
{
    switch(spec_)
    {
    case ICCSpec::ICCv2:
        return "2.1";
    case ICCSpec::ICCv4:
        return "4.0";
    }
    return "4.0";
}

int QubyxProfile::ICCspecificationAsInt()
{
    switch(spec_)
    {
    case ICCSpec::ICCv2:
        return 21;
    case ICCSpec::ICCv4:
        return 40;
    }
    return 40;
}

void QubyxProfile::setTextTag(icSignature sig, QString text)
{
    switch(spec_)
    {
    case ICCSpec::ICCv2:
        {
            profile_.DeleteTag(sig);

            if (sig==icSigCopyrightTag || sig==icSigCharTargetTag)
            {
                CIccTagText *tag = new CIccTagText;
                tag->SetText(text.toLatin1());
                profile_.AttachTag(sig, tag);
            }
            else
            {
                CIccTagTextDescription *tag = new CIccTagTextDescription;
                tag->SetText(text.toLatin1());
                profile_.AttachTag(sig, tag);
            }
        }
        break;

    case ICCSpec::ICCv4:
        {
            profile_.DeleteTag(sig);
            if (sig==icSigCharTargetTag)
            {
                    CIccTagText *tag = new CIccTagText;
                    tag->SetText(text.toLatin1());
                    profile_.AttachTag(sig, tag);
            }
            else
            {
                CIccLocalizedUnicode USAEnglish;
                USAEnglish.SetText(text.utf16());
                CIccTagMultiLocalizedUnicode* tag = new CIccTagMultiLocalizedUnicode;
                tag->m_Strings = new CIccMultiLocalizedUnicode; // dtor does deletion
                tag->m_Strings->push_back(USAEnglish);
                profile_.AttachTag(sig, tag);
            }
        }
        break;
    }
}

QString QubyxProfile::getTextTag(icSignature sig) const
{
    CIccTag* tag = profile_.FindTag(sig);

    if (!tag)
        return "";

    if (dynamic_cast<CIccTagMultiLocalizedUnicode*>(tag))
    {
        CIccTagMultiLocalizedUnicode* mlTag = dynamic_cast<CIccTagMultiLocalizedUnicode*>(tag);
        if (!mlTag->m_Strings)
            return "";

        icChar buff[4096];
        QString res;
        for (auto itr = mlTag->m_Strings->begin();itr!=mlTag->m_Strings->end();++itr)
        {
            itr->GetAnsi(buff, 4096);
            res = res + QString::fromLatin1(buff);
        }
        return res;
    }

    if (dynamic_cast<CIccTagTextDescription*>(tag))
    {
        CIccTagTextDescription* tdTag = dynamic_cast<CIccTagTextDescription*>(tag);
        return QString::fromLatin1(tdTag->GetText());
    }

    if (dynamic_cast<CIccTagText*>(tag))
    {
        CIccTagText* tTag = dynamic_cast<CIccTagText*>(tag);
        return QString::fromLatin1(tTag->GetText());
    }

    return "";
}

QString QubyxProfile::getMeasurementList() const
{
    QString meas = getTextTag(icSigCharTargetTag).trimmed();
//    if (meas[0] == '\"')
//        meas.remove(0, 1);
//    if (meas[meas.size()-1] == '\"')
//        meas.remove(meas.size()-1, 1);

//    meas = meas.trimmed();

    qDebug() << "QubyxProfile::getMeasurementList()" << meas;
    return meas;
}

bool QubyxProfile::setMeasurementList(QString measData)
{
    //measData = "\"" + measData + "\"";
    setTextTag(icSigCharTargetTag, measData);
    return true;
}

template bool QubyxProfile::applyChromaticAdaptation<float>(long double m[9], float XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<double>(long double m[9], double XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<long double>(long double m[9], long double XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<float>(const std::vector<double> &m, float XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<double>(const std::vector<double> &m, double XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<long double>(const std::vector<double> &m, long double XYZ[3]);
template bool QubyxProfile::applyChromaticAdaptation<float>(const std::vector<double> &m, std::vector<float> &XYZ);
template bool QubyxProfile::applyChromaticAdaptation<double>(const std::vector<double> &m, std::vector<double> &XYZ);
