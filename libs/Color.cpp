#include "Color.h"
#include "Interpolation.h"
#include "mathconst.h"

#include <cmath>
#include <algorithm>

const long double e_ = 216./24389;
const long double k_ = 24389./27;

const Color Color::D50 = Color::FromXYZ(0.9642, 1.0, 0.8249);
const Color Color::D65 = Color::FromxyY(0.31271, 0.32902, 1.0);
Color Color::Ref_White_ = Color::D50;

Color::Color()
{
    space_ = SpaceRepresentation::Space_None;
    X_ = Y_ = Z_ = x_ = y_ = 0;
    L_ = 100; a_ = b_ = 0;
    //u_ = v_ = 0.0;
}

Color::~Color()
{
}

Color  Color::FromXYZ(double X, double Y, double Z)
{
    Color newcolor;
    newcolor.space_ = SpaceRepresentation::Space_XYZ;
    newcolor.X_ = X;
    newcolor.Y_ = Y;
    newcolor.Z_ = Z;
    newcolor.ReCalc();
    return newcolor;
}

Color Color::FromxyY(double x, double y, double Y)
{
    Color newcolor;
    newcolor.space_ = SpaceRepresentation::Space_xyY;
    newcolor.x_ = x;
    newcolor.y_ = y;
    newcolor.Y_ = Y;
    newcolor.ReCalc();
    return newcolor;
}

Color Color::FromLab(double L, double a, double b)
{
    Color newcolor;
    newcolor.space_ = SpaceRepresentation::Space_Lab;
    newcolor.L_ = L;
    newcolor.a_ = a;
    newcolor.b_ = b;
    newcolor.ReCalc();
    return newcolor;
}

//Color Color::FromLuv(double L, double u, double v)
//{
//    Color newcolor;
//    newcolor.space_ = SpaceRepresentation::Space_Luv;
//    newcolor.L_ = L;
//    newcolor.u_ = u;
//    newcolor.v_ = v;
//    newcolor.ReCalc();
//    return newcolor;
//}

bool Color::operator==(const Color &other) const
{
    return (X() == other.X()) && (Y() == other.Y()) && (Z() == other.Z());
}

void Color::SetWhiteReference(const Color& white)
{
    Ref_White_ = white;
}

Color Color::GetWhiteReference()
{
    return Ref_White_;
}

double Color::u1() const
{
    if (fabs(X_+15*Y_+3*Z_)<1e-4)
        return 0;
        
    return 4*X_/(X_+15*Y_+3*Z_);
}

double Color::v1() const
{
    if (fabs(X_+15*Y_+3*Z_)<1e-4)
        return 0;
    
    return 9*Y_/(X_+15*Y_+3*Z_);
}

void Color::XYZtoxyY()
{
    if (fabsl(X_+Y_+Z_)<0.0001 || fabsl(Y_)<0.0001)
    {
        x_ = Ref_White_.x();
        y_ = Ref_White_.y();
        return;
    }

    x_ = X_/(X_+Y_+Z_);
    y_ = Y_/(X_+Y_+Z_);
}

void Color::xyYtoXYZ()
{
    if (fabsl(y_)<0.0001)
    {
        X_ = Y_ = Z_ = 0;
        return;
    }

    X_ = x_*Y_/y_;
    Z_ = (1-x_-y_)*Y_/y_;
}

void Color::XYZtoLab()
{
    if (Ref_White_.space_==SpaceRepresentation::Space_None || ((Ref_White_.X_==0) && (Ref_White_.Y_==0) && (Ref_White_.Z_==0)) )
    {
        L_ = 100;
        a_ = b_ = 0;
        return;
    }

    long double xr, yr, zr, fx, fy, fz;
    xr = X_/Ref_White_.X_;
    yr = Y_/Ref_White_.Y_;
    zr = Z_/Ref_White_.Z_;

    fx = (xr > e_) ? cbrtl(xr) : (k_*xr+16)/116;
    fy = (yr > e_) ? cbrtl(yr) : (k_*yr+16)/116;
    fz = (zr > e_) ? cbrtl(zr) : (k_*zr+16)/116;

    L_ = 116*fy-16;
    a_ = 500*(fx-fy);
    b_ = 200*(fy-fz);
}

