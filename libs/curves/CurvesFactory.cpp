/* 
 * File:   CurvesFactory.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 20 січня 2012, 16:17
 */

#include <map>

#include "CurvesFactory.h"

#include "DICOMCurve.h"
#include "EPD3Curve.h"
#include "EPDCurve.h"
#include "CIELCurve.h"
#include "GammaCurve.h"
#include "sRGBCurve.h"
#include "Rec709Curve.h"
#include "rec709_bt1886.h"
#include "CustomCurve.h"

#include <QDebug>

CurvesFactory::CurvesFactory()
{
}

TargetCurve* CurvesFactory::createCurve(QString name, double gamma/*=0*/)
{
    name = name.toLower();
    
    if (name.contains("dicom"))
        return new DICOMCurve;
    if (name.contains("epd3"))
        return new EPD3Curve;
    if (name.contains("epd"))
        return new EPDCurve;
    if (name.contains("gamma"))
        return new GammaCurve(gamma);
    if (name.contains("cie") || name.contains("l*"))
        return new CIELCurve;
    if (name.contains("srgb"))
        return new sRGBCurve;
    if (name.contains("rec709"))
        return new Rec709Curve;
    if (name.contains("bt1886"))
        return new Rec709_BT1886;
    if (name.contains("custom"))
        return new CustomCurve;

    return new DICOMCurve;
}

std::vector<std::pair<QString, double> > CurvesFactory::curvesList()
{
    std::vector<std::pair<QString, double> > res;
    res.push_back(std::make_pair("dicom", 0));
    res.push_back(std::make_pair("epd3", 0));
    res.push_back(std::make_pair("epd", 0));
    res.push_back(std::make_pair("cie l*", 0));
    res.push_back(std::make_pair("srgb", 0));
    res.push_back(std::make_pair("rec709", 0));
    res.push_back(std::make_pair("bt1886", 0));
    
    for (double g=1.2;g<=3;g+=.1)
        res.push_back(std::make_pair("gamma", g));
    
    return res;
}

QString CurvesFactory::curveName(QString name, double gamma/*=0*/)
{   
    static std::map<QString, QString> names;
    if (names.empty())
    {
        std::vector<QString> types;
        types.push_back("dicom");
        types.push_back("epd3");
        types.push_back("epd");
        types.push_back("gamma");
        types.push_back("cie l*");
        types.push_back("srgb");
        types.push_back("rec709");
        types.push_back("bt1886");
        
        TargetCurve* c = nullptr;
        for (std::vector<QString>::const_iterator itr=types.begin();itr!=types.end();++itr)
        {
            c = createCurve(*itr, gamma);
            if (c)
            {
                names[*itr] = c->CurveName();
                delete c;
                c = nullptr;
            }
        }
    }
    
    QString res;
    
    std::map<QString, QString>::iterator it = names.find(name.toLower());
    if (it!=names.end())
        res = it->second;
    else
        res = name;
    
    if (name.toLower().contains("gamma") && gamma>0)
        res += QString(" ") + QString::number(gamma);
    
    return res;
}

bool CurvesFactory::initCustomCurve(TargetCurve *curve, std::vector<double> ddl, std::vector<double> lum, QString name)
{
    CustomCurve *customCurve = dynamic_cast<CustomCurve*>(curve);
    if (customCurve)
    {
        customCurve->SetName(name);
        return customCurve->SetData(ddl, lum);
    }
    else
        return false;
}