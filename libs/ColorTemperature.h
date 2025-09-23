#ifndef COLORTEMPERATURE_H
#define	COLORTEMPERATURE_H

class ColorTemperature
{
    ColorTemperature();
public:
    virtual ~ColorTemperature();

    static void Ttoxy(double T, double &x, double &y);
    static double xytoT(double x, double y);
};

#endif	/* COLORTEMPERATURE_H */