void  Color::LabtoXYZ()
{
    long double xr, yr, zr, fx, fy, fz;

    fy = (L_ + 16)/116;
    fz = fy - b_/200;
    fx = a_/500 + fy;

    //xr = std::pow(fx, 3.)>e_ ? std::pow(fx, 3.) : (116*fx-16)/k_;
    xr = std::pow(fx, 3.);
    if (xr <= e_)
        xr = (116*fx-16)/k_;
    yr = L_ > k_*e_ ? std::pow((L_+16)/116, 3.) : L_/k_;
    //zr = std::pow(fz, 3.)>e_ ? std::pow(fz, 3.) : (116*fz-16)/k_;
    zr = std::pow(fz, 3.);
    if (zr <= e_)
        zr = (116*fz-16)/k_;

    X_ = xr*Ref_White_.X_;
    Y_ = yr*Ref_White_.Y_;
    Z_ = zr*Ref_White_.Z_;
}

//void Color::LuvtoXYZ()
//{
//    double u0 =  Ref_White_.u1();
//    double v0 =  Ref_White_.v1();
    
//    const double k = 903.3, e = 0.008856;
//    if (L_ > k*e)
//        Y_ = std::pow((L_ + 16.0)/116.0, 3);
//    else
//        Y_ = L_ / k;
    
//    if (std::abs(u_ + 13*L_*u0) >= 1.0e-4 && std::abs(v_ + 13*L_*v0) >= 1.0e-4 )
//    {
//        double a = 1.0/3.0 * (52.0*L_/(u_ + 13.0*L_*u0) - 1 );
//        double b = -5.0*Y_;
//        double c = -1.0/3.0;
//        double d = Y_*(39.0*L_/(v_ + 13.0*L_*v0) - 5.0);
        
//        X_ = (d - b)/(a - c);
//        Z_ = X_*a + b;
//    }
//    else
//        X_ = Z_ = 0.0;
//}

//void Color::XYZtoLuv()
//{
//    double up, vp;
//    up = u1();
//    vp = v1();

//    double upw = Ref_White_.u1();
//    double vpw = Ref_White_.v1();
    
////    if (space_ != SpaceRepresentation::Space_Lab)
////    {
////        if (Y_/Ref_White_.Y_ > 0.008856)
////            L_ = 116.0 * std::cbrt(Y_/Ref_White_.Y_) - 16.0;
////        else
////            L_ = 903.3*(Y_/Ref_White_.Y_);
////    }
    
//    u_ = 13.0*L_*(up - upw);
//    v_ = 13.0*L_*(vp - vpw);
//}

double Color::u() const
{
    double up = u1();
    double upw = Ref_White_.u1();
    return 13.0*L_*(up - upw);
}

double Color::v() const
{
    double vp = v1();
    double vpw = Ref_White_.v1();
    return 13.0*L_*(vp - vpw);
}

double Color::C() const
{
    return sqrt(a_*a_ + b_*b_);
}

double Color::h() const
{
    double res = atan2(b_,a_)*180./M_PI;

    while(res<0)
        res+=360;

    while(res>360)
        res-=360;

    return res;
}

double Color::callGetterByName(char getterCode) const
{
	switch(getterCode) {
		case 'x':
			return this->x();
		case 'y':
			return this->y();
		case 'Y':
			return this->Y();
	}
	return (double)nan("bad getterCode in CallGetterByName");
}

