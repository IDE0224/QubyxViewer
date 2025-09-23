#include "rec709_bt1886.h"

#include <algorithm>
#include <cmath>

typedef  long double lDouble;

double Rec709_BT1886::GetTarget(double position, double maxposition)
{
    lDouble p = lDouble(position / maxposition);
    lDouble g = lDouble(2.4);

    long double a = powl(powl(lDouble(maxlum_), 1 / g) - powl(lDouble(minlum_), 1 / g), g);
    long double b = powl(lDouble(minlum_), 1 / g) / (powl(lDouble(maxlum_), 1 / g) - powl(lDouble(minlum_), 1 / g));

    return double(a * powl(std::max <long double>(p+b, 0), g));
}

QString Rec709_BT1886::CurveName()
{
    return "BT.1886";
}

