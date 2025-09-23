/* 
 * File:   EPD3Curve.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 18 березня 2011, 15:36
 */

#include "EPD3Curve.h"
#include "../ColorUnitsConversion.h"
#include <cmath>
#include <QDebug>

typedef long double lD;

EPD3Curve::EPD3Curve() : xmin_(0), xmax_(1)
{
    //
}

EPD3Curve::~EPD3Curve()
{
    //
}

double EPD3Curve::GetTargetWithoutCheck(double position, double maxposition)
{
    lD x;
    lD res;
    x = lD(xmax_- xmin_) * lD((position)) / lD(maxposition) + lD(xmin_);
    res = lD(0.2) + lD(8.1206638) * x - lD(12.453941) * powl(x, 2) + lD(96.293375) * powl(x, 3) - lD(121.85936) * powl(x, 4) + lD(99.699238) * powl(x, 5);
    return double(ColorUnitsConversion::fLtoCd(res));
}

double EPD3Curve::GetTarget(double position, double maxposition)
{
    lD res = lD(GetTargetWithoutCheck(position, maxposition));
//    if (res<minlum_)
//            res = minlum_;
//	if (res>maxlum_)
//		res=maxlum_;
    return double(res);
}

QString EPD3Curve::CurveName()
{
    return "EPD3";
}

void EPD3Curve::SetMinMaxLuminance(double minlum, double maxlum)
{
    minlum_ = minlum;
    maxlum_ = maxlum;

    xmax_ = 1;
    xmin_ = 0;

    double xmin, xmax;

    long double minpos, maxpos, cpos, clum;

    //find xmin
    minpos = 0;
    maxpos = 1;

    while (GetTargetWithoutCheck(double(minpos)) > minlum)
        minpos -= 1;

    while (GetTargetWithoutCheck(double(maxpos)) < maxlum)
        maxpos += 1;

    while (true){
        cpos = minpos + (maxpos - minpos) / 2;
        clum = lD(GetTargetWithoutCheck(double(cpos)));
        if (fabsl(clum - lD(minlum)) < lD(0.000001) || fabsl(minpos - maxpos) < lD(0.000001))
            break;
        if (clum < lD(minlum))
            minpos = cpos;
        else
            maxpos = cpos;
    }

    xmin = double(cpos);

    //find xmax
    minpos = 0;
    maxpos = 1;

    while (GetTargetWithoutCheck(double(minpos)) > minlum)
        minpos -= 1;

    while (GetTargetWithoutCheck(double(maxpos)) < maxlum)
        maxpos += 1;

    while (true){
        cpos = minpos + (maxpos - minpos) / 2;
        clum = lD(GetTargetWithoutCheck(double(cpos)));
        if (fabsl(clum - lD(maxlum)) < lD(0.000001) || fabsl(minpos - maxpos) < lD(0.000001))
            break;
        if (clum < lD(maxlum))
            minpos = cpos;
        else
            maxpos = cpos;
    }

    xmax = double(cpos);

    xmin_ = xmin;
    xmax_ = xmax;
}
