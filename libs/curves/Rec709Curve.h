/* 
 * File:   Rec709Curve.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 3 березня 2012, 14:27
 */

#ifndef REC709CURVE_H
#define	REC709CURVE_H

#include "TargetCurve.h"

class Rec709Curve : public TargetCurve
{
public:
    Rec709Curve();
    virtual ~Rec709Curve();

    virtual double GetTarget(double position, double maxposition=1);
    
    virtual QString CurveName();
};

#endif	/* REC709CURVE_H */

