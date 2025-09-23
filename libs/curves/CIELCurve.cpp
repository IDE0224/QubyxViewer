#include "CIELCurve.h"

#include <cmath>

CIELCurve::CIELCurve()
{
}

CIELCurve::~CIELCurve()
{
}

double CIELCurve::GetTarget(double position, double maxposition)
{
    if (position<=maxposition*216./(27*100))
            return minlum_ + position*(maxlum_-minlum_)*(100*27.)/(maxposition*24389.);

    return minlum_ + (maxlum_-minlum_)*powl((100.*position/maxposition+16)/116, 3);
}

double CIELCurve::GetPosition(double lum, double maxposition)
{
    long double Y = (lum-minlum_)/(maxlum_-minlum_);
    double r;
    if (Y<=216./24389)
            r = Y*maxposition*24389./(27*100);
    else
            r = maxposition*(116*pow(Y, (long double)(1)/3.)-16)/100.;

//    if (r<0)
//        r = 0;
//    if (r>maxposition)
//        r = maxposition;

    return r;
}

QString CIELCurve::CurveName()
{
    return "CIE L*";
}