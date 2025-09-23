#ifndef QUBYX_MATH_COLOR_H
#define	QUBYX_MATH_COLOR_H

#include <vector>

class Color
{
public:
    
    enum class Observer
    {
        degree2,
        degree10
    };
    
    enum class Illuminant
    {
        A, B, C, D50, D55, D65, D75, F2, F7, F11
    };

    enum class SpaceRepresentation
    {
        Space_None = -1,
        Space_XYZ,
        Space_xyY,
        Space_Lab,
        Space_Luv
    };

    class TempWhiteReference;
private:    
    
    SpaceRepresentation space_;

    long double X_, Y_, Z_;
    long double x_, y_;
    long double L_, a_, b_;
//    long double u_, v_;

    static Color Ref_White_;

    void XYZtoxyY();
    void xyYtoXYZ();
    void XYZtoLab();
    void LabtoXYZ();
//    void LuvtoXYZ();
//    void XYZtoLuv();

    static const std::vector<double>& spectralLambdaX(Observer observer);
    static const std::vector<double>& spectralLambdaY(Observer observer);
    static const std::vector<double>& spectralLambdaZ(Observer observer);
    
    static const std::vector<double>& spectralIlluminant(Illuminant ill);
    
public:

    Color();
    virtual ~Color();
    void ReCalc();

    static Color FromXYZ(double X, double Y, double Z);
    static Color FromxyY(double x, double y, double Y);
    static Color FromLab(double L, double a, double b);
//    static Color FromLuv(double L, double u, double v);
    static void fromXYZtoRGBf(const double &x, const double &y, const double &z, double &rf, double &gf, double &bf);

    bool operator==(const Color& other) const;
    
    /**
     * Convert spectral vector to the XYZ and return it as Color
     * @param spectro vector of spectral measurements from 380 to 780
     * @return calculated color
     */
    static Color FromSpectro(std::vector<double> spectro, 
                             Observer observer=Observer::degree2,
                             Illuminant ill=Illuminant::D50);
    
    /**
     * Convert spectral vector to the XYZ and return it as Color
     * @param spectro vector of spectral measurements from startNm to endNm
     * @param startNm first measurement frequency
     * @param endNm last measurement frequency
     * @return calculated color
     */
    static Color FromSpectro(std::vector<double> spectro, int startNm, int endNm, 
                             Observer observer=Observer::degree2,
                             Illuminant ill=Illuminant::D50);

    static void SetWhiteReference(const Color& white);
	static Color GetWhiteReference();

    inline double X() const { return X_;}
    inline double Y() const { return Y_;}
    inline double Z() const { return Z_;}
    inline double x() const { return x_;}
    inline double y() const { return y_;}
    inline double L() const { return L_;}
    inline double a() const { return a_;}
    inline double b() const { return b_;}
//    inline double u() const { return u_;}
//    inline double v() const { return v_;}
    double u() const;
    double v() const;
    double C() const;
    double h() const;

	double callGetterByName(char getterCode) const;

    Color addAmbient(double ambient);

    //u'v'
    double u1() const;
    double v1() const;

    static void InitReference();

    SpaceRepresentation internalSpace();
    
    static const Color D50;
    static const Color D65;


};

class Color::TempWhiteReference
{
    Color old_;
public:
    TempWhiteReference(Color c)
    {
        old_ = Color::GetWhiteReference();
        Color::SetWhiteReference(c);
    }

    ~TempWhiteReference()
    {
        Color::SetWhiteReference(old_);
    }
};

#endif	/* COLOR_H */

