/* 
 * File:   ColorUnitsConversion.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 13 липня 2011, 12:35
 */

#include "ColorUnitsConversion.h"

#include <cmath>
typedef long double ldouble;

ColorUnitsConversion::ColorUnitsConversion()
{
}

long double ColorUnitsConversion::cdtofL(long double cd)
{
    auto value = cd / ldouble(3.4262591);
    value = ldouble(round(double(value) * 100) / 100);
    return value;
}

long double ColorUnitsConversion::fLtoCd(long double fL)
{
    auto value = fL * ldouble(3.4262591);
    value = ldouble(round(double(value) * 100) / 100);
    return value;
}
