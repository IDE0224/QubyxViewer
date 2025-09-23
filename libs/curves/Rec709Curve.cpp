/* 
 * File:   Rec709Curve.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 3 березня 2012, 14:26
 */

#include "Rec709Curve.h"

#include <cmath>

Rec709Curve::Rec709Curve() 
{
}

Rec709Curve::~Rec709Curve() {
}

double Rec709Curve::GetTarget(double position, double maxposition/*=1*/)
{
    double p = position/maxposition;
    if (p<0.0812428582986315)
        return minlum_ +  p/4.5*(maxlum_-minlum_);
    
    const double a = 0.09929682680944;
    return minlum_ + pow((p+a)/(1+a), 20./9)*(maxlum_-minlum_);
}

QString Rec709Curve::CurveName()
{
    return "BT.709";
}
