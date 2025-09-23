/* 
 * File:   CustomCurve.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 28 квітня 2014, 10:00
 */

#ifndef CUSTOMCURVE_H
#define	CUSTOMCURVE_H

#include "TargetCurve.h"
#include <vector>

class CustomCurve : public TargetCurve
{
public:
    CustomCurve();
    virtual ~CustomCurve();
    
    virtual double GetTarget(double position, double maxposition=1);
    virtual QString CurveName();
    
    void SetName(QString name);
    bool SetData(std::vector<double> ddl, std::vector<double> lum);
    bool IsValid();
private:
    bool valid_;
    QString name_;
    
    std::vector<double> ddl_, lum_;
};

#endif	/* CUSTOMCURVE_H */

