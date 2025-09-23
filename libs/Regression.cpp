/* 
 * File:   Regression.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 29 листопада 2010, 12:51
 */

#include "Regression.h"
#include "Interpolation.h"

#include <cmath>
#include <algorithm>
#include "Matrix.h"

#include <QDebug>

template <class XType, class YType>
Regression<XType, YType>::Regression()
{
}

template <class XType, class YType>
Regression<XType, YType>::~Regression()
{
}

template <class XType, class YType>
bool Regression<XType, YType>::LinearRegression(const std::vector<XType>& xs, const std::vector<YType>& ys, long double& resk, long double& resb)
{
    std::vector<long double> res;
    if (!NPowerRegression(1, xs, ys, res))
        return false;
    resk = res[0];
    resb = res[1];
    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::QuadraticRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                           long double& resa, long double& resb, long double& resc)
{
    std::vector<long double> res;
    if (!NPowerRegression(2, xs, ys, res))
        return false;
    resa = res[0];
    resb = res[1];
    resc = res[2];
    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::NPowerRegression(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                            std::vector<long double>& res)
{
    res.resize(n+1);

    if (xs.size()!=ys.size() || xs.size()<n+1)
        return false;

    Matrix A(xs.size(), n+1);

    for (int i=0;i<A.rows();i++)
    {
        for (int j=0;j<n;j++)
            A.at(i, j) = powl(xs[i], n-j);
        A.at(i, n) = 1; //0 power
    }

    Matrix AT = Matrix::Transpone(A);


    Matrix Y(ys.size(), 1);
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = ys[i];

    Matrix W = Matrix::Inverse(AT*A)*(AT*Y);

    if (W.rows()<n+1)
        return false;

    for (int i=0;i<n+1;i++)
        res[i] = W.at(i, 0);

    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::NPowerRegressionNormalized(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                                           std::vector<long double>& res, std::vector<XType>& normalizecoefs,
                                           long double lambda)
{    
    res.resize(n+1);

    if (xs.size()!=ys.size() || xs.size()<(unsigned)n+1)
        return false;
    
    //normalization
    normalizecoefs.resize((n+1)*2);
    long double nt;
    double mmax, mmin;
    mmax = *std::max_element(xs.begin(), xs.end());
    mmin = *std::min_element(xs.begin(), xs.end());
    for (int j=0;j<n;j++)
    {
        nt = 0;
        for (unsigned i=0;i<xs.size();i++)
           nt += powl(xs[i], n-j);
        nt /= xs.size();
        normalizecoefs[j*2] = nt;
        normalizecoefs[j*2+1] = fabsl(powl(mmax, n-j) - powl(mmin, n-j));
    }
    
    nt = 0;
    for (unsigned i=0;i<ys.size();i++)
       nt += ys[i];
    nt /= ys.size();
    normalizecoefs[n*2] = nt;
    normalizecoefs[n*2+1] = fabsl(*std::max_element(ys.begin(), ys.end()) - *std::min_element(ys.begin(), ys.end()));

    Matrix A(xs.size(), n+1);

    for (int i=0;i<A.rows();i++)
    {
        for (int j=0;j<n;j++)
            A.at(i, j) = (powl(xs[i], n-j) - normalizecoefs[j*2])/normalizecoefs[j*2+1];
        A.at(i, n) = 1; //0 power
    }

    Matrix AT = Matrix::Transpone(A);


    Matrix Y(ys.size(), 1);
//    for (int i=0;i<Y.rows();i++)
//        Y.at(i, 0) = (ys[i] - normalizecoefs[n*2])/normalizecoefs[n*2+1];
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = ys[i];
    
    Matrix T = AT*A;
    for (int i=0;i<T.rows()-1 && i<T.columns()-1;i++)
        T.at(i,i) += lambda;

    Matrix W = Matrix::Inverse(T)*(AT*Y);

    if (W.rows()<n+1)
        return false;

    for (int i=0;i<n+1;i++)
        res[i] = W.at(i, 0);

    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::WeightedLinearRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                                                        const std::vector<long double>& w,
                                                        long double& resk, long double& resb)
{
    std::vector<long double> res;
    if (!WeightedNPowerRegression(1, xs, ys, w, res))
        return false;
    resk = res[0];
    resb = res[1];
    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::WeightedQuadraticRegression(const std::vector<XType>& xs, const std::vector<YType>& ys,
                           const std::vector<long double>& w,
                           long double& resa, long double& resb, long double& resc)
{
    std::vector<long double> res;
    if (!WeightedNPowerRegression(2, xs, ys, w, res))
        return false;
    resa = res[0];
    resb = res[1];
    resc = res[2];
    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::WeightedNPowerRegression(int n, const std::vector<XType>& xs, const std::vector<YType>& ys,
                            const std::vector<long double>& w,
                            std::vector<long double>& res)
{
    res.resize(n+1);

    if (xs.size()!=ys.size() || xs.size()!=w.size() || xs.size()<n+1)
        return false;

    Matrix A(xs.size(), n+1);

    for (int i=0;i<A.rows();i++)
    {
        for (int j=0;j<n;j++)
            A.at(i, j) = w[i]*powl(xs[i], n-j);
        A.at(i, n) =  w[i]; //0 power
    }

    Matrix AT = Matrix::Transpone(A);


    Matrix Y(ys.size(), 1);
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = w[i]*ys[i];

    Matrix W = Matrix::Inverse(AT*A)*(AT*Y);

    if (W.rows()<n+1)
        return false;

    for (int i=0;i<n+1;i++)
        res[i] = W.at(i, 0);

    return true;
}

template <class XType, class YType>
YType Regression<XType, YType>::ApplyRegression(XType x, std::vector<long double>& coefs)
{
    long double res = 0;
    for (unsigned i=0;i<coefs.size();i++)
        res = res*x + coefs[i];

    return Interpolation::round<YType>(res);
}

template <class XType, class YType>
YType Regression<XType, YType>::ApplyRegressionNormalized(XType x, std::vector<long double>& coefs, std::vector<XType>& normalizecoefs)
{
    if (!coefs.size() || coefs.size()*2!=normalizecoefs.size())
        return -1;
    
    long double res = 0;
    int n = coefs.size()-1;
    for (int i=0;i<n;i++)
        res += (powl(x, n-i)-normalizecoefs[2*i])/normalizecoefs[2*i+1]*coefs[i];
    res += coefs[n];

    //res = res*normalizecoefs[n*2+1] + normalizecoefs[n*2];
    
    return static_cast<YType>(res);
}

template <class XType, class YType>
void Regression<XType, YType>::ExpandXs(const std::vector<XType> &x, unsigned maxpower, std::vector<XType> &resX)
{
    std::vector<unsigned> pows(x.size(), 0);
    resX.clear();
    
    bool finish = false;
    unsigned long sum;
    unsigned i;
    long double t;
    do
    {
        sum = 0;
        for (i=0;i<pows.size();i++)
            sum += pows[i];
        
        if (sum<=maxpower)
        {
            t = 1;
            for (i=0;i<x.size();i++)
                t *= powl(x[i], pows[i]);
            
            resX.push_back(t);
        }
        
        for (i=0;i<pows.size() && pows[i]==maxpower;i++)
            pows[i] = 0;
        
        if (i==pows.size())
            finish = true;
        else
            pows[i]++;
        
    }while(!finish);
}

template <class XType, class YType>
bool Regression<XType, YType>::MultiRegression(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                const std::vector<YType> &ys,
                                std::vector<long double> &resCoefs,
                                double lambda)
{
    if (xs.size()!=ys.size() || !xs.size())
        return false;
    
    std::vector<std::vector<XType> > expX(xs.size());
    for (unsigned i=0;i<xs.size();i++)
        ExpandXs(xs[i], maxpower, expX[i]);
    
    if (expX[0].size()>expX.size())
        return false;
    resCoefs.resize(expX[0].size(), 0);
    
    Matrix A(expX.size(), expX[0].size());

    for (int i=0;i<A.rows();i++)
        for (int j=0;j<A.columns();j++)
            A.at(i, j) =  expX[i][j];

    Matrix Y(ys.size(), 1);
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = ys[i];   
    
    Matrix AT = Matrix::Transpone(A);
    Matrix T = AT*A;
    for (int i=1;i<T.rows() && i<T.columns();i++)
        T.at(i,i) += lambda;

    Matrix W = Matrix::Inverse(T)*(AT*Y);

    if ((unsigned)W.rows()<resCoefs.size())
        return false;

    for (int i=0;i<W.rows();i++)
        resCoefs[i] = W.at(i, 0);

    return true;
}

template <class XType, class YType>
bool Regression<XType, YType>::MultiRegressionWeighted(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                        const std::vector<YType> &ys,
                                        const std::vector<YType> &weights,
                                        std::vector<long double> &resCoefs,
                                        double lambda/*=0*/)
{
    if (xs.size()!=ys.size() || !xs.size())
        return false;
    
    std::vector<std::vector<XType> > expX(xs.size());
    for (unsigned i=0;i<xs.size();i++)
        ExpandXs(xs[i], maxpower, expX[i]);
    
    if (expX[0].size()>expX.size())
        return false;
    resCoefs.resize(expX[0].size(), 0);
    
    Matrix A(expX.size(), expX[0].size());

    for (int i=0;i<A.rows();i++)
        for (int j=0;j<A.columns();j++)
            A.at(i, j) =  expX[i][j];

    Matrix Y(ys.size(), 1);
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = ys[i];
    
    //add weights
    for (int i=0;i<A.rows() && i<Y.rows() && unsigned(i)<weights.size();i++)
    {
        Y.at(i, 0) *= weights[i];
        for (int j=0;j<A.columns();j++)
            A.at(i, j) *= weights[i];
    }
    
    
    Matrix AT = Matrix::Transpone(A);
    Matrix T = AT*A;
    for (int i=1;i<T.rows() && i<T.columns();i++)
        T.at(i,i) += lambda;

    Matrix W = Matrix::Inverse(T)*(AT*Y);

    if ((unsigned)W.rows()<resCoefs.size())
        return false;

    for (int i=0;i<W.rows();i++)
        resCoefs[i] = W.at(i, 0);

    return true;
}
    
template <class XType, class YType>
YType Regression<XType, YType>::ApplyMultiRegression(const std::vector<XType> &x, unsigned maxpower,
                                const std::vector<long double> &coefs)
{
    std::vector<XType> expX;
    ExpandXs(x, maxpower, expX);
       
    long double res = 0;
    for (unsigned i=0;i<expX.size() && i<coefs.size();i++)
        res += expX[i]*coefs[i];
    
    return static_cast<YType>(res);
}

template <class XType, class YType>
bool Regression<XType, YType>::MultiRegressionNormalized(const std::vector<std::vector<XType> > &xs, unsigned maxpower,
                                const std::vector<YType> &ys,
                                std::vector<long double> &resCoefs, 
                                std::vector<long double>& normalizecoefs,
                                double lambda/*=0*/)
{
    if (xs.size()!=ys.size() || !xs.size())
        return false;
    
    std::vector<std::vector<XType> > expX(xs.size());
    for (unsigned i=0;i<xs.size();i++)
        ExpandXs(xs[i], maxpower, expX[i]);
    
    if (expX[0].size()>expX.size())
        return false;
    resCoefs.resize(expX[0].size(), 0);
    
    Matrix A(expX.size(), expX[0].size());

    for (int i=0;i<A.rows();i++)
        for (int j=0;j<A.columns();j++)
            A.at(i, j) =  expX[i][j];
    
    calculateNormalization(A, normalizecoefs, 0);
    applyNormalization(A, normalizecoefs, 0);

    Matrix AT = Matrix::Transpone(A);


    Matrix Y(ys.size(), 1);
    for (int i=0;i<Y.rows();i++)
        Y.at(i, 0) = ys[i];
    
    Matrix T = AT*A;
    for (int i=1;i<T.rows() && i<T.columns();i++)
        T.at(i,i) += lambda;

    Matrix W = Matrix::Inverse(T)*(AT*Y);

    if ((unsigned)W.rows()<resCoefs.size())
        return false;

    for (int i=0;i<W.rows();i++)
        resCoefs[i] = W.at(i, 0);

    return true;
}
    
template <class XType, class YType>
YType Regression<XType, YType>::ApplyMultiRegressionNormalized(const std::vector<XType> &x, unsigned maxpower,
                            const std::vector<long double> &coefs, 
                            std::vector<long double>& normalizecoefs)
{
    std::vector<XType> expX;
    ExpandXs(x, maxpower, expX);
    applyNormalization(expX, normalizecoefs, 0);
       
    long double res = 0;
    for (unsigned i=0;i<expX.size() && i<coefs.size();i++)
        res += expX[i]*coefs[i];
    
    return static_cast<YType>(res);
}

template <class XType, class YType>
void Regression<XType, YType>::calculateNormalization(Matrix &A, std::vector<long double>& normalizecoefs, int ignoredcolumn/*=-1*/)
{
    if (!A.rows())
    {
        normalizecoefs.resize(0);
        return;
    }
    
    int sz = A.columns();
    if (ignoredcolumn>=0 && ignoredcolumn<sz)
        --sz;
    
    normalizecoefs.resize(2*sz);
    long double sum, vmax, vmin, v;
    for (int c=0, pos=0;c<A.columns();++c, ++pos)
    {
        if (c==ignoredcolumn)
        {
            --pos;
            continue;
        }
        
        sum = vmax = vmin = A.at(0,c);
        
        for (int r=1;r<A.rows();++r)
        {
            v = A.at(r,c);
            sum += v;
            if (v>vmax)
                vmax = v;
            if (v<vmin)
                vmin = v;
        }
        
        normalizecoefs[pos*2] = sum/A.rows();
        normalizecoefs[pos*2+1] = fabsl(vmax-vmin)<1e-9 ? 1 : (vmax-vmin);
    }
}

template <class XType, class YType>
void Regression<XType, YType>::applyNormalization(Matrix &A, std::vector<long double>& normalizecoefs, int ignoredcolumn/*=-1*/)
{
    int sz = A.columns();
    if (ignoredcolumn>=0 && ignoredcolumn<sz)
        --sz;
    
    if (normalizecoefs.size()!=(unsigned)2*sz)
    {
        qDebug() << "real applyNormalization matrix failed" << normalizecoefs.size()
                 << sz;
        return;
    }
    
     //QString s[A.rows()];
    std::vector<QString> s(A.rows());
     
     for (int r=0;r<A.rows();++r)
        s[r] = "real applyNormalization matrix:";
    
    for (int c=0, pos=0;c<A.columns();++c, ++pos)
    {
        if (c==ignoredcolumn)
        {
            --pos;
            for (int r=0;r<A.rows();++r)
                s[r]+= " "+QString::number((double)A.at(r,c));
            continue;
        }
               
        for (int r=0;r<A.rows();++r)
        {
            A.at(r,c) = (A.at(r,c)-normalizecoefs[2*pos])/normalizecoefs[2*pos+1];
            s[r]+= " "+QString::number((double)A.at(r,c));
        }
    }
     
//     for (int r=0;r<A.rows();++r)
//        qDebug() << s[r];
}

template <class XType, class YType>
void Regression<XType, YType>::applyNormalization(std::vector<XType> &expX, std::vector<long double>& normalizecoefs, int ignoredcolumn/*=-1*/)
{
    int sz = (int)expX.size();
    if (ignoredcolumn>=0 && ignoredcolumn<sz)
        --sz;
    
    if ((int)normalizecoefs.size()!=2*sz)
    {
        qDebug() << "real applyNormalization vector failed" << normalizecoefs.size()
                 << sz;
        return;
    }
    
    QString s = "real applyNormalization vector:";
    
    for (int c=0, pos=0;c<(int)expX.size();++c, ++pos)
    {
        if (c==ignoredcolumn)
        {
            --pos;
            s+= " "+QString::number((double)expX[c]);
            continue;
        }
               
        expX[c] = (expX[c]-normalizecoefs[2*pos])/normalizecoefs[2*pos+1];
        s+= " "+QString::number((double)expX[c]);
    }
//    qDebug() << s;
}

template <class XType, class YType>
bool Regression<XType, YType>::ConvertRegressionToMultiRegression(unsigned maxpower, int channels, int channelIdx, 
                                const std::vector<long double> &coefs, std::vector<long double> &multyCoefs)
{
    multyCoefs.clear();
    
    std::vector<unsigned> pows(channels, 0);
    
    bool finish = false;
    unsigned i;
    do
    {
        int sum = 0;
        for (i=0;i<pows.size();i++)
            sum += pows[i];
        
        if (sum<=maxpower)
        {
            if (pows[channelIdx] == sum)
            {
                if (maxpower - sum >= coefs.size() || maxpower - sum < 0)
                    return false;
                multyCoefs.push_back(coefs[maxpower - sum]);
            }
            else
                multyCoefs.push_back(0.0);
        }
        
        for (i=0;i<pows.size() && pows[i]==maxpower;i++)
            pows[i] = 0;
        
        if (i==pows.size())
            finish = true;
        else
            pows[i]++;
        
    }while(!finish);
    return true;
}

template class Regression<int, int>;
template class Regression<int, double>;
template class Regression<double, double>;
template class Regression<double, int>;
template class Regression<long double, double>;
template class Regression<long double, long double>;
