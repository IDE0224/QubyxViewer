/* 
 * File:   sRGBCurve.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 23 січня 2012, 15:18
 */

#include "sRGBCurve.h"

#include <cmath>

sRGBCurve::sRGBCurve()
{
}

sRGBCurve::~sRGBCurve()
{
}

double sRGBCurve::GetTarget(double position, double maxposition/*=1*/)
{
    double p = position/maxposition;
    if (p<0.04045)
        return minlum_ +  p/12.92*(maxlum_-minlum_);
    
    const double a = 0.055;
    return minlum_ + pow((p+a)/(1+a), 2.4)*(maxlum_-minlum_);
}

double sRGBCurve::GetPosition(double lum, double maxposition/*=1*/)
{
    const double a = 0.055;
    double Y = (lum-minlum_)/(maxlum_-minlum_);
    
    if (Y <= 0.0031308)
        return Y*12.92*maxposition;
    else
        return ((1.0 + a)*std::pow(Y, 1/2.4) - a)*maxposition;
}

QString sRGBCurve::CurveName()
{
    return "sRGB";
}

