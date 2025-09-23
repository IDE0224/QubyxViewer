#ifndef QUBYXPROFILE_H
#define	QUBYXPROFILE_H

#include <QString>
#include "Color.h"
#include "Matrix.h"
#include "ProfileCLUT_Equidistant.h"
#include "ICCProfLib/IccProfile.h"
#include "ICCProfLib/IccCmm.h"
#include "ICCProfLib/IccEval.h"
#include "CAT.h"

#include <vector>
#include <memory>

#include <QDebug>

class CIccMinMaxEval : public CIccEvalCompare
{
public:
  CIccMinMaxEval();

  void Compare(icFloatNumber *pixel, icFloatNumber *deviceLab, icFloatNumber *lab1, icFloatNumber *lab2);

  icFloatNumber GetMean1() { return sum1 / num1; }
  icFloatNumber GetMean2() { return sum2 / num2; }

  icFloatNumber minDE1, minDE2;
  icFloatNumber maxDE1, maxDE2;

  icFloatNumber maxLab1[3], maxLab2[3];
  icFloatNumber maxPixel1[3], maxPixel2[3];

protected:

  icFloatNumber sum1, sum2;
  icFloatNumber num1, num2;
};

class QubyxProfile
{
public:   
    typedef std::vector<long double> Curve;

    struct ParamCurve
    {
        int type;
        std::vector<double> params;
    };

    struct Curves
    {
        Curves() : parametric(false) {}

        std::vector<bool> parametric;

        std::vector<ParamCurve> paramCurves;
        std::vector<Curve> curves;
    };
    
    enum ChromaType
    {
        ChromaBradford,
        ChromaCAT02,
        ChromaVonKries
    };
    
    
    enum class ClutElement
    {
        curveA, 
        curveB,
        curveM,
        Matrix,
        Lut
    };

    enum class ICCSpec {
        ICCv4,
        ICCv2
    };
    
private:
    int devDim_;
    std::unique_ptr<CIccCmm> dev2lab_, dev2xyz_, lab2dev_, xyz2dev_;
    Color whiteXYZ_;
    
    long double convertChroma_[9];
    long double convertRevChroma_[9];
    icColorSpaceSignature inColorSpace_, outColorSpace_;
    
    double maxY_;
    
    QString optDescription_;
    bool skipDescrType_;
    ICCSpec spec_;
    
protected:    
    QString filename_;
        
    void addPointTag(icFloatNumber *point, icSignature tag, CAT* cat=nullptr);
    void addPointTag(icInt32Number *point, icSignature tag);
    bool readPointTag(icFloatNumber *point, icSignature tag);
    bool readPointTag(icInt32Number *point, icSignature tag);
    
        

    void normalizeColor(icFloatNumber *normalized, Color &color, Color &refcolor, Color *black=nullptr);
    void perceptualTransform(icFloatNumber *point, icFloatNumber *white, icFloatNumber *black);

    
    void addTRCtag(icSignature tagname, std::vector<double> &x, std::vector<double> &values);
    void addTRCtag(icSignature tagname, std::vector<double> &values);
    
    void fillCurveTag(CIccTagCurve* curve, Curve &values);
    void fillCurveTagLinear(CIccTagCurve* curve);

    void fillParamCurveTag(CIccTagParametricCurve* curve, const ParamCurve &values);
    void fillParamCurveTagLinear(CIccTagParametricCurve* curve);
    
    template<class Tag>
    void AddFull3dLUTTag(icTagSignature signature, Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &matr, ProfileCLUT_Equidistant &lut);
    
    
//    template<class Tag>
//    ProfileCLUT_Equidistant get3dLUTTag(icTagSignature signature, bool applyChromatic, bool applyLuminance);      
    
    bool savable_;
    CIccProfile profile_;
    QString deviceName_;
    
    
    virtual void fillProfileHeader();

    void setColorSpaces(icColorSpaceSignature inColorSpace, icColorSpaceSignature outColorSpace);
    
    /**
     * Create profile folder for MacOS + set permissions.
     * Called in constructor. So no need to call it by hands
     */
    void InitProfileCatalog();

    /**
     * @brief makeProfileTitle generate string for profile name shown in Photoshop for example
     * @param deviceName name of display/camera/printer
     * @param type some common name for printer type. e.g. "Qubyx printer profile"
     * @return title with the date, type and devicename (see @setDeviceName)
     */
    QString makeProfileTitle(QString type);
    
public:
    QubyxProfile();
    QubyxProfile(QString profilePath);
    QubyxProfile(const QubyxProfile& other);
    
    virtual ~QubyxProfile();

    QubyxProfile& operator=(const QubyxProfile& other);
    
    void setFileName(QString filename);

    /**
     * Calculate 3x3 matrix for chromatic adaptation. This matrix transfer src to PCS
     * @param PCS pointer to PCS white (usually D50)
     * @param src pointer to src white (real measured and normalized)
     * @param resMatrix pointer to 9 element buffer. Result matrix will be saved
     * @param type type of calculations
     */
    static void calcChromaticAdaptation(icFloatNumber* PCS, icFloatNumber* src, icFloatNumber* resMatrix, ChromaType type=ChromaBradford);

    bool validate();
    
    /**
     * Check by header is current profile qubyx profile
     * @return is header looks like qubyx profile
     */
    bool isQubyxProfile();
        
    /**
     * Fetch main colors from the profile (if not found sRGB values returned). Colors is transformed back with chromatic adaptation if it is exists
     * @param black
     * @param white
     * @param red
     * @param green
     * @param blue
     */
    void readProfileColors(Color &black, Color &white, Color &red, Color &green, Color &blue, QubyxProfile::ChromaType type =ChromaBradford);
    
