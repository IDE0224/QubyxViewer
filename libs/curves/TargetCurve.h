#ifndef TARGETCURVE_H
#define	TARGETCURVE_H

#include <vector>
#include <QString>

class TargetCurve
{
protected:
    double minlum_, maxlum_;

public:
    TargetCurve();
    virtual ~TargetCurve();

    virtual void SetMinMaxLuminance(double minlum, double maxlum);
    virtual void GetMinMaxLuminance(double& minlum, double& maxlum);

    virtual void GetTarget(std::vector<double> &target, int count);
    virtual void GetTarget(std::vector<double> &target, const std::vector<double> &points, int maxposition=1);
    virtual double GetTarget(double position, double maxposition=1)=0;
    virtual double GetPosition(double lum, double maxposition=1);

    virtual QString CurveName()=0;
};

#endif	/* TARGETCURVE_H */

