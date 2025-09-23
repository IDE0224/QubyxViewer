#ifndef EPDCURVE_H
#define	EPDCURVE_H

#include "TargetCurve.h"

class EPDCurve : public TargetCurve
{
public:
    EPDCurve() = default;
    virtual ~EPDCurve() = default;
    
    virtual double GetTarget(double position, double maxposition=1);
    virtual QString CurveName();
};

#endif	/* EPDCURVE_H */

