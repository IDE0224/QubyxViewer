/* 
 * File:   CurvesFactory.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 20 січня 2012, 16:17
 */

#ifndef CURVESFACTORY_H
#define	CURVESFACTORY_H

#include "TargetCurve.h"

#include <vector>
#include <map>

class CurvesFactory 
{
    CurvesFactory();
public:
    static TargetCurve* createCurve(QString name, double gamma=0);
    static QString curveName(QString name, double gamma=0);
    static bool initCustomCurve(TargetCurve *curve, std::vector<double> ddl, std::vector<double> lum, QString name);
    
    static std::vector<std::pair<QString, double> > curvesList();
    
};

#endif	/* CURVESFACTORY_H */

