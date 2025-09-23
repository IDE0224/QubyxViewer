/* 
 * File:   EPD3Curve.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 18 березня 2011, 15:36
 */

#ifndef EPD3CURVE_H
#define	EPD3CURVE_H

#include "TargetCurve.h"


class EPD3Curve : public TargetCurve
{
    double xmin_, xmax_;

    double GetTargetWithoutCheck(double position, double maxposition=1);
public:
    EPD3Curve();
    virtual ~EPD3Curve();

    virtual double GetTarget(double position, double maxposition=1);
    virtual QString CurveName();

    virtual void SetMinMaxLuminance(double minlum, double maxlum);
};

#endif	/* EPD3CURVE_H */