Color Color::addAmbient(double ambient)
{
    if (std::abs(y_) < 1.0e-5)
    {
        space_ = SpaceRepresentation::Space_xyY;
        x_ = D65.x();
        y_ = D65.y();
        Y_ += ambient;
        ReCalc();
        return *this;
    }

    return (*this = FromxyY(x_, y_, Y_ + ambient));
}

void Color::ReCalc()
{
    switch(space_)
    {
        case SpaceRepresentation::Space_XYZ:
            XYZtoLab();
            XYZtoxyY();
            //XYZtoLuv();
            break;
        case SpaceRepresentation::Space_xyY:
            xyYtoXYZ();
            XYZtoLab();
            //XYZtoLuv();
            break;
        case SpaceRepresentation::Space_Lab:
            LabtoXYZ();
            XYZtoxyY();
            //XYZtoLuv();
            break;
        case SpaceRepresentation::Space_Luv:
            //LuvtoXYZ();
            XYZtoLab();
            XYZtoxyY();
            break;
    }
}


void Color::InitReference()
{
    if (Ref_White_.space_==SpaceRepresentation::Space_None)
        Color::SetWhiteReference(Color::FromxyY(0.31272, 0.32903, 100)); //d65
}

Color::SpaceRepresentation Color::internalSpace()
{
    return space_;
}

Color Color::FromSpectro(std::vector<double> spectro, int startNm, int endNm, 
                             Observer observer/*=Observer::degree2*/,
                             Illuminant ill/*=Illuminant::D50*/)
{
    const int snm = 380, enm = 780;
    
    if (startNm==snm && endNm==enm)
        return FromSpectro(spectro);
    
    if (spectro.size()<3)
        return Color();
    
    int step = (endNm-startNm)/(spectro.size()-1);
    std::vector<double> s((enm-snm)/step +1 +.5, 0);
    std::copy(spectro.begin(), spectro.end(), s.begin()+(startNm-snm)/step);
    
    return FromSpectro(s, observer, ill);
}

const std::vector<double>& Color::spectralLambdaX(Observer observer)
{
    static const std::vector<double> x10{0.0002,0.0007,0.0024,0.0072,0.0191,0.0434,
                0.0847,0.1406,0.2045,0.2647,0.3147,0.3577,0.3837,0.3867,0.3707,
                0.3430,0.3023,0.2541,0.1956,0.1323,0.0805,0.0411,0.0162,0.0051,
                0.0038,0.0154,0.0375,0.0714,0.1177,0.1730,0.2365,0.3042,0.3768,
                0.4516,0.5298,0.6161,0.7052,0.7938,0.8787,0.9512,1.0142,1.0743,
                1.1185,1.1343,1.1240,1.0891,1.0305,0.9507,0.8563,0.7549,0.6475,
                0.5351,0.4316,0.3437,0.2683,0.2043,0.1526,0.1122,0.0813,0.0579,
                0.0409,0.0286,0.0199,0.0138,0.0096,0.0066,0.0046,0.0031,0.0022,
                0.0015,0.0010,0.0007,0.0005,0.0004,0.0003,0.0002,0.0001,0.0001,
                0.0001,0.0000,0.0000};
                
    static const std::vector<double> x2{0.0014, 0.0022, 0.0042, 0.0077, 0.0143,
                0.0232, 0.0435, 0.0776, 0.1344, 0.2148, 0.2839, 0.3285, 0.3483, 
                0.3481, 0.3362, 0.3187, 0.2908, 0.2511, 0.1954, 0.1421, 0.0956,
                0.0580, 0.0320, 0.0147, 0.0049, 0.0024, 0.0093, 0.0291, 0.0633, 
                0.1096, 0.1655, 0.2257, 0.2904, 0.3597, 0.4334, 0.5121, 0.5945, 
                0.6784, 0.7621, 0.8425, 0.9163, 0.9786, 1.0263, 1.0567, 1.0622, 
                1.0456, 1.0026, 0.9384, 0.8544, 0.7514, 0.6424, 0.5419, 0.4479, 
                0.3608, 0.2835, 0.2187, 0.1649, 0.1212, 0.0874, 0.0636, 0.0468, 
                0.0329, 0.0227, 0.0158, 0.0114, 0.0081, 0.0058, 0.0041, 0.0029, 
                0.0020, 0.0014, 0.0010, 0.0007, 0.0005, 0.0003, 0.0002, 0.0002, 
                0.0001, 0.0001, 0.0001, 0.0000};
    
    if (observer==Observer::degree10)
        return x10;
    
    return x2;    
}

