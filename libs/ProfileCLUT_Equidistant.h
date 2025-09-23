/* 
 * File:   ProfileCLUT_Equidistant.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 28 липня 2011, 16:37
 */

#ifndef PROFILECLUT_EQUIDISTANT_H
#define	PROFILECLUT_EQUIDISTANT_H

#include <vector>
#include <stddef.h>

#include "Optimization.h"
#include <cstddef>

class ProfileCLUT_Equidistant 
{
public:
    enum FirstApproximationType {InputAsTarget, MinimalDistance, HookeJeevesMethod, GCRandHookeJeevesMethod};
    enum class InvertedCLUTType {UNKNOWN_CLUT, DISPLAY_CLUT, RGB_PRINTER_CLUT, CMYK_PRINTER_CLUT};


    /**
     * Helping class for managing output of CLUT
     */
    class Values
    {
        int sz_;
        int index_;
        int grid_;
        
        void addValue(long double value);
        void subValue();
    public:
        Values(int sz);
        Values(const std::vector<long double> &values);
        
        int size() const {return sz_;}
        long double &operator[](int index) {return values_[index];}
        long double /*const&*/ operator[](int index) const{return values_[index];}
        std::vector<long double> values_;
        
        int index(int grid);
        std::vector<long double> distances(const Values &other);
        long double distance(const Values& other);
        
        std::vector<long double> distances(const Values &other, const std::vector<double>& weights);
        void distances(const Values &other, const std::vector<double>& weights, std::vector<long double>& res);
        long double distance(const Values& other, const std::vector<double>& weights);
        
        Values operator+(const Values& other);
        Values operator*(const Values& other);
        Values operator-(const Values& other);
        Values operator/(const Values& other);
        Values operator*(const double& coef);
        bool operator<(const Values& other);
        
        friend class ProfileCLUT_Equidistant;
    };
    
private:
    int n_;
    int An_, Bn_;
    std::vector<Values> lut_;
    
    
    /**
     * Variable used as reference during optimization, so should be set to the 
     * target value before optimization
     */
    Values target_;
    
    Values target_mult_;
    
    /**
     * Weights for calculation distance between output space points
     */
    std::vector<double> outputWeights_;
    
    /**
     * special mode for inverting clut (currently with GCR - gray component replace)
     */
    bool isSpecialBlack_;
    /**
     * special black mode - black ink position
     */
    int sk_blackPos_;
    /**
     * special black - black width squared
     */
    double sk_maxC2_;
    /**
     * special black - total ink count
     */
    double sk_TIC_;
    /**
     * special black - start point of black curve
     */
    double sk_startK_;
    /**
     * special black - maximum black value in black curve
     */
    double sk_maxK_;
    /**
     * special black - gamma of black curve
     */
    double sk_gamma_;

    long double GCR_K_;
    long double weightK_;

    long double Lmin_inv_;
    
    /**
     * helper method for iterating target_ and inputs. Set all zeros
     * @param v values for iterating
     * @param v_multiplied  temporary values for iterating in integral space (for better accuracy)
     */
    void zeroValues(Values& v, Values& v_multiplied);
    
    /**
     * helper method for iterating target_ and inputs. Increase values to next one
     * @param gridCount count of points on grid of new LUT
     * @param v values for iterating
     * @param v_multiplied  temporary values for iterating in integral space (for better accuracy)
     * @return is new value exists
     */
    bool incValues(int gridCount, Values& v, Values& v_multiplied);
    
    /**
     * Overwrited function for optimization when search reverse CLUT
     * @param point approximation of input Triple
     * @param target output target point
     * @param res distances by axises
     * @return is successfull
     */
    bool ErrValues(const std::vector<long double> &point, const Values &target, std::vector<long double> &res);
    
    /**
     * Calculate index in plain lut_ vector by point
     * @param point multi-dimension point
     * @return plain index
     */
    int index(Values &point);
    
    Values subInterpolate(Values &fixed, Values &search);

    void setMinLForK(double Lmin);
    
