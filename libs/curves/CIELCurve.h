#ifndef CIELCURVE_H
#define	CIELCURVE_H

#include "TargetCurve.h"

class CIELCurve : public TargetCurve
{
public:
    CIELCurve();
    virtual ~CIELCurve();

    virtual double GetTarget(double position, double maxposition=1);
    virtual double GetPosition(double lum, double maxposition=1);

    virtual QString CurveName();
};

#endif	/* CIELCURVE_H */

