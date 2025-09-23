/* 
 * File:   ProfileCLUT_Equidistant.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 28 липня 2011, 16:37
 */

#include "ProfileCLUT_Equidistant.h"

#include <cmath>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <limits>

#include <QDebug>
#include <QString>
#include <numeric>

#include "cppfunc.h"
#include "Color.h"
#include "ColorDistance.h"

// Boost dependencies removed for open-source compatibility
// Using simple linear search instead of R-tree for spatial indexing

#include <type_traits>

using namespace cppfunc;

ProfileCLUT_Equidistant::Values::Values(int sz) : sz_(sz), 
                                                  values_(std::vector<long double>(sz, 0)),
                                                  index_(-1),
                                                  grid_(0)
{
    
}

ProfileCLUT_Equidistant::Values::Values(const std::vector<long double> &values)
{
    sz_ = values.size();
    values_ = values;
    index_ = -1;
    grid_ = 0;
}

std::vector<long double> ProfileCLUT_Equidistant::Values::distances(const Values &other)
{
    if (other.sz_!=sz_)
    {
        qDebug() << "Distances error. sizes different-" << sz_ << other.sz_;
        return std::vector<long double>(sz_, -1);
    }
    
    std::vector<long double> res(sz_, 0);
    for (int i=0;i<sz_;i++)
    {
        //res[i] = powl(values_[i]-other.values_[i], 2);
        res[i] = values_[i]-other.values_[i];
    }
    
    return res;
}

long double ProfileCLUT_Equidistant::Values::distance(const Values& other)
{   
    std::vector<long double> res = distances(other);
    long double ldres=0;
    for (auto v : res)
        ldres += v*v;
    return sqrt(ldres);
}

inline std::vector<long double> ProfileCLUT_Equidistant::Values::distances(const Values &other, const std::vector<double>& weights)
{   
    std::vector<long double> res(sz_, 0);
    distances(other, weights, res);
    
    return res;
}

inline void ProfileCLUT_Equidistant::Values::distances(const Values &other, const std::vector<double>& weights, std::vector<long double>& res)
{
    if (other.sz_!=sz_)
    {
        qDebug() << "Distances error. sizes different-" << sz_ << other.sz_;
        res.resize(sz_, -1);
        for (auto &el : res)
            el = -1;
        return;
    }
    
    res.resize(sz_, 0);
    for (int i=0;i<sz_;i++)
        res[i] = values_[i]-other.values_[i];
    
    if (weights.size()>=sz_)
        for (int i=0;i<sz_;i++)
            res[i] *= weights[i];
    
    return;
}

inline long double ProfileCLUT_Equidistant::Values::distance(const Values& other, const std::vector<double>& weights)
{
    static std::vector<long double> ds;
    distances(other, weights, ds);
    long double r = 0;
    for (auto el : ds)
        r += el*el;
    
    return r;
}

inline void ProfileCLUT_Equidistant::Values::addValue(long double value)
{
    if (values_.size()==sz_)
        values_.push_back(value);
    else
        values_[sz_] = value;
    ++sz_;
    index_ = -1;
}

inline void ProfileCLUT_Equidistant::Values::subValue()
{
    if (sz_>0)
    {
        --sz_;
        index_ = -1;
    }
}

