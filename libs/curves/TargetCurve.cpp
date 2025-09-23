#include "TargetCurve.h"

#include <cmath>

typedef long double lDouble;

TargetCurve::TargetCurve() :minlum_(1), maxlum_(100)
{
    //
}

TargetCurve::~TargetCurve()
{
    //
}

void TargetCurve::SetMinMaxLuminance(double minlum, double maxlum)
{
    minlum_ = minlum;
    maxlum_ = maxlum;
}

void TargetCurve::GetMinMaxLuminance(double& minlum, double& maxlum)
{
    minlum = minlum_;
    maxlum = maxlum_;
}

void TargetCurve::GetTarget(std::vector<double> &target, int count)
{
    target.clear();
    for (int i=0;i<count;i++)
        target.push_back(GetTarget(i, count-1));
}

void TargetCurve::GetTarget(std::vector<double> &target, const std::vector<double> &points, int maxposition)
{
    target.clear();
    for (unsigned i=0;i<points.size();i++)
        target.push_back(GetTarget(points[i], maxposition));
}

double TargetCurve::GetPosition(double lum, double maxposition)
{
    lDouble minpos, maxpos, cpos, clum;

    minpos = lDouble(-maxposition);
    maxpos = lDouble(2 * maxposition);

//    if (lum<=minlum_)
//        return 0;
//    if (lum>=maxlum_)
//        return maxposition;

    while(true){
        cpos = minpos + (maxpos - minpos) / 2;
        clum = lDouble(GetTarget(double(cpos), maxposition));
        if (fabsl(clum - lDouble(lum)) < lDouble(0.000001) || fabsl(minpos - maxpos) < lDouble(0.000001))
            break;
        if (clum < lDouble(lum))
            minpos = cpos;
        else
            maxpos = cpos;
    }
    return double(cpos);
}
