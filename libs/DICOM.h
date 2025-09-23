#ifndef DICOM_H
#define	DICOM_H

#include <vector>

class DICOM
{
    DICOM();
public:
    static double GetJNDIndex(double L);
    static double GetLuminanceFromJNDIndex(double JNDindex);
    static void GetGraphInfo(const std::vector<double> &ddls, const std::vector<double> &luminance, double &maxerror, std::vector<std::pair<double,double> > &points, std::vector<std::pair<double,double> > &targetpoints);
};

#endif	/* DICOM_H */

