#include "EPDCurve.h"
#include <cmath>

typedef long double lD;

double EPDCurve::GetTarget(double position, double maxposition)
{
    long double x;
    long double res;
    x = lD(position / maxposition);
    res = lD(0.10) + lD(4.060264) * x - lD(6.226862) * powl(x, 2) + lD(48.145864) * powl(x, 3) - lD(60.928632) * powl(x, 4) + lD(49.848766) * powl(x, 5);
    res = (res - lD(0.1)) / lD(34.9) * lD(maxlum_ - minlum_) + lD(minlum_);
//    if (res<minlum_)
//            res = minlum_;
//	if (res>maxlum_)
//		res=maxlum_;
    return double(res);
}

QString EPDCurve::CurveName()
{
    return "EPD";
}
