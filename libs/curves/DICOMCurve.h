#ifndef DICOMCURVE_H
#define	DICOMCURVE_H

#include "TargetCurve.h"

class DICOMCurve : public TargetCurve
{
public:
    DICOMCurve() = default;
    virtual ~DICOMCurve() = default;

    virtual void GetTarget(std::vector<double> &target, int count);
    virtual double GetTarget(double position, double maxposition=1);
    virtual double GetPosition(double lum, double maxposition=1);

    virtual QString CurveName();
};

#endif	/* DICOMCURVE_H */