const std::vector<double>& Color::spectralLambdaY(Observer observer)
{
    static const std::vector<double> y10{0,0.0001,0.0003,0.0008,0.002,0.0045,0.0088,
                0.0145,0.0214,0.0295,0.0387,0.0496,0.0621,0.0747,0.0895,0.1063,
                0.1282,0.1528,0.1852,0.2199,0.2536,0.2977,0.3391,0.3954,0.4608,
                0.5314,0.6067,0.6857,0.7618,0.8233,0.8752,0.9238,0.962,0.9822,
                0.9918,0.9991,0.9973,0.9824,0.9556,0.9152,0.8689,0.8256,0.7774,
                0.7204,0.6583,0.5939,0.528,0.4618,0.3981,0.3396,0.2835,0.2283,
                0.1798,0.1402,0.1076,0.0812,0.0603,0.0441,0.0318,0.0226,0.0159,
                0.0111,0.0077,0.0054,0.0037,0.0026,0.0018,0.0012,0.0008,0.0006,
                0.0004,0.0003,0.0002,0.0001,0.0001,0.0001,0,0,0,0,0};
                
    static const std::vector<double> y2{0,0.0001,0.0001,0.0002,0.0004,0.0006,
                0.0012,0.0022,0.004,0.0073,0.0116,0.0168,0.023,0.0298,0.038,
                0.048,0.06,0.0739,0.091,0.1126,0.139,0.1693,0.208,0.2586,0.323,
                0.4073,0.503,0.6082,0.71,0.7932,0.862,0.9149,0.954,0.9803,0.995,
                1,0.995,0.9786,0.952,0.9154,0.87,0.8163,0.757,0.6949,0.631,
                0.5668,0.503,0.4412,0.381,0.321,0.265,0.217,0.175,0.1382,0.107,
                0.0816,0.061,0.0446,0.032,0.0232,0.017,0.0119,0.0082,0.0057,
                0.0041,0.0029,0.0021,0.0015,0.001,0.0007,0.0005,0.0004,0.0002,
                0.0002,0.0001,0.0001,0.0001,0,0,0,0};
    
    if (observer==Observer::degree10)
        return y10;
    
    return y2;
}

