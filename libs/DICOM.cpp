#include "DICOM.h"

#include <cmath>
#include <vector>
#include <algorithm>

DICOM::DICOM()
{
}

double DICOM::GetJNDIndex(double L)
{
    if (L<.01)
        return GetJNDIndex(.01);

    long double A = 71.498068l;
    long double B = 94.593053l;
    long double C = 41.912053l;
    long double D = 9.8247004l;
    long double F = -1.1878455l;
    long double E = 0.28175407l;
    long double G = -0.18014349l;
    long double H = 0.14710899l;
    long double I = -0.017046845l;
    long double x = log10l(static_cast<long double>(L));
    double JND =static_cast<double>(A+B*x+C*powl(x,2)+D*powl(x,3)+E*powl(x,4)+F*powl(x,5)+G*powl(x,6)+H*powl(x,7)+I*powl(x,8));
    return JND;
}


double DICOM::GetLuminanceFromJNDIndex(double JNDindex)
{
        long double piv=JNDindex;
        long double x = log(piv);
        long double A = (long double)-1.3011877;
        long double B = (long double)-0.025840191;
        long double C = (long double)0.080242636;
        long double D = (long double)-0.10320229;
        long double E = (long double)0.13646699;
        long double F = (long double)0.028745620;
        long double G = (long double)-0.025468404;
        long double H = (long double)-0.0031978977;
        long double K = (long double)0.00012992634;
        long double M = (long double)0.0013635334;
        long double lo = (A+C*x+E*pow(x,2)+G*pow(x,3)+M*pow(x,4))/(1+B*x+D*pow(x,2)+F*pow(x,3)+H*pow(x,4)+K*pow(x,5));
        return (double)powl((long double)10.0,lo);
}

void DICOM::GetGraphInfo(const std::vector<double> &ddls, const std::vector<double> &luminance, double &maxerror,
        std::vector<std::pair<double,double> > &points, std::vector<std::pair<double,double> > &targetpoints)
{
    double minl, maxl;
    int i;
    double m_jndmin;
    double m_jndmax;
    
    std::vector<double> target;

    double maxddl = *std::max_element(ddls.begin(), ddls.end());
    double minddl = *std::min_element(ddls.begin(), ddls.end());

    minl = *std::min_element(luminance.begin(), luminance.end());
    maxl = *std::max_element(luminance.begin(), luminance.end());
    m_jndmin = DICOM::GetJNDIndex(minl);
    m_jndmax = DICOM::GetJNDIndex(maxl);

    int pointcount = luminance.size();

    target.push_back(luminance[0]);
    for (i=1;i<pointcount-1;i++)
        target.push_back(DICOM::GetLuminanceFromJNDIndex(m_jndmin + (m_jndmax-m_jndmin)*(ddls[i]-minddl)/(maxddl-minddl)));
//        target.push_back(DICOM::GetLuminanceFromJNDIndex(m_jndmin + (m_jndmax-m_jndmin)*i/(pointcount-1)));
    target.push_back(luminance[pointcount-1]);

    double dj = (m_jndmax - m_jndmin)/(pointcount-1);
    double di, ditarget;
    double error;
    double x;

    for (i=0;i<pointcount-1;i++)
    {
            dj = DICOM::GetJNDIndex(target[i+1]) - DICOM::GetJNDIndex(target[i]);
            if (fabsl(dj)<0.000001)
                dj = 0.000001;

            di = luminance[i+1] - luminance[i];
            di /= (luminance[i+1] + luminance[i])/2.;
            di /= dj;

            ditarget = target[i+1] - target[i];
            ditarget /= (target[i+1] + target[i])/2.;
            ditarget /= dj;
 
            x =(DICOM::GetJNDIndex(target[i+1]) + DICOM::GetJNDIndex(target[i]))/2;

            points.push_back(std::make_pair(x, di));
            targetpoints.push_back(std::make_pair(x, ditarget));

            if (fabsl(ditarget)>0.00001)
                error = fabs(di - ditarget)*(100./ditarget);
            else
                error = 100;

            maxerror = (i==0 ? error : std::max<double>(error, maxerror));
    }
}
