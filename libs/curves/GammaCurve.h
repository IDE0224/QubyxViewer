#ifndef GAMMACURVE_H
#define	GAMMACURVE_H

#include "TargetCurve.h"

class GammaCurve : public TargetCurve
{
    double gamma_;
public:
    GammaCurve(double gamma);

    virtual double GetTarget(double position, double maxposition=1);
    virtual double GetPosition(double lum, double maxposition=1);

    virtual QString CurveName();
};

#endif	/* GAMMACURVE_H */

