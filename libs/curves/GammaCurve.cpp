#include "GammaCurve.h"

#include <cmath>

GammaCurve::GammaCurve(double gamma) : gamma_(gamma){}

double GammaCurve::GetTarget(double position, double maxposition)
{
    return minlum_ + pow(position/maxposition, gamma_)*(maxlum_-minlum_);
}

double GammaCurve::GetPosition(double lum, double maxposition)
{
    if (lum<minlum_)
        return -pow(-(lum-minlum_)/(maxlum_-minlum_), 1./gamma_)*maxposition;

    return pow((lum-minlum_)/(maxlum_-minlum_), 1./gamma_)*maxposition;
}

QString GammaCurve::CurveName()
{
    return "Gamma";
}