inline int ProfileCLUT_Equidistant::Values::index(int grid)
{
    //if (grid!=grid_ || index_ == -1)
    {
        index_=0;
        grid_ = grid;
        for (int i=0;i<sz_;i++)
        {
            index_ *= grid_;
            index_ += values_[i];
        }
    }
    
    return index_;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::Values::operator+(const ProfileCLUT_Equidistant::Values& other)
{
    Values res = *this;
    for (unsigned i=0; i<res.sz_ && i<other.sz_;++i)
        res.values_[i] += other.values_[i];
    return res;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::Values::operator*(const ProfileCLUT_Equidistant::Values& other)
{
    Values res = *this;
    for (unsigned i=0; i<res.sz_ && i<other.sz_;++i)
        res.values_[i] *= other.values_[i];
    return res;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::Values::operator-(const ProfileCLUT_Equidistant::Values& other)
{
    Values res = *this;
    for (unsigned i=0; i<res.sz_ && i<other.sz_;++i)
        res.values_[i] -= other.values_[i];
    return res;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::Values::operator/(const ProfileCLUT_Equidistant::Values& other)
{
    Values res = *this;
    for (unsigned i=0; i<res.sz_ && i<other.sz_;++i)
        res.values_[i] /= other.values_[i];
    return res;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::Values::operator*(const double& coef)
{
    Values res = *this;
    for (unsigned i=0; i<res.sz_;++i)
        res.values_[i] *= coef;
    return res;
}

bool ProfileCLUT_Equidistant::Values::operator<(const ProfileCLUT_Equidistant::Values &other)
{
    for (unsigned i=0;i<sz_ && i<other.sz_;++i)
        if (values_[i]!=other.values_[i])
            return values_[i]<other.values_[i];
    return false;
}


ProfileCLUT_Equidistant::ProfileCLUT_Equidistant(int N, int A_count, int B_count) 
        : n_(N), An_(A_count), Bn_(B_count),
        target_(B_count), target_mult_(B_count),
        isSpecialBlack_(false), sk_blackPos_(-1),
        sk_maxC2_(1), sk_TIC_(4), sk_startK_(0),
        sk_maxK_(1), sk_gamma_(2),
        GCR_K_(0), weightK_(100)
{
    int lutSz = n_;
    for (int i=1;i<An_;++i)
        lutSz *= n_;
    lut_.resize(lutSz, Values(Bn_));
    
    isSpecialBlack_ = false;
    Lmin_inv_ = 1;
}

ProfileCLUT_Equidistant::~ProfileCLUT_Equidistant()
{
    qDebug() << "ProfileCLUT_Equidistant::~ProfileCLUT_Equidistant()";
}

int ProfileCLUT_Equidistant::gridCount()
{
    return n_;
}

int ProfileCLUT_Equidistant::inChannels()
{
    return An_;
}

int ProfileCLUT_Equidistant::outChannels()
{
    return Bn_;
}

inline int ProfileCLUT_Equidistant::index(Values& point)
{  
    return point.index(n_);
}

bool ProfileCLUT_Equidistant::ErrValues(const std::vector<long double> &point, const Values &target, std::vector<long double> &res)
{
    if (point.size()<(unsigned)An_)
    {
        qDebug() << "!!!!!!! ErrValues false";
        return false;
    }
       
    value(point).distances(target, outputWeights_, res); 
    //value(point).distances(target, std::vector<double>(), res);  
       
    return true;
}

void ProfileCLUT_Equidistant::setGridValue(Values point, const Values& values)
{
    if (values.size()==Bn_ && point.size()>=An_)
        lut_[index(point)] = values;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::value(const Values &point)
{
    if (point.size()<An_)
        return Values(0);
    
/*    
    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
    std::cout << " ++ search: ";
    for (int i=0;i<point.size();i++)
        std::cout << point[i] << " ";
    std::cout << std::endl;    
*/     
    if (An_==3)
    {
        Values i0(An_), i1(An_);
    
        for (int i=0;i<i1.size();i++)
        {
            i0[i] = int(point[i]*(n_-1));
            i1[i] = i0[i]+1;
        }

        for (int i=0;i<i1.size();i++)
            if (i0[i]<0)
            {
                i0[i] = 0;
                i1[i] = 1;
            }
            else
                if (i1[i]>n_-1)
                {
                    i0[i] = n_-2;
                    i1[i] = n_-1;
                }
                
        //tethrahedral
        long double x0, x1, y0, y1, z0, z1;
        x0 = i0[0]/(n_-1);
        x1 = i1[0]/(n_-1);
        y0 = i0[1]/(n_-1);
        y1 = i1[1]/(n_-1);
        z0 = i0[2]/(n_-1);
        z1 = i1[2]/(n_-1);
        
        long double dx, dy, dz, dx1, dy1, dz1;
        dx = point[0]-x0;
        dy = point[1]-y0;
        dz = point[2]-z0;
        dx1 = x1-x0;
        dy1 = y1-y0;
        dz1 = z1-z0;
        
        Values res(Bn_);
        Values iv(An_);
        long double c1,c2,c3;
        long double p000, p001, p010, p011, p100, p101, p110, p111;
        for (int j=0;j<Bn_;j++)
        {
            iv[0] = i0[0];iv[1] = i0[1];iv[2] = i0[2];
            p000 = lut_[index(iv)][j];
            iv[0] = i0[0];iv[1] = i0[1];iv[2] = i1[2];
            p001 = lut_[index(iv)][j];
            iv[0] = i0[0];iv[1] = i1[1];iv[2] = i0[2];
            p010 = lut_[index(iv)][j];
            iv[0] = i0[0];iv[1] = i1[1];iv[2] = i1[2];
            p011 = lut_[index(iv)][j];
            iv[0] = i1[0];iv[1] = i0[1];iv[2] = i0[2];
            p100 = lut_[index(iv)][j];
            iv[0] = i1[0];iv[1] = i0[1];iv[2] = i1[2];
            p101 = lut_[index(iv)][j];
            iv[0] = i1[0];iv[1] = i1[1];iv[2] = i0[2];
            p110 = lut_[index(iv)][j];
            iv[0] = i1[0];iv[1] = i1[1];iv[2] = i1[2];
            p111 = lut_[index(iv)][j];
               
            if (dx >= dy && dy >= dz)
            {
                c1 = p100 - p000;
                c2 = p110 - p100;
                c3 = p111 - p110;
            }
            else
                if (dx >= dz && dz >= dy)
                {
                    c1 = p100 - p000;
                    c2 = p111 - p101;
                    c3 = p101 - p100;
                }
                else
                    if (dz >= dx && dx >= dy)
                    {
                        c1 = p101 - p001;
                        c2 = p111 - p101;
                        c3 = p001 - p000;
                    }
                    else
                        if (dy >= dx && dx >= dz)
                        {
                            c1 = p110 - p010;
                            c2 = p010 - p000;
                            c3 = p111 - p110;
                        }
                        else
                            if (dy >= dz && dz >= dx)
                            {
                                c1 = p111 - p011;
                                c2 = p010 - p000;
                                c3 = p011 - p010;        
                            }
                            else // dz >= dy && dy >= dx
                            {
                                c1 = p111 - p011;
                                c2 = p011 - p001;
                                c3 = p001 - p000;
                            }
                       
             res[j] = p000 + c1*dx/dx1 + c2*dy/dy1 + c3*dz/dz1;
        }
        return res;
    }
    Values search(point), fixed(0), res(Bn_);
    for (int i=0;i<search.size();i++)
        search[i] *= (n_-1);
    
   
//    for (int i=0;i<res.size();i++)
//        res[i] = subInterpolate(fixed, search, i);
    res = subInterpolate(fixed, search);
    
    return res;
}

void ProfileCLUT_Equidistant::setGridValue(const std::vector<long double> &point, const Values& values)
{
    setGridValue(Values(point), values);
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::value(const std::vector<long double> &point)
{
    return value(Values(point));
}

std::vector<ProfileCLUT_Equidistant::Values> ProfileCLUT_Equidistant::lut()
{
    return lut_;
}

bool ProfileCLUT_Equidistant::setLut(std::vector<ProfileCLUT_Equidistant::Values> lut)
{
    if (lut_.size()!=lut.size())
        return false;

    lut_ = lut;
    return true;
}

ProfileCLUT_Equidistant::Values ProfileCLUT_Equidistant::subInterpolate(ProfileCLUT_Equidistant::Values &fixed, ProfileCLUT_Equidistant::Values &search)
{
    Values res(Bn_);
   
    if (fixed.size()==search.size())
        return lut_[index(fixed)];
      
    int ind = fixed.size();
    
    fixed.addValue(int(search[ind]));
    if (search[ind]==int(search[ind]))
    {
        res = subInterpolate(fixed, search);
        fixed.subValue();
        return res;
    }
       
    long double d;
    Values v1(Bn_), v2(Bn_);
    v1 = subInterpolate(fixed, search);
    
    fixed[ind] = int(search[ind])+1;
    
    v2 = subInterpolate(fixed, search);
    
    d = search[ind] - int(search[ind]);
    res = v1 + (v2-v1)*d;
    fixed.subValue();
    return res;
}

void ProfileCLUT_Equidistant::setMinLForK(double Lmin)
{    
    Lmin_inv_ = 1 - Lmin;
}

ProfileCLUT_Equidistant ProfileCLUT_Equidistant::inverted(int gridCount, double error, InvertedCLUTType clutType,
    FirstApproximationType approximationType/*=MinimalDistance*/, int startPointGrid/*=-1*/, void (*progress_fn)()/*=nullptr*/)
{    
    qDebug() << "ProfileCLUT_Equidistant::inverted" << gridCount << (int)clutType << (int)approximationType;
    
    //error functions 
    auto errorGCRFunc = [&, this](const std::vector<long double> &x)-> double
                        {
                            if (!isSpecialBlack_)
                                return 0;

                            long double tic=0;
                            double res=0;
                            for (auto el : x)
                                tic += el;
                            if (tic>sk_TIC_)
                                res += 1e6*(tic-sk_TIC_);         //total ink count penalty

                            if (x[sk_blackPos_]>sk_maxK_)
                                res += 1e6*(x[sk_blackPos_]-sk_maxK_);  //MaxBlack error

                            res += weightK_*fabs(x[sk_blackPos_]-GCR_K_); //K try to be near GCR -- BlackError
                            return res;
                        };
    
    std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> errorFuncOutput;

    if (clutType==InvertedCLUTType::CMYK_PRINTER_CLUT || clutType==InvertedCLUTType::RGB_PRINTER_CLUT)
        errorFuncOutput = [&, this](const std::vector<long double> &x, const std::vector<long double> &output_x)
                        {
                            long double res = ColorDistance::dE2000Lab(Color::FromLab(output_x[0]*100, output_x[1]*255-128, output_x[2]*255-128),
                                Color::FromLab(target_[0]*100, target_[1]*255-128, target_[2]*255-128));
                            return res+errorGCRFunc(x);
                        };
    else
    {
        errorFuncOutput = [&, this](const std::vector<long double> &x, const std::vector<long double> &output_x)
                        {
                            long double res = 0;
                            for (unsigned i=0;i<output_x.size();++i)
                                res += outputWeights_[i]*outputWeights_[i]*(output_x[i]-target_[i])*(output_x[i]-target_[i]);
                            return sqrtl(res)+errorGCRFunc(x);
                        };
    }

    auto errorFunc = [&, this](const std::vector<long double> &x)
                        {
                            return errorFuncOutput(x, value(x).values_);
                        };


    std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> newtonErrFunc;
    if (clutType==InvertedCLUTType::CMYK_PRINTER_CLUT || clutType==InvertedCLUTType::RGB_PRINTER_CLUT)
    {
        newtonErrFunc = [&] (const std::vector<long double> &v, const std::vector<long double> &axisErr)
        {
            return errorFunc(v);
        };
    }
    else
    {
        newtonErrFunc = [] (const std::vector<long double> &v, const std::vector<long double> &axisErr)
        {
            long double res = 0.0;
            for (auto err : axisErr)
                res += err*err;
            return std::sqrt(res);
        };
    }


    qDebug() << "ProfileCLUT_Equidistant::inverted approximationType =" << approximationType;

    std::vector<Values> targets, targets_mult;
    zeroValues(target_, target_mult_);
    do
    {
        targets.push_back(target_);
        targets_mult.push_back(target_mult_);
    }
    while(incValues(gridCount, target_, target_mult_));

    outputWeights_.resize(Bn_, 1);

    std::vector<std::vector<long double>> startpoints;
    std::vector<bool> inGamut(targets.size(), true);

    Values min_CMYK(An_);
    // calculate Lmin if CMYK
    if (isSpecialBlack_)
    {
        target_ = std::vector<long double>(Bn_, 0);
        std::vector<long double> startpoint(An_, 0);
        for (int i=0;i<An_;++i)
            if (i==sk_blackPos_)
                startpoint[i] = sk_maxK_;
            else
                startpoint[i] = (sk_TIC_-sk_maxK_)/(An_-1);

        //L=0, a=b=0
        target_[0] = 0;
        target_[1] = target_[2] = 128/255.;

        setMinLForK(0);
        if (!calcSpecialBlack(target_.values_, GCR_K_, weightK_))
        {
            qDebug() << "!!! can't calculate GCR K";
            weightK_ = 10;
            GCR_K_ = sk_TIC_/2/An_;
        }
        weightK_ = 100;

        //search first approximation
        startpoint = findRoughApproximationForInvertion(HookeJeevesMethod, startpoint, errorFuncOutput, errorFunc);


        //found exact value with newton search
        startpoint = findResultApproximationForInvertion(startpoint, error, newtonErrFunc, errorGCRFunc);

        Values v = value(startpoint);
        setMinLForK(v[0]);
        qDebug() << "!!!! setMinLForK" << (double)v[0];

        //now get min_CMYK for Lab (0,-0.5,-0.5)
        //L=0, a=b=-0.5
        target_[0] = 0;
        target_[1] = target_[2] = (-0.5+128)/255.;
        for (int i=0;i<An_;++i)
            if (i==sk_blackPos_)
                startpoint[i] = sk_maxK_;
            else
                startpoint[i] = (sk_TIC_-sk_maxK_)/(An_-1);

        if (!calcSpecialBlack(target_.values_, GCR_K_, weightK_))
        {
            qDebug() << "!!! can't calculate GCR K";
            weightK_ = 10;
            GCR_K_ = sk_TIC_/2/An_;
        }
        weightK_ = 100;

        //search first approximation
        startpoint = findRoughApproximationForInvertion(HookeJeevesMethod, startpoint, errorFuncOutput, errorFunc);
        //found exact value with newton search
        startpoint = findResultApproximationForInvertion(startpoint, error, newtonErrFunc, errorGCRFunc);

        min_CMYK = Values(startpoint);
        qDebug() << "min cmyk" << (double)min_CMYK[0] << (double)min_CMYK[1] << (double)min_CMYK[2] << (double)min_CMYK[3];
    }
    //

    qDebug() << "ProfileCLUT_Equidistant::inverted 2";

    if (approximationType == MinimalDistance ||
       approximationType == HookeJeevesMethod)
    {
        if (isSpecialBlack_)
            findClosestPointsForInvertion<true, true>(clutType, targets, startpoints, inGamut, errorFuncOutput, errorFunc);
        else
        {
            if (clutType==InvertedCLUTType::UNKNOWN_CLUT || clutType==InvertedCLUTType::DISPLAY_CLUT)
                findClosestPointsForInvertion<false, false>(clutType, targets, startpoints, inGamut, errorFuncOutput, errorFunc);
            else
                findClosestPointsForInvertion<false, true>(clutType, targets, startpoints, inGamut, errorFuncOutput, errorFunc);
        }
    }
    else
    {
        for (Values v : targets)
            startpoints.push_back(v.values_);
    }
        //startpoints.resize(targets.size(), std::vector<long double>(An_, 0));

    qDebug() << "ProfileCLUT_Equidistant::inverted 3";

    ProfileCLUT_Equidistant res_lut(gridCount, Bn_, An_);

    int prevt = 0;
    for (int t=0;t<targets.size();t++)
    {
        target_ = targets[t];
        target_mult_ = targets_mult[t];
        
        if (prevt!=(int)target_mult_[target_mult_.size()-1])
        {
            prevt=target_mult_[target_mult_.size()-1];
            if (progress_fn)
                (*progress_fn)();
        }
        
        std::vector<long double> startpoint = startpoints[t], resultpoint;
        Values in(Bn_), out(An_);

        for (int i=0;i<Bn_;i++)
            in[i] = target_mult_[i];

        if (isSpecialBlack_)
        {
            if (!calcSpecialBlack(target_.values_, GCR_K_, weightK_))
            {
                qDebug() << "!!! can't calculate GCR K";
                weightK_ = 10;
                GCR_K_ = sk_TIC_/2/An_;
            }

            long double L = target_[0];
            long double a = target_[1]*255.-128;
            long double b = target_[2]*255.-128;

            if (a*a+b*b<=4 && L<=1-Lmin_inv_)
            {
                out = min_CMYK;
                res_lut.setGridValue(in, out);
                qDebug() << "set special min CMYK" << (double)L << (double)a << (double)b
                            << (double)target_[0] << (double)target_[1] << (double)target_[2];
                continue;
            }
        }

        //search first approximation
        startpoint = findRoughApproximationForInvertion(approximationType, startpoint, errorFuncOutput, errorFunc);

//        qDebug() << "startpoint." << (double)target_[0] << (double)target_[1] << (double)target_[2]
//                 << "->" << (double)startpoint[0] << (double)startpoint[1] << (double)startpoint[2];

        //found exact value with newton search
//        if (inGamut[t])
            resultpoint = findResultApproximationForInvertion(startpoint, error, newtonErrFunc, errorGCRFunc);
//        else
//            resultpoint = startpoint;

        for (int i=0;i<An_;i++)
            out[i] = resultpoint[i];

//        qDebug() << (double)in[0] << (double)in[1] << (double)in[2]
//                << "->" << (double)out[0] << (double)out[1] << (double)out[2];

        res_lut.setGridValue(in, out);
    }

    return res_lut;
}

void ProfileCLUT_Equidistant::zeroValues(Values& v, Values& v_multiplied)
{
    for (int i=0;i<v.size();++i)
    {
        v[i] = 0;
        v_multiplied[i] = 0;
    }
}
    
bool ProfileCLUT_Equidistant::incValues(int gridCount, Values& v, Values& v_multiplied)
{
    int i;
    for (i=0;i<v.size() && fabsl(v_multiplied[i] - gridCount+1)<.5;++i)
    {
        v[i] = 0;
        v_multiplied[i] = 0;
    }
    
    if (i>=v.size())
        return false;
    
    v_multiplied[i]++;
    v[i] = v_multiplied[i]/(gridCount-1);
    return true;
}

void ProfileCLUT_Equidistant::setDistanceWeights(const std::vector<double>& outputWeights)
{
    outputWeights_ = outputWeights;
}

void ProfileCLUT_Equidistant::setSpecialBlackMode(bool blackmode/*=true*/)
{
   isSpecialBlack_ = blackmode; 
}
    
    
void ProfileCLUT_Equidistant::setSpecialBlackModeParams(int sk_blackPos, double sk_maxC, 
                               double sk_TIC, double sk_startK,
                               double sk_maxK, double sk_gamma)
{
    sk_blackPos_ = sk_blackPos;
    sk_maxC2_ = sk_maxC*sk_maxC*2*(128./255)*(128./255);
    sk_TIC_ = sk_TIC;
    sk_startK_ = sk_startK;
    sk_maxK_ = sk_maxK;
    sk_gamma_ = sk_gamma;
}

long double ProfileCLUT_Equidistant::targetK(long double L)
{
    L = 1 - L;
    if (L<sk_startK_)
      return 0;
    if (L>Lmin_inv_)
        return sk_maxK_;
    return sk_maxK_*powl((L-sk_startK_)/(Lmin_inv_-sk_startK_), sk_gamma_);
}

bool ProfileCLUT_Equidistant::calcSpecialBlack(const std::vector<long double>& point, long double& resK, long double& Kweight)
{
    if (point.size()<3)
        return false;
    
    double dab = (point[1]*255-128.)*(point[1]*255-128.) + (point[2]*255-128.)*(point[2]*255-128.);
    
    resK = targetK(point[0]);
    
    if (dab>sk_maxC2_*255*255)
      Kweight = 0;
    else
      if (dab<=400)     // dEab <= 20 with a,b in range [-128,127]
        Kweight = 60;  // gray colors
      else
        Kweight = 1;

    
    return true;
}

std::vector<long double> ProfileCLUT_Equidistant::findRoughApproximationForInvertion(FirstApproximationType approximationType,
     const std::vector<long double> &oldStartpoint,
     std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> errorFuncOutput,
     std::function<long double(const std::vector<long double> &)> errorFunc)
{
    std::vector<long double> startpoint = oldStartpoint, resultpoint;
    Values min_v(An_);
    long double minimal, td;
    long double result;

    switch(approximationType)
    {
        case InputAsTarget:
        {
            for (int i=0;i<An_ && i<target_.size();i++)
                startpoint[i] = target_[i];
            for (int i=target_.size();i<An_;i++)
                startpoint[i] = 0.5;

            if (isSpecialBlack_)
                startpoint[sk_blackPos_] = GCR_K_;

            break;
        }
        case HookeJeevesMethod:
        {
            if (approximationType == HookeJeevesMethod)
            {
                std::vector<long double> resultpoint;
                long double result;

//                    qDebug("Before HookeJeevesSearch startpoint: %1.5Lf; %1.5Lf, %1.5Lf", startpoint[0], startpoint[1], startpoint[2]);
                Optimization<long double>::setMinMaxValues(0, 1);
                if (Optimization<long double>::HookeJeevesSearch(startpoint, resultpoint, errorFunc, result, 0.5, 0.01) )
                    startpoint = resultpoint;
//                    if (target_.size()==3 && target_[1]==target_[2] && fabs(target_[1]-.5)<1./(gridCount-1))
//                        qDebug() << "HJ    " << (double)startpoint[0] << (double)startpoint[1] << (double)startpoint[2];
//                    qDebug("After HookeJeevesSearch startpoint: %1.5Lf; %1.5Lf, %1.5Lf", startpoint[0], startpoint[1], startpoint[2]);
            }


            break;
        }
        case GCRandHookeJeevesMethod:
        {
            startpoint[sk_blackPos_] = GCR_K_;
            double v = (sk_TIC_-GCR_K_)/2/(An_-1);
            if (v<0)
                v = 0;
            for (int i=0;i<An_;++i)
                if (i!=sk_blackPos_)
                    startpoint[i] = v;


            Optimization<long double>::setMinMaxValues(0, 1);
            if (Optimization<long double>::HookeJeevesSearch(startpoint, resultpoint, errorFunc, result) )
                startpoint = resultpoint;

            break;
        }
    }

    return startpoint;
}

std::vector<long double> ProfileCLUT_Equidistant::findResultApproximationForInvertion(const std::vector<long double> &startpoint, double error,
        std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> sumErrorFunc,
        std::function<double (const std::vector<long double> &)> errorGCRFunc)
{
    std::vector<long double> resultpoint;
    long double result;

    if (Bn_>=An_)
    {
        Optimization<long double>::setMinMaxValues(0, 1);
        Optimization<long double>::NewtonSearch(startpoint, resultpoint,
                                                [this] (const std::vector<long double> &p, std::vector<long double> &r) -> bool {
                                                            std::vector<long double> v = value(p).values_;
                                                            r.resize(v.size(), 0);
                                                            for (unsigned i=0;i<p.size();++i)
                                                                r[i] = outputWeights_[i]*(v[i]-target_[i]);
                                                            return true;
                                                        },
                                                sumErrorFunc, errorGCRFunc, error, result, 200);

//            if (target_.size()==3 && target_[1]==target_[2] && fabs(target_[1]-.5)<1./(gridCount-1))
//                        qDebug() << "Newton" << (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2];

//           if (result > 0.1)
//           {
//               qDebug() << "inverted startpoint:" << (double)startpoint[0] << (double)startpoint[1] << (double)startpoint[2] << "result:" << (double)result;
//               qDebug() << "resultpoint:" << (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2];
//           }
    }
    else
        if (Bn_+1==An_ && isSpecialBlack_)
        {
            //case when we can just move by CMY with fixed K
            std::vector<long double> startpoint2(startpoint);
            long double K = startpoint[sk_blackPos_];
            startpoint2.erase(startpoint2.begin()+sk_blackPos_);
            Optimization<long double>::setMinMaxValues(0, 1);
            bool newtonres = Optimization<long double>::NewtonSearch(startpoint2, resultpoint,
                                                    [&, this] (const std::vector<long double> &p, std::vector<long double> &r) -> bool {
                                                                static std::vector<long double> t;
                                                                t = p;
                                                                t.insert(t.begin()+sk_blackPos_, K);
                                                                t = value(t).values_;
                                                                r.resize(t.size());
                                                                for (unsigned i=0;i<r.size();++i)
                                                                    r[i] = outputWeights_[i]*(t[i]-target_[i]);
                                                                return true;
                                                            },
                                                    [&, this] (const std::vector<long double> &p, const std::vector<long double> &r) -> long double {
                                                                static std::vector<long double> t;
                                                                t = p;
                                                                t.insert(t.begin()+sk_blackPos_, K);
                                                                return sumErrorFunc(t, r);
                                                            },
                                                    [&, this] (const std::vector<long double> &x) -> long double {
                                                                double tic = cppfunc::sum(x) + K;
                                                                if (tic>sk_TIC_)
                                                                    return 1e6;     //total ink count over limited
                                                                return 0;   //no gcr error here
                                                            },
                                                    error, result, 20);

            resultpoint.insert(resultpoint.begin()+sk_blackPos_, K);

//                if (target_.size()==3 && target_[1]==target_[2] && fabs(target_[1]-.5)<1./(gridCount-1))
//                        qDebug() << "Newton" << (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << newtonres;

            if (!newtonres)
                resultpoint = startpoint;
        }
        else
            resultpoint = startpoint;

    return resultpoint;
}

// PointFilling template removed - no longer needed with simple PointND structure

template<bool isSpecialBlack, bool useUV>
void ProfileCLUT_Equidistant::findClosestPointsForInvertion(InvertedCLUTType clutType,
        const std::vector<Values> &targets,
        std::vector<std::vector<long double>> &startpoints,
        std::vector<bool> &inGamut,
        std::function<long double(const std::vector<long double> &, const std::vector<long double> &)> errorFuncOutput,
        std::function<long double(const std::vector<long double> &)> errorFunc )
{
    startpoints.clear();
    inGamut.clear();
    inGamut.resize(targets.size(), true);

    // Simple point structure to replace Boost geometry
    struct Point3D {
        double x, y, z;
        Point3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    };
    
    struct PointND {
        std::vector<double> coords;
        PointND(int dim) : coords(dim, 0.0) {}
        void set(int i, double value) { if (i < coords.size()) coords[i] = value; }
        double get(int i) const { return (i < coords.size()) ? coords[i] : 0.0; }
    };
    
    typedef std::pair<PointND, int> ValuePair;

    std::vector<Values> input_points;
    std::vector<ValuePair> point_list; // Simple vector instead of R-tree

    std::vector<Point3D> P(lut_.size());
    Values iter(An_), iter_mult(An_);

//    qDebug() << "added type" << (int)clutType;

    zeroValues(iter, iter_mult);
    do
    {
        int ind = index(iter_mult);
        Values &c = lut_[ind];
        input_points.push_back(iter);
        Color col = Color::FromLab(c.values_[0]*100, c.values_[1]*255-128, c.values_[2]*255-128);
        if (clutType==InvertedCLUTType::UNKNOWN_CLUT)
            col = Color::FromLab(c.values_[0], c.size()>1 ? c.values_[1] : 0, c.size()>2 ? c.values_[2] : 0);
        if (clutType==InvertedCLUTType::DISPLAY_CLUT)
            col = Color::FromXYZ(c.values_[0]*100, c.values_[1]*100, c.values_[2]*100);

        PointND p(isSpecialBlack ? 6 : (useUV ? 5 : 3));
        Point3D sp;

        p.set(0, col.L());
        p.set(1, col.a());
        p.set(2, col.b());
        if (useUV) {
            p.set(3, col.u());
            p.set(4, col.v());
        }
        if (isSpecialBlack)  //to avoid using -1 index (sk_blackPos_=-1)
            p.set(5, 90*iter[sk_blackPos_]);
        point_list.push_back(std::make_pair(p, static_cast<int>(input_points.size()-1)));

        sp.x = col.L();
        sp.y = col.a();
        sp.z = col.b();
        P[ind] = sp;
//        qDebug() << "added" << ind << "------"
//                 << p.template get<0>()
//                 << p.template get<1>()
//                 << p.template get<2>()
//                 << p.template get<3>()
//                 << p.template get<4>()
//                 << "---------------------"
//                 << (double)(100*targetK(col.L()/100));
    }while(incValues(n_, iter, iter_mult));


    Values neighbors(An_), neighbors_mult(An_);
    double maxDist = 0.0;
    zeroValues(iter, iter_mult);
    do
    {
        zeroValues(neighbors, neighbors_mult);
        do
        {
            Values r = iter_mult + neighbors_mult;
            bool in=true;
            for (unsigned i = 0;i<r.sz_ && in;++i)
                if (r[i]>=n_)
                    in = false;

            if (in)
            {
                // Simple Euclidean distance calculation
                Point3D& p1 = P[index(iter_mult)];
                Point3D& p2 = P[index(r)];
                double dist = std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) + (p1.z-p2.z)*(p1.z-p2.z));
                //qDebug() << index(iter_mult) << index(r) << dist;
                maxDist = std::max(dist, maxDist);
            }
        }while(incValues(2, neighbors, neighbors_mult));

    }while(incValues(n_, iter, iter_mult));


    Values gray(An_);
    for (int i=0;i<An_;++i)
        gray.values_[i] = 0;

    long double Lmin = 1-Lmin_inv_;
    for (int t=0;t<targets.size();t++)
    {
        const Values &v = targets[t];
        Color col = Color::FromLab(v.values_[0]*100, v.values_[1]*255-128, v.values_[2]*255-128);
        if (clutType==InvertedCLUTType::UNKNOWN_CLUT)
            col = Color::FromLab(v.values_[0], v.values_[1], v.values_[2]);
        if (clutType==InvertedCLUTType::DISPLAY_CLUT)
            col = Color::FromXYZ(v.values_[0]*100, v.values_[1]*100, v.values_[2]*100);
        PointND target(isSpecialBlack ? 6 : (useUV ? 5 : 3));
        target.set(0, col.L());
        target.set(1, col.a());
        target.set(2, col.b());
        if (useUV) {
            target.set(3, col.u());
            target.set(4, col.v());
        }
        long double K = targetK(col.L()/100);
        if (col.a()*col.a()+col.b()*col.b()>400)
        {
            setMinLForK(0);
            K = targetK(col.L()/100);
            setMinLForK(Lmin);
        }
        if (isSpecialBlack)  //to avoid using -1 index (sk_blackPos_=-1)
            target.set(5, 90*targetK(col.L()/100));

//        qDebug() << "target" << t
//                 << target.template get<0>()
//                 << target.template get<1>()
//                 << target.template get<2>()
//                 << target.template get<3>()
//                 << target.template get<4>()
//                 << (double)(100*targetK(col.L()/100));


        // Linear search for nearest point (replaces R-tree query)
        int closest_index = 0;
        double min_distance = std::numeric_limits<double>::max();
        
        for (size_t i = 0; i < point_list.size(); ++i) {
            double distance = 0.0;
            for (int j = 0; j < target.coords.size(); ++j) {
                double diff = target.get(j) - point_list[i].first.get(j);
                distance += diff * diff;
            }
            distance = std::sqrt(distance);
            
            if (distance < min_distance) {
                min_distance = distance;
                closest_index = i;
            }
        }
        
        startpoints.push_back(input_points[point_list[closest_index].second].values_);

        Point3D starget, sclosest;
        starget.x = col.L();
        starget.y = col.a();
        starget.z = col.b();
        sclosest.x = point_list[closest_index].first.get(0);
        sclosest.y = point_list[closest_index].first.get(1);
        sclosest.z = point_list[closest_index].first.get(2);
        // Simple Euclidean distance calculation
        double dist = std::sqrt((starget.x-sclosest.x)*(starget.x-sclosest.x) + 
                               (starget.y-sclosest.y)*(starget.y-sclosest.y) + 
                               (starget.z-sclosest.z)*(starget.z-sclosest.z));
        if (dist > 2.0*maxDist )
            inGamut[t] = false;

//        qDebug() << "found" << t << "------"
//                 << (double)startpoints[t][0]
//                 << (double)startpoints[t][1]
//                 << (double)startpoints[t][2]
//                 << (double)startpoints[t][2]
//                 << inGamut[t]
//                 << "("
//                 << (double)targets[t][0]
//                 << (double)targets[t][1]
//                 << (double)targets[t][2]
//                 << "---" << (double)(100*targetK(col.L()/100))
//                 << ")";
    }
}
