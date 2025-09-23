/* 
 * File:   sRGBCurve.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 23 січня 2012, 15:18
 */

#ifndef SRGBCURVE_H
#define	SRGBCURVE_H

#include "TargetCurve.h"

class sRGBCurve : public TargetCurve
{
public:
    sRGBCurve();
    virtual ~sRGBCurve();

    virtual double GetTarget(double position, double maxposition=1);
    virtual double GetPosition(double lum, double maxposition=1);
    
    virtual QString CurveName();
};

#endif	/* SRGBCURVE_H */

