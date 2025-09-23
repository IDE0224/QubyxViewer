/* 
 * File:   Regression.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 29 листопада 2010, 12:51
 */

#ifndef REGRESSION_H
#define	REGRESSION_H


#include <vector>
#include "Matrix.h"

template <class XType, class YType> class Regression
{
   Regression();
   
   static void ExpandXs(const std::vector<XType> &x, unsigned maxpower, std::vector<XType> &resX);
   friend class MultiRegressiontestclass;
   
   static void calculateNormalization(Matrix &A, std::vector<long double>& normalizecoefs, int ignoredcolumn=-1);
   static void applyNormalization(Matrix &A, std::vector<long double>& normalizecoefs, int ignoredcolumn=-1);
   static void applyNormalization(std::vector<XType> &expX, std::vector<long double>& normalizecoefs, int ignoredcolumn=-1);
public:
    virtual ~Regression();

    //y = kx + b
    static bool LinearRegression(const std::vector<XType>& xs, const std::vector<YType>& ys, long double& resk, long double& resb);

    //y = ax^2 + bx + c
    static bool QuadraticRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                                    long double& resa, long double& resb, long double& resc);

    //y = res[0]x^n + ... + res[n-1]x + res[n]
    static bool NPowerRegression(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                                 std::vector<long double>& res);
    
    //y = res[0]x^n + ... + res[n-1]x + res[n]
    static bool NPowerRegressionNormalized(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                                           std::vector<long double>& res, std::vector<XType>& normalizecoefs,
                                           long double lambda=0);

    //y = kx + b
    static bool WeightedLinearRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                                         const std::vector<long double>& w,
                                         long double& resk, long double& resb);

    //y = ax^2 + bx + c
    static bool WeightedQuadraticRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                                            const std::vector<long double>& w,
                                            long double& resa, long double& resb, long double& resc);

    //y = res[0]x^n + ... + res[n-1]x + res[n]
    static bool WeightedNPowerRegression(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                                         const std::vector<long double>& w,
                                         std::vector<long double>& res);

    static YType ApplyRegression(XType x, std::vector<long double>& coefs);
    static YType ApplyRegressionNormalized(XType x, std::vector<long double>& coefs, std::vector<XType>& normalizecoefs);
    
    
    
    static bool MultiRegression(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                const std::vector<YType> &ys,
                                std::vector<long double> &resCoefs,
                                double lambda=0);
    
     static bool MultiRegressionWeighted(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                        const std::vector<YType> &ys,
                                        const std::vector<YType> &weights,
                                        std::vector<long double> &resCoefs,
                                        double lambda=0);
    
    static YType ApplyMultiRegression(const std::vector<XType> &x, unsigned maxpower,
                                const std::vector<long double> &coefs);
    
    static bool MultiRegressionNormalized(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                const std::vector<YType> &ys,
                                std::vector<long double> &resCoefs, 
                                std::vector<long double>& normalizecoefs,
                                double lambda=0);
    
    static YType ApplyMultiRegressionNormalized(const std::vector<XType> &x, unsigned maxpower,
                                const std::vector<long double> &coefs, 
                                std::vector<long double>& normalizecoefs);
    
    
    static bool ConvertRegressionToMultiRegression(unsigned maxpower, int channels, int channelIdx, 
                                const std::vector<long double> &coefs, std::vector<long double> &multyCoefs);
};

#endif	/* REGRESSION_H */