    virtual bool applyChromaticAdaptationToAll(std::vector<std::vector<std::vector<Color> > > &meas);
    virtual bool applyPerceptualAdaptationToAll(std::vector<std::vector<std::vector<Color> > > &meas, Color black);
    
    void AddAToBTag(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature AToBTag=icSigAToB0Tag);
    void AddAToBTagPrinter(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature AToBTag=icSigAToB0Tag);
    void AddBToATag(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature BToATag=icSigBToA0Tag);
    void AddBToATagPriner(Curves &aCurve, Curves &mCurve, Curves &bCurve, Matrix &m, ProfileCLUT_Equidistant &clut, icTagSignature BToATag=icSigBToA0Tag);
    void addTRCtagDirect(icSignature tagname,const std::vector<long double> &values);
    
    bool haveTag(icSignature tagName);
    bool haveTagInCLUT(icSignature clutName, ClutElement element);
    
//    ProfileCLUT_Equidistant getAToBTag(icTagSignature signature, bool applyChromatic = false, bool applyLuminance = false);
//    ProfileCLUT_Equidistant getBToATag(icTagSignature signature);
    

    bool SaveToFile();
    bool SaveToFileWithTry(int trycount = 10);
    bool LoadFromFile();
    bool LoadFromMemory(unsigned char *buf, size_t size);
    bool SaveToMemory(unsigned char* &buf, size_t &size);
    
    bool roundTripData(CIccMinMaxEval &eval);
    QString roundTrip();
        
    /**
     * Calculate dimention of device space, eg. for RGB=3, for CMYK=4
     * @return dimention of device color space
     */
    int deviceColorDimention();
        
    /**
     * Function for checking what is exact file used for load/save.
     * @return full path to the profile
     */
    QString profilePath();
    
    /**
     * Used for denying saving of profile to the  disk and applying.
     * @param deny true - to deny, false - to return to normal state
     */
    void denySaving(bool deny=true);

    void setDeviceName(QString deviceName);
    
    bool addGammutTag(int gridCount);
    
    void setLuminance(double lum);
    bool getLuminance(double &resLum) const;

    /**
     * Get list of all measurements that used for building profile
     * @return list of measurements in CGTA format
     */
    QString getMeasurementList() const;

    /**
     * @brief Save list of measurements that used for building profile
     * @param measData - list in CGTA format
     * @return
     */
    bool setMeasurementList(QString measData);
    
    Matrix getChromaticAdaptation() const;
    std::vector<double> getChromaticAdaptationAsVector() const;
    std::vector<double> getRevertChromaticAdaptationAsVector() const;
    bool setChromaticAdaptation(Matrix chad);

    static bool applyChromaticAdaptation(long double m[9], Color &meas);
    template <class T>
    static bool applyChromaticAdaptation(long double m[9], T XYZ[3]);
    template <typename T>
    static bool applyChromaticAdaptation(const std::vector<double> &m, T XYZ[3]);
    template <typename T>
    static bool applyChromaticAdaptation(const std::vector<double> &m, std::vector<T> &XYZ);
    static bool applyChromaticAdaptation(const std::vector<double> &m, Color &meas);
    
    template<typename T> bool applyChromaticAdaptationToAll(std::map<T, Color> &meas)
    {
        CIccTagS15Fixed16* chroma = dynamic_cast<CIccTagS15Fixed16*>(profile_.FindTag(icSigChromaticAdaptationTag));
        if (!chroma || chroma->GetSize()<9)
            return false;


        long double m[9];
        for (int i=0;i<9;i++)
            m[i] = icFtoD((*chroma)[i]);

        std::map<T, Color> mcopy = meas;
        for (auto itr : mcopy)
        {
            Color c = itr.second;
            //qDebug() << "applyChromaticAdaptationToAll gray. before:" << (double)c.X() << (double)c.Y() << (double)c.Z();
            applyChromaticAdaptation(m, c);
            meas[itr.first] = c;
            //qDebug() << "applyChromaticAdaptationToAll gray. after:" << /*itr.first << "->" <<*/ (double)c.X() << (double)c.Y() << (double)c.Z();
        }

        return true;
    }
    
    template<typename T> bool applyPerceptualAdaptationToAll(std::map<T, Color> &meas, Color black)
    {
        double blackPCS[3];
        if (!meas.empty())
        {
            blackPCS[0] = black.X();
            blackPCS[1] = black.Y();
            blackPCS[2] = black.Z();
        }


        std::map<T, Color> mcopy = meas;
        for (auto itr : mcopy)
        {
            Color c = itr.second;
            //qDebug() << "applyPerceptualAdaptationToAll. before:" << (double)c.X() << (double)c.Y() << (double)c.Z();
            applyPerceptualAdaptation(blackPCS, c);
            //qDebug() << "applyPerceptualAdaptationToAll. after:" << (double)c.X() << (double)c.Y() << (double)c.Z();
            meas[itr.first] = c;
        }


        return true;
    }

    bool applyPerceptualAdaptation(double blackPCS[3], Color &meas);
    
    static Matrix calcChromaticAdaptation(Color src, ChromaType type=ChromaBradford);
    static QString getDefaultProfilePath();
    
    void setOptionalDescription(QString text, bool useType=true);

    void setICCspecification(ICCSpec spec);
    ICCSpec ICCspecification();
    QString ICCspecificationAsString();
    int ICCspecificationAsInt();
    void setTextTag(icSignature sig, QString text);
    QString getTextTag(icSignature sig) const;

    bool isLabProfile();

    CIccTag* getIccTag(icTagSignature signature);

    friend class QubyxProfileChain;
};



#endif	/* QUBYXPROFILE_H */

