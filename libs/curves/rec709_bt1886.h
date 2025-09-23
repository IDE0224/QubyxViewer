#ifndef REC709_BT1886_H
#define REC709_BT1886_H

#include "TargetCurve.h"

class Rec709_BT1886 : public TargetCurve
{
public:
    Rec709_BT1886() = default;

    virtual double GetTarget(double position, double maxposition=1);

    virtual QString CurveName();
};

#endif // REC709_BT1886_H
