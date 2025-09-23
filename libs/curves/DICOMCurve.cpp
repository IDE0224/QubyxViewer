#include "DICOMCurve.h"

#include <cmath>
#include "../DICOM.h"

void DICOMCurve::GetTarget(std::vector<double> &target, int count)
{
    target.clear();
    double minjnd = DICOM::GetJNDIndex(minlum_);
    double maxjnd = DICOM::GetJNDIndex(maxlum_);
    for (int i=0;i<count;i++)
        target.push_back(DICOM::GetLuminanceFromJNDIndex(minjnd  + (maxjnd - minjnd)*i/(count-1)));
}

double DICOMCurve::GetTarget(double position, double maxposition)
{
    double minjnd = DICOM::GetJNDIndex(minlum_);
    double maxjnd = DICOM::GetJNDIndex(maxlum_);
    return DICOM::GetLuminanceFromJNDIndex(minjnd  + (maxjnd - minjnd)*position/maxposition);
}

double DICOMCurve::GetPosition(double lum, double maxposition)
{
    typedef long double lDouble;
    double minjnd = DICOM::GetJNDIndex(minlum_);
    double maxjnd = DICOM::GetJNDIndex(maxlum_);
    double jndc   = DICOM::GetJNDIndex(lum);
    double res = (jndc-minjnd)*maxposition/(maxjnd-minjnd);
    if (jndc>maxjnd && fabsl(lDouble(lum - maxlum_)) < lDouble(1e-3))
            res = maxposition;
    if (jndc < minjnd && fabsl(lDouble(lum - minlum_)) < lDouble(1e-3))
            res = 0;
    return res;
}

QString DICOMCurve::CurveName()
{
    return "DICOM";
}