const std::vector<double>& Color::spectralLambdaZ(Observer observer)
{
    static const std::vector<double> z10{0.0007,0.0029,0.0105,0.0323,0.0860,0.1971,
                0.3894,0.6568,0.9725,1.2825,1.5535,1.7985,1.9673,2.0273,1.9948,
                1.9007,1.7454,1.5549,1.3176,1.0302,0.7721,0.5701,0.4153,0.3024,
                0.2185,0.1592,0.1120,0.0822,0.0607,0.0431,0.0305,0.0206,0.0137,
                0.0079,0.0040,0.0011,0.0000, 0, 0, 0, 0, 
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0};
                
    static const std::vector<double> z2{0.0065,0.0105,0.0201,0.0362,0.0679,
                0.1102,0.2074,0.3713,0.6456,1.0391,1.3856,1.623,1.7471,1.7826,
                1.7721,1.7441,1.6692,1.5281,1.2876,1.0419,0.813,0.6162,0.4652,
                0.3533,0.272,0.2123,0.1582,0.1117,0.0782,0.0573,0.0422,0.0298,
                0.0203,0.0134,0.0087,0.0057,0.0039,0.0027,0.0021,0.0018,0.0017,
                0.0014,0.0011,0.001,0.0008,0.0006,0.0003,0.0002,0.0002,0.0001,0,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    if (observer==Observer::degree10)
        return z10;
    
    return z2;
}

const std::vector<double>& Color::spectralIlluminant(Illuminant ill)
{
    static const std::vector<double> A{9.795100,10.899600,12.085300,13.354300,
            14.708000,16.148000,17.675300,19.290700,20.995000,22.788300,
            24.670900,26.642500,28.702700,30.850800,33.085900,35.406800,
            37.812100,40.300200,42.869300,45.517400,48.242300,51.041800,
            53.913200,56.853900,59.861100,62.932000,66.063500,69.252500,
            72.495900,75.790300,79.132600,82.519300,85.947000,89.412400,
            92.912000,96.442300,100.000000,103.582000,107.184000,110.803000,
            114.436000,118.080000,121.731000,125.386000,129.043000,132.697000,
            136.346000,139.988000,143.618000,147.235000,150.836000,154.418000,
            157.979000,161.516000,165.028000,168.510000,171.963000,175.383000,
            178.769000,182.118000,185.429000,188.701000,191.931000,195.118000,
            198.261000,201.359000,204.409000,207.411000,210.365000,213.268000,
            216.120000,218.920000,221.667000,224.361000,227.000000,229.585000,
            232.115000,234.589000,237.008000,239.370000,241.675000};
            
            static const std::vector<double> B{22.40,26.85,31.30,36.18,41.30,
                    46.62,52.10,57.70,63.20,68.37,73.10,77.31,80.80,83.44,85.40,
                    86.88,88.30,90.08,92.00,93.75,95.20,96.23,96.50,95.71,94.20,
                    92.37,90.70,89.95,89.50,90.43,92.20,94.46,96.90,99.16,
                    101.00,102.20,102.80,102.92,102.60,101.90,101.00,100.07,
                    99.20,98.44,98.00,98.08,98.50,99.06,99.70,100.36,101.00,
                    101.56,102.20,103.05,103.90,104.59,105.00,105.08,104.90,
                    104.55,103.90,102.84,101.60,100.38,99.10,97.70,96.20,94.60,
                    92.90,91.10,89.40,88.00,86.90,85.90,85.20,84.80,84.70,84.90,
                    85.40,86.10,87.00};
            
        static const std::vector<double> C{33.00,39.92,47.40,55.17,63.30,71.81,
                80.60,89.53,98.10,105.80,112.40,117.75,121.50,123.45,124.00,
                123.60,123.10,123.30,123.80,124.09,123.90,122.92,120.70,116.90,
                112.10,106.98,102.30,98.81,96.90,96.78,98.00,99.94,102.10,
                103.95,105.20,105.67,105.30,104.11,102.30,100.15,97.80,95.43,
                93.20,91.22,89.70,88.83,88.40,88.19,88.10,88.06,88.00,87.86,
                87.80,87.99,88.20,88.20,87.90,87.22,86.30,85.30,84.00,82.21,
                80.20,78.24,76.30,74.36,72.40,70.40,68.30,66.30,64.40,62.80,
                61.50,60.20,59.20,58.50,58.10,58.00,58.20,58.50,59.10};
                
        static const std::vector<double> D50{24.49,27.18,29.87,39.59,49.31,
                52.91,56.51,58.27,60.03,58.93,57.82,66.32,74.82,81.04,87.25,
                88.93,90.61,90.99,91.37,93.24,95.11,93.54,91.96,93.84,95.72,
                96.17,96.61,96.87,97.13,99.61,102.10,101.43,100.75,101.54,
                102.32,101.16,100.00,98.87,97.74,98.33,98.92,96.21,93.50,95.59,
                97.69,98.48,99.27,99.16,99.04,97.38,95.72,97.29,98.86,97.26,
                95.67,96.93,98.19,100.60,103.00,101.07,99.13,93.26,87.38,89.49,
                91.60,92.25,92.89,84.87,76.85,81.68,86.51,89.55,92.58,85.40,
                78.23,67.96,57.69,70.31,82.92,80.60,78.27};
                
        static const std::vector<double> D55{32.58,35.34,38.09,49.52,60.95,
                64.75,68.55,70.07,71.58,69.75,67.91,76.76,85.61,91.80,97.99,
                99.23,100.46,100.19,99.91,101.33,102.74,100.41,98.08,99.38,
                100.68,100.69,100.70,100.34,99.99,102.10,104.21,103.16,102.10,
                102.53,102.97,101.48,100.00,98.61,97.22,97.48,97.75,94.59,91.43,
                92.93,94.42,94.78,95.14,94.68,94.22,92.33,90.45,91.39,92.33,
                90.59,88.85,89.59,90.32,92.13,93.95,91.95,89.96,84.82,79.68,
                81.26,82.84,83.84,84.84,77.54,70.24,74.77,79.30,82.15,84.99,
                78.44,71.88,62.34,52.79,64.36,75.93,73.87,71.82};
                
        static const std::vector<double> D65{49.98,52.31,54.65,68.70,82.75,
                87.12,91.49,92.46,93.43,90.06,86.68,95.77,104.86,110.94,117.01,
                117.41,117.81,116.34,114.86,115.39,115.92,112.37,108.81,109.08,
                109.35,108.58,107.80,106.30,104.79,106.24,107.69,106.05,104.41,
                104.23,104.05,102.02,100.00,98.17,96.33,96.06,95.79,92.24,88.69,
                89.35,90.01,89.80,89.60,88.65,87.70,85.49,83.29,83.49,83.70,
                81.86,80.03,80.12,80.21,81.25,82.28,80.28,78.28,74.00,69.72,
                70.67,71.61,72.98,74.35,67.98,61.60,65.74,69.89,72.49,75.09,
                69.34,63.59,55.01,46.42,56.61,66.81,65.09,63.38};
                
        static const std::vector<double> D75{66.65,68.28,69.91,85.89,101.87,
                106.85,111.83,112.28,112.74,107.89,103.04,112.09,121.15,127.05,
                132.96,132.63,132.31,129.80,127.28,127.02,126.76,122.26,117.75,
                117.16,116.57,115.12,113.68,111.16,108.65,109.54,110.44,108.36,
                106.28,105.59,104.90,102.45,100.00,97.81,95.62,94.92,94.22,
                90.61,87.00,87.12,87.24,86.69,86.15,84.87,83.59,81.18,78.76,
                78.60,78.44,76.63,74.82,74.58,74.34,74.89,75.44,73.52,71.60,
                67.73,63.87,64.48,65.10,66.59,68.09,62.27,56.46,60.36,64.26,
                66.71,69.17,63.91,58.64,50.64,42.63,52.00,61.37,59.85,58.34};
                
        static const std::vector<double> F2{1.18,1.48,1.84,2.15,3.44,15.69,3.85,
                3.74,4.19,4.62,5.06,34.98,11.81,6.27,6.63,6.93,7.19,7.40,7.54,
                7.62,7.65,7.62,7.62,7.45,7.28,7.15,7.05,7.04,7.16,7.47,8.04,
                8.88,10.01,24.88,16.64,14.59,16.16,17.56,18.62,21.47,22.79,
                19.29,18.66,17.73,16.54,15.21,13.80,12.36,10.95,9.65,8.40,7.32,
                6.31,5.43,4.68,4.02,3.45,2.96,2.55,2.19,1.89,1.64,1.53,1.27,
                1.10,0.99,0.88,0.76,0.68,0.61,0.56,0.54,0.51,0.47,0.47,0.43,
                0.46,0.47,0.40,0.33,0.27};
        
        static const std::vector<double> F7{2.56,3.18,3.84,4.53,6.15,19.37,7.37,
                7.05,7.71,8.41,9.15,44.14,17.52,11.35,12.00,12.58,13.08,13.45,
                13.71,13.88,13.95,13.93,13.82,13.64,13.43,13.25,13.08,12.93,
                12.78,12.60,12.44,12.33,12.26,29.52,17.05,12.44,12.58,12.72,
                12.83,15.46,16.75,12.83,12.67,12.45,12.19,11.89,11.60,11.35,
                11.12,10.95,10.76,10.42,10.11,10.04,10.02,10.11,9.87,8.65,7.27,
                6.44,5.83,5.41,5.04,4.57,4.12,3.77,3.46,3.08,2.73,2.47,2.25,
                2.06,1.90,1.75,1.62,1.54,1.45,1.32,1.17,0.99,0.81};
        
        static const std::vector<double> F11{0.91,0.63,0.46,0.37,1.29,12.68,
                1.59,1.79,2.46,3.33,4.49,33.94,12.13,6.95,7.19,7.12,6.72,6.13,
                5.46,4.79,5.66,14.29,14.96,8.97,4.72,2.33,1.47,1.10,0.89,0.83,
                1.18,4.90,39.59,72.84,32.61,7.52,2.83,1.96,1.67,4.43,11.28,
                14.76,12.73,9.74,7.33,9.72,55.27,42.58,13.18,13.16,12.26,5.11,
                2.07,2.34,3.58,3.01,2.48,2.14,1.54,1.33,1.46,1.94,2.00,1.20,
                1.35,4.10,5.58,2.51,0.57,0.27,0.23,0.21,0.24,0.24,0.20,0.24,
                0.32,0.26,0.16,0.12,0.09};
                
    switch(ill)
    {
        case Illuminant::A:
            return A;
        case Illuminant::B:
            return B;
        case Illuminant::C:
            return C;
        case Illuminant::D65:
            return D65;
        case Illuminant::D75:
            return D75;
        case Illuminant::F2:
            return F2;
        case Illuminant::F7:
            return F7;
        case Illuminant::F11:
            return F11;
            
        case Illuminant::D50:
        default:
            return D50;
    }
}

Color Color::FromSpectro(std::vector<double> spectro, 
                             Observer observer/*=Observer::degree2*/,
                             Illuminant illuminant/*=Illuminant::D50*/)
{
    if (spectro.size()<3)
        return Color();
    
    const double step = 5;    
    const double b = 380, e = 780;
    const int sz = ((e-b)/step + 1)+.5;
    std::vector<double> newspectro(spectro);
    
    if (spectro.size()!=(e-b)/step +1)
    {
        std::vector<double> bx(spectro.size()), ax((e-b)/step +1);
        for (unsigned i=0;i<spectro.size();++i)
            bx[i] = b+(e-b)*i/(spectro.size()-1);
        for (unsigned v=b, i=0;v<=e;v+=step,++i)
            ax[i] = v;


        if (!Interpolation::SplineHermite(bx, spectro, ax, newspectro))
            return Color();
    }
    
    const std::vector<double>& x = spectralLambdaX(observer);
    const std::vector<double>& y = spectralLambdaY(observer);
    const std::vector<double>& z = spectralLambdaZ(observer);
    const std::vector<double>& ill = spectralIlluminant(illuminant);
          
    long double X=0, Y=0, Z=0, K=0;
    
//    //trapezoidal 
//    unsigned shift = (xb-b)/step + .5;
//    for (unsigned i=0;i<x10.size()-1;++i)
//        X += (newspectro[i+shift]*x10[i]+newspectro[i+shift+1]*x10[i+1])/2;
//    
//    shift = (yb-b)/step + .5;
//    for (unsigned i=0;i<y10.size()-1;++i)
//        Y += (newspectro[i+shift]*y10[i]+newspectro[i+shift+1]*y10[i+1])/2;
//    
//    shift = (zb-b)/step + .5;
//    for (unsigned i=0;i<z10.size()-1;++i)
//        Z += (newspectro[i+shift]*z10[i]+newspectro[i+shift+1]*z10[i+1])/2;
//    
//    return Color::FromXYZ(X*step/683, Y*step/683, Z*step/683);
    
    //parabolic
    unsigned n = sz;
    unsigned n2 = (n-1)%2 ? n-1 : n-2;
    X = newspectro[0]*x[0]*ill[0] + newspectro[n2]*x[n2]*ill[n2];
    for (unsigned i=1;i<n2;i+=2)
        X += 4*newspectro[i]*x[i]*ill[i];
    for (unsigned i=2;i<n2;i+=2)
        X += 2*newspectro[i]*x[i]*ill[i];
    X *= (e-b-step*(n-1-n2))/3/n2;
    if ((n-1)%2==0)
        X += step/2*(newspectro[n-2]*x[n-2]*ill[n-2]+newspectro[n-1]*x[n-1]*ill[n-1]);
    
    long double t;
    Y = newspectro[0]*y[0]*ill[0] + newspectro[n2]*y[n2]*ill[n2];
    K = y[0]*ill[0] + y[n2]*ill[n2];
    for (unsigned i=1;i<n2;i+=2)
    {
        t = 4*y[i]*ill[i];
        K += t;
        Y += newspectro[i]*t;
    }
    for (unsigned i=2;i<n2;i+=2)
    {
        t = 2*y[i]*ill[i];
        K += t;
        Y += newspectro[i]*t;
    }
    Y *= (e-b-step*(n-1-n2))/3/n2;
    K *= (e-b-step*(n-1-n2))/3/n2;
    if ((n-1)%2==0)
    {
        Y += step/2*(newspectro[n-2]*y[n-2]*ill[n-2]+newspectro[n-1]*y[n-1]*ill[n-1]);
        K += step/2*(y[n-2]*ill[n-2]+y[n-1]*ill[n-1]);
    }
    K = 100/K;
    
    Z = newspectro[0]*z[0]*ill[0] + newspectro[n2]*z[n2]*ill[n2];
    for (unsigned i=1;i<n2;i+=2)
        Z += 4*newspectro[i]*z[i]*ill[i];
    for (unsigned i=2;i<n2;i+=2)
        Z += 2*newspectro[i]*z[i]*ill[i];
    Z *= (e-b-step*(n-1-n2))/3/n2;
    if ((n-1)%2==0)
        Z += step/2*(newspectro[n-2]*z[n-2]*ill[n-2]+newspectro[n-1]*z[n-1]*ill[n-1]);
    
    return Color::FromXYZ(X*K, Y*K, Z*K);
}

void Color::fromXYZtoRGBf(const double &x, const double &y, const double &z, double &rf, double &gf, double &bf)
{
    rf = (x*3.1338561)/Ref_White_.Y()    + (y*(-1.6168667))/Ref_White_.Y() + (z*(-0.4906146))/Ref_White_.Y();
    gf = (x*(-0.9787684))/Ref_White_.Y() + (y*(1.9161415))/Ref_White_.Y()  + (z*(0.0334540))/Ref_White_.Y();
    bf = (x*0.0719453)/Ref_White_.Y()    + (y*(-0.2289914))/Ref_White_.Y() + (z*(1.4052427))/Ref_White_.Y();

    if (rf>1.) rf = 1.;
    if (rf<0.) rf = 0.;

    if (gf>1.) gf = 1.;
    if (gf<0.) gf = 0.;

    if (bf>1.) bf = 1.;
    if (bf<0.) bf = 0.;

    rf = std::pow(rf, 1/2.2);
    gf = std::pow(gf, 1/2.2);
    bf = std::pow(bf, 1/2.2);

    //qDebug()<<rf<<gf<<bf;
}
