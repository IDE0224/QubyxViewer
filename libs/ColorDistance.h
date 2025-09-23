#ifndef COLORDISTANCE_H
#define	COLORDISTANCE_H

#include "Color.h"

class ColorDistance
{
    ColorDistance();

    static Color oldwhite_;

    static void fakeWhite(Color col);
    static void restoreWhite();

    static double dE1976(double L1, double a1, double b1, double L2, double a2, double b2);
    static double dE1994(double L1, double a1, double b1, double L2, double a2, double b2);
    static double dE2000(double L1, double a1, double b1, double L2, double a2, double b2);
public:
    virtual ~ColorDistance();

    static double dE1976Lab(Color col1, Color col2);
    static double dE1976ab(Color col1, Color col2);
    static double dE1976L(Color col1, Color col2);

    static double dE1994Lab(Color col1, Color col2);
    static double dE1994ab(Color col1, Color col2);
    static double dE1994L(Color col1, Color col2);

    static double dE2000Lab(Color col1, Color col2);
    static double dE2000ab(Color col1, Color col2);
    static double dE2000L(Color col1, Color col2);
};

#endif	/* COLORDISTANCE_H */

