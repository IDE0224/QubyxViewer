#include "ColorDistance.h"

#include "mathconst.h"
#include <cmath>

#include <QDebug>

Color ColorDistance::oldwhite_=Color::FromXYZ(0, 0, 0);

ColorDistance::ColorDistance()
{
}

ColorDistance::~ColorDistance()
{
}

void ColorDistance::fakeWhite(Color col)
{
    oldwhite_ = Color::GetWhiteReference();
    if (oldwhite_.X()==0 && oldwhite_.Y()==0 && oldwhite_.Z()==0)
        Color::SetWhiteReference(col);
}

void ColorDistance::restoreWhite()
{
    Color::SetWhiteReference(oldwhite_);
}

double ColorDistance::dE1976(double L1, double a1, double b1, double L2, double a2, double b2)
{
    return sqrt(pow(L1-L2,2) + pow(a1-a2, 2) + pow(b1-b2,2));
}

double ColorDistance::dE1976Lab(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE1976(col1.L(), col1.a(), col1.b(), col2.L(), col2.a(), col2.b());

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE1976ab(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE1976(0.0, col1.a(), col1.b(), 0.0, col2.a(), col2.b());

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE1976L(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE1976(col1.L(), 0, 0, col2.L(), 0, 0);

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE1994(double L1, double a1, double b1, double L2, double a2, double b2)
{
    double K1=0.045, K2=0.015;
    double KC=1, KH=1, KL=1;
    double C1 = sqrt(a1*a1+b1*b1);
    double C2 = sqrt(a2*a2+b2*b2);
    double SL = 1, SC = 1+K1*C1, SH = 1 + K2*C1;
    double dC = (C1-C2);

    double dH = pow(a1-a2, 2) + pow(b1-b2, 2) - dC*dC;
    if (dH<0)
        dH = 0;
    dH = sqrt(dH);

    return sqrt(pow((L1-L2)/(KL*SL),2)+pow(dC/(KC*SC), 2) + pow(dH/(KH*SH),2));
}

double ColorDistance::dE1994Lab(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE1994(col1.L(), col1.a(), col1.b(), col2.L(), col2.a(), col2.b());

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE1994ab(Color col1, Color col2)
{
    fakeWhite(Color::FromxyY(0.3127, 0.3290, 100));     //sRGB
    col1.ReCalc();
    col2.ReCalc();
    
//    Color wh = Color::GetWhiteReference();
//    double level =  std::min(wh.Y(), std::max(col1.Y(), col2.Y()));
//    col1 = Color::FromxyY(col1.x(), col1.y(), level);
//    col2 = Color::FromxyY(col2.x(), col2.y(), level);

    double res = dE1994(0, col1.a(), col1.b(), 0, col2.a(), col2.b());
    restoreWhite();
    return res;

}

double ColorDistance::dE1994L(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE1994(col1.L(), 0, 0, col2.L(), 0, 0);

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE2000(double L1, double a1, double b1, double L2, double a2, double b2)
{  
    double KL = 1, KC = 1, KH = 1;
    double mL = (L1+L2)/2;
    double mC = (sqrt(a1*a1+b1*b1)+sqrt(a2*a2+b2*b2))/2;
    double G = (1-sqrt(pow(mC, 7)/(pow(mC,7)+pow(25.,7))))/2;

    double a11 = a1*(1+G), a12 = a2*(1+G);
    double mC1 = (sqrt(a11*a11+b1*b1) + sqrt(a12*a12+b2*b2))/2;

    double h1 = atan2(b1, a11), h2 = atan2(b2, a12);
    if (h1<0)
        h1 += 2*M_PI;
    if (h2<0)
        h2 += 2*M_PI;
    double mH1;
    if (fabs(h2-h1)>M_PI)
        mH1 = (h1+h2+2*M_PI)/2;
    else
        mH1 = (h1+h2)/2;

    double T = 1 - 0.17*cos(mH1-M_PI/6) + 0.24*cos(2*mH1) + 0.32*cos(3*mH1 + 6*M_PI/180) - 0.2*cos(4*mH1-63*M_PI/180);

    double dh;
    if (fabs(h2-h1)<=M_PI)
        dh = h2-h1;
    else
        if (h2<=h1)
            dh = h2-h1+2*M_PI;
        else
            dh = h2-h1-2*M_PI;

    double dC1 = sqrt(a12*a12+b2*b2) - sqrt(a11*a11+b1*b1);
    double dH = 2*sqrt(sqrt(a11*a11+b1*b1)*sqrt(a12*a12+b2*b2))*sin(dh/2);

    double SL = 1 + 0.015*pow(mL-50, 2)/sqrt(20+pow(mL-50,2));
    double SC = 1 + 0.045*mC1;
    double SH = 1 + 0.015*mC1*T;

    //double RT = -2*sqrt(pow(mC1, 7)/(pow(mC1, 7)+pow(25., 7))) * sin(2*30*exp( -pow((mH1-275*M_PI/180)/25,2) ) *M_PI/180 );
    double RT = -2*sqrt(pow(mC1, 7)/(pow(mC1, 7)+pow(25., 7))) * sin(2*30*exp( -pow((mH1/M_PI*180.0-275)/25,2) ) *M_PI/180 );

    return sqrt(pow((L1-L2)/(KL*SL),2)+pow(dC1/(KC*SC), 2) + pow(dH/(KH*SH),2) + RT*(dC1/(KC*SC))*dH/(KH*SH));
}

double ColorDistance::dE2000Lab(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE2000(col1.L(), col1.a(), col1.b(), col2.L(), col2.a(), col2.b());

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}

double ColorDistance::dE2000ab(Color col1, Color col2)
{   
    fakeWhite(Color::FromxyY(0.3127, 0.3290, 100));     //sRGB
    col1.ReCalc();
    col2.ReCalc();
    
//    Color wh = Color::GetWhiteReference();
//    double level =  std::min(wh.Y(), std::max(col1.Y(), col2.Y()));
//    col1 = Color::FromxyY(col1.x(), col1.y(), level);
//    col2 = Color::FromxyY(col2.x(), col2.y(), level);

    double res = dE2000(0, col1.a(), col1.b(), 0, col2.a(), col2.b());
    restoreWhite();

    return res;
}

double ColorDistance::dE2000L(Color col1, Color col2)
{
    fakeWhite(col1);
    col1.ReCalc();
    col2.ReCalc();

    double res = dE2000(col1.L(), 0, 0, col2.L(), 0, 0);

    restoreWhite();
    col1.ReCalc();
    col2.ReCalc();

    return res;
}
