/* 
 * File:   CustomCurve.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 28 квітня 2014, 10:00
 */

#include "CustomCurve.h"
#include "Interpolation.h"
#include <QDebug>

#include <algorithm>
#include <cmath>

CustomCurve::CustomCurve() : valid_(false), name_("Unnamed curve")
{
}

CustomCurve::~CustomCurve()
{
}

double CustomCurve::GetTarget(double position, double maxposition/*=1*/)
{
    position /= maxposition;
    
    if (position < 0.0)
        return minlum_ - (maxlum_ - minlum_);
    if (position > 1.0)
        return maxlum_ + (maxlum_ - minlum_);
    
    
    std::vector<double> x = {position}, y;
    if (!Interpolation::SplineHermite(ddl_, lum_, x, y) )
    {
        qDebug() << "interpolation error" << position;
        return 0.0;
    }
    
    return minlum_ + y[0]*(maxlum_ - minlum_);
}

QString CustomCurve::CurveName()
{
    return name_;
}

void CustomCurve::SetName(QString name)
{
    name_ = name;
}

bool CustomCurve::SetData(std::vector<double> ddl, std::vector<double> lum)
{
    lum_.clear();
    ddl_.clear();
    
    if (ddl.size() != lum.size() || ddl.size() < 4)
    {
        qDebug() << "wrong size" << ddl.size() << lum.size();
        valid_ = false;
        return false;
    }
       
    double maxDDL = *std::max_element(ddl.begin(), ddl.end());
    double maxLum = *std::max_element(lum.begin(), lum.end());
    
    if (maxDDL <= 0.0 || maxLum <= 0.0)
    {
        qDebug() << "wrong data" << maxDDL << maxLum;
        valid_ = false;
        return false;
    }
    
    for (uint i = 0; i < ddl.size(); i++)
        if (ddl[i] < 0 || lum[i] < 0){
            qDebug() << "Negative Value" << ddl[i] << lum[i] << i;
            valid_ = false;
            return false;
        }
    
    std::vector<std::pair<double, double> > P;
    for (uint i = 0; i < ddl.size(); i++)
        P.push_back(std::make_pair(ddl[i]/maxDDL, lum[i]/maxLum) );
    
    std::sort(P.begin(), P.end() );
    
    for (uint i = 1; i < P.size(); i++){
        if (std::abs(P[i-1].first - P[i].first) < 1.0e-6)
        {
            qDebug() << "Equal DDL";
            valid_ = false;
            return false;
        }
        
        if (P[i-1].second >= P[i].second)
        {
            qDebug() << "Non-monotonic curve" << P[i-1].second << P[i].second << i;
            valid_ = false;
            return false;
        }
    }
    
    for (uint i = 0; i < P.size(); i++)
    {
        ddl_.push_back(P[i].first);
        lum_.push_back(P[i].second);
    }
    
    valid_ = true;
    return true;
}

bool CustomCurve::IsValid()
{
    return valid_;
}
