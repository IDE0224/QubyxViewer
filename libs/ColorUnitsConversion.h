/* 
 * File:   ColorUnitsConversion.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 13 липня 2011, 12:35
 */

#ifndef COLORUNITSCONVERSION_H
#define	COLORUNITSCONVERSION_H

class ColorUnitsConversion 
{
    ColorUnitsConversion();
public:

    static long double cdtofL(long double cd);
    static long double fLtoCd(long double fL);
};

#endif	/* COLORUNITSCONVERSION_H */