    long double targetK(long double L);
    bool calcSpecialBlack(const std::vector<long double>& point, long double& resK, long double& Kweight);

    template<bool isSpecialBlack, bool useUV>
    void findClosestPointsForInvertion(InvertedCLUTType clutType, 
        const std::vector<Values> &targets,
        std::vector<std::vector<long double>> &startpoints,
        std::vector<bool> &inGamut,
        std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> errorFuncOutput,
        std::function<long double(const std::vector<long double> &)> errorFunc );
    
    std::vector<long double> findRoughApproximationForInvertion(FirstApproximationType approximationType,
        const std::vector<long double> &oldStartpoint,
        std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> errorFuncOutput,
        std::function<long double(const std::vector<long double> &)> errorFunc);

    std::vector<long double> findResultApproximationForInvertion(const std::vector<long double> &startpoint, double error,
        std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> sumErrorFunc,
        std::function<double(const std::vector<long double>&)> errorGCRFunc);

public:
    /**
     * Create equidistand CLUT NxNxN 
     * for transforming A_count dimension space to the
     * B_count dimension space
     * @param N count of measurements per axis
     */
    ProfileCLUT_Equidistant(int N, int A_count, int B_count);
    virtual ~ProfileCLUT_Equidistant();
    
    
    int gridCount();
    int inChannels();
    int outChannels();
    
    
    /**
     * Fill lut by grid points
     * @param point all axises [0-N)
     * @param values value to put in CLUT
     */
    void setGridValue(Values point, const Values &values);
    
    /**
     * Fill lut by grid points
     * @param point all axises [0-N)
     * @param values value to put in CLUT
     */
    void setGridValue(const std::vector<long double> &point, const Values &values);
    
    
    /**
     * Return interpolated value for exact place using grid values
     * @param x point all axises [0-1]
     * @return interpolated value
     */
    Values value(const Values &point);
    /**
     * Return interpolated value for exact place using grid values
     * @param x point all axises [0-1]
     * @return interpolated value
     */
    Values value(const std::vector<long double> &point);
    
    /**
     * Creates inverted CLUT to the current.
     * Current realization is Newton optimization search
     * @param gridCount inverted CLUT will be filled with this count of values on each grid
     * @param error accepted error of interpolation
     * @param useInputAsTarget set it to true if you expect CLUT be close to linear
     * @param startPointGrid how big grid point to use for start point. default -1 mean same as internal grid (warning: -1 or big number can slow process much)
     * @param progress_fn callback function to describe progress
     * @return inverted CLUT
     */
    ProfileCLUT_Equidistant inverted(int gridCount, double error, InvertedCLUTType clutType, FirstApproximationType approximationType=MinimalDistance, int startPointGrid=-1, void (*progress_fn)()=nullptr);
    
    std::vector<Values> lut();
    bool setLut(std::vector<Values> lut);
      
    /**
     * Set weights for calculating distance between points.
     * Note: eights applied BEFORE squaring axis distances. So D = (w1*(x-x'))^2 + (w2*(y-y'))^2
     * @param outputWeights weights in euclidian distance. Size should be same as OUTPUT dimention of CLUT
     */
    void setDistanceWeights(const std::vector<double>& outputWeights);
    
    
    /**
     * Activate or deactivate special inverting mode with black calculations
     * @param blackmode true if you want to activate special mode
     */
    void setSpecialBlackMode(bool blackmode=true);
    
    /**
     * Setting of special inverting mode (black mode) parameters
     * @param sk_blackPos position of black ink, e.g. in CMYK K position is 3 (zero based)
     * @param sk_maxC black width in range 0-1
     * @param sk_TIC start point of black curve
     * @param sk_startK start point of black curve
     * @param sk_maxK maximum black value in black curve
     * @param sk_gamma gamma of black curve
     */
    void setSpecialBlackModeParams(int sk_blackPos, double sk_maxC, 
                                   double sk_TIC, double sk_startK,
                                   double sk_maxK, double sk_gamma);
};

#endif	/* PROFILECLUT_EQUIDISTANT_H */

