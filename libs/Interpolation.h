#ifndef INTERPOLATION_H
#define	INTERPOLATION_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <QDebug>

#include <iostream>

#include "Matrix.h"
#include "MatrixSolve.h"

#include "cppfunc.h"

#include <QString>

class Interpolation
{
    template<class XType, class YType> static void ParabolicCoefs(XType x0, YType y0, XType x1, YType y1, XType x2, YType y2, long double &a, long double &b, long double &c);
    
    static long double polyHarmonicBase(long double d);
    template<class XType> static long double polyHarmonicBase(const std::vector<XType> &v1, const std::vector<XType> &v2);
public:
    template<class YType> static YType round(long double t);

    template<class XType, class YType> static YType Linear(std::vector<XType> &xbefore, std::vector<YType> &ybefore, XType x);
    template<class XType, class YType> static bool Linear(std::vector<XType> &xbefore, std::vector<YType> &ybefore, std::vector<XType> &xafter, std::vector<YType> &yout);

	template<class XType, class YType> static bool Spline(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout);
	template<class XType, class YType> static bool SplineAkima(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout);
	template<class XType, class YType> static bool SplineAkima2(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout);
    
	template<class XType, class YType> static bool SplineHermite(std::vector<XType> &xbefore, std::vector<YType> &ybefore, std::vector<XType> &xafter, std::vector<YType> &yout);
	template<class XType, class YType> static bool SplineHermite_Olga(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout);
    
    /**
     * Calculating of polyharmonic interpolation coeficients. r^2*ln(r) used as radial basis function
     * @param xbefore x's of key points (can be any dimention)
     * @param ybefore targets for key points
     * @param results calculated coeficients
     * @param smooth level of smoothing. by default=0.0 - no smoothing
     * @return is successfull. Can be false if wrong input data or not enough info.
     */
    template<class XType, class YType> static bool buildPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<YType> &ybefore, std::vector<long double> &results, double smooth=0.0);
    
    /**
     * Calculating of polyharmonic interpolation coeficients. MultiDimention result version
     * r^2*ln(r) used as radial basis function
     * @param xbefore x's of key points (can be any dimention)
     * @param ybefore targets for key points
     * @param results calculated coeficients
     * @param smooth level of smoothing. by default=0.0 - no smoothing
     * @return is successfull. Can be false if wrong input data or not enough info.
     */
    template<class XType, class YType> static bool buildPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<std::vector<YType> > &ybefore, std::vector<std::vector<long double> > &results, double smooth=0.0);
    
    /**
     * Applying of polyharmonic interpolation to exact point
     * @param xbefore x's of key points (can be any dimention) - MUST be same as input to buildPolyHarmonic
     * @param coefs coeficients calculated by buildPolyHarmonic
     * @param x coordinates of point to calculate value - MUST be same dimentions as xbefore
     * @param result interpolated value
     * @return is successfull. Can be false if wrong input data
     */
    template<class XType, class YType> static bool ApplyPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<long double> &coefs, 
                                                                     const std::vector<XType> &x, YType &result);

    /**
     * Applying of polyharmonic interpolation to exact point. MultiDimention result version
     * @param xbefore x's of key points (can be any dimention) - MUST be same as input to buildPolyHarmonic
     * @param coefs coeficients calculated by buildPolyHarmonic
     * @param x coordinates of point to calculate value - MUST be same dimentions as xbefore
     * @param result interpolated value
     * @return is successfull. Can be false if wrong input data
     */
    template<class XType, class YType> static bool ApplyPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<std::vector<long double> > &coefs, 
                                                                     const std::vector<XType> &x, std::vector<YType> &result);
};

template<class YType> 
inline YType Interpolation::round(long double t)
{
    return t;
}


template<> 
inline int Interpolation::round(long double t)
{
    return t<0 ? t-0.5 : t+.5;
}

template<> 
inline unsigned Interpolation::round(long double t)
{
    return t<0 ? 0 : t+.5;
}

template<> 
inline long Interpolation::round(long double t)
{
    return t<0 ? t-0.5 : t+.5;
}

template<class XType, class YType> YType Interpolation::Linear(std::vector<XType> &xbefore, std::vector<YType> &ybefore, XType x)
{
    if (xbefore.size()<2 && xbefore.size()!=ybefore.size())
        return 0;

    unsigned pos;
    for (pos=0;pos<xbefore.size()-1 && x>xbefore[pos+1];pos++)
        ;
    
    if (pos==xbefore.size()-1)
        pos--;

    return ybefore[pos] + (long double)(x-xbefore[pos])*(ybefore[pos+1]-ybefore[pos])/(xbefore[pos+1]-xbefore[pos]);
}

template<class XType, class YType> bool Interpolation::Linear(std::vector<XType> &xbefore, std::vector<YType> &ybefore, std::vector<XType> &xafter, std::vector<YType> &yout)
{
    if (xbefore.size()<2 && xbefore.size()!=ybefore.size())
        return false;

    yout.clear();
    for (unsigned i=0;i<xafter.size();i++)
        yout.push_back(Linear<XType, YType>(xbefore, ybefore, xafter[i]));

    return true;
}

template<class XType, class YType> bool Interpolation::Spline(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout)
{
    if (xbefore.size()<4 || xbefore.size()!=ybefore.size())
        return false;

    yout.resize(xafter.size());

    int bcount, acount;
    bcount = xbefore.size();
    acount = xafter.size();

    int i, j;

    long double *d, *h, *m, *u;
    long double *s1, *s2, *s3;
    long double *pa, *pb;
    long double px;
    long double t;

   

     int N=bcount-1;

     //memory allocation
     d = new long double[N];
     h = new long double[N];
     m = new long double[N+1];
     u = new long double[N];
     s1 = new long double[N];
     s2 = new long double[N];
     s3 = new long double[N];
     pa = new long double[N];
     pb = new long double[N];

    //calculate easest parameters
    for (i=0;i<N;i++)
    {
        h[i] = (xbefore[i+1]-xbefore[i]);
        if (h[i]==0)
            h[i] = 1e-5;
        d[i] = (ybefore[i+1]-ybefore[i])/h[i];
        if (i>0)
            u[i] = 6*(d[i]-d[i-1]);
    }

    //find second derivation points (m)
    m[0] = 0;
    m[N]= 0;

    pa[1] = -h[1]/(2*(h[0]+h[1]));
    pb[1] = u[1]/(2*(h[0]+h[1]));
    for (i=2;i<N-1;i++)
    {
        pa[i] = -h[i]/(2*(h[i]+h[i-1])+h[i-1]*pa[i-1]);
        pb[i] = (u[i]-h[i-1]*pb[i-1])/(2*(h[i]+h[i-1])-h[i-1]*pa[i-1]);
    }

    m[N-1] = (u[N-1]-h[N-2]*pb[N-2])/(2*(h[N-1]+h[N-2])-h[N-2]*pa[N-2]);
    for (i=N-2;i>0;i--)
        m[i] = pa[i]*m[i+1]+pb[i];


    //find spline coefs
    for (i=0;i<N;i++)
    {
        s1[i] = d[i] - h[i]/6.*(2*m[i]+m[i+1]);
        s2[i] = m[i]/2;
        s3[i] = (m[i+1]-m[i])/(6*h[i]);
    }

//    int lastj;
    j=0;
    for (i=0;i<acount;i++)
    {
//        lastj = j;
        for (;j>0 && xafter[i]<xbefore[j];j--)
            ;
        for (;j<bcount-2 && xafter[i]>=xbefore[j+1];j++)
            ;

        px =  (xafter[i]-xbefore[j]);

        t = ((s3[j]*px+s2[j])*px+s1[j])*px+ybefore[j];
/*
        qDebug() << "interpolation. " << xafter[i] << "->" << double(t)
                 << ". pos=" << j << ", px=" << double(px)
                 << "coef[3]=" <<  double(coef[4*j+3])
                 << "coef[2]=" <<  double(coef[4*j+2])
                 << "coef[1]=" <<  double(coef[4*j+1])
                 << "coef[0]=" <<  double(coef[4*j]);
 */
        yout[i] = /*isconvert ? MyToInt(t) :*/ t;
    }


    //free memory
    delete[] d;
    delete[] h;
    delete[] m;
    delete[] u;
    delete[] s1;
    delete[] s2;
    delete[] s3;
    delete[] pa;
    delete[] pb;


    //borders check
    YType miny, maxy;
    miny = *std::min_element(ybefore.begin(), ybefore.end());
    maxy = *std::max_element(ybefore.begin(), ybefore.end());

    for (i=0;i<acount;i++)
    {
         if (yout[i]<miny)
                 yout[i] = miny;
         if (yout[i]>maxy)
                  yout[i] = maxy;
    }

    return true;
}


template<class XType, class YType> bool Interpolation::SplineAkima2(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout)
{
    if (xbefore.size()<2 || xbefore.size()!=ybefore.size())
        return false;
    
    int n = xbefore.size();
    
    //derivations
    std::vector<long double> m1(n+3, 0);
    for (int i=0;i<n-1;++i)
        m1[i+2] = (ybefore[i+1]-ybefore[i])/(xbefore[i+1]-xbefore[i]);
    
    m1[1] = 2*m1[2]-m1[3];
    m1[0] = 2*m1[1]-m1[2];
    m1[n+1] = 2*m1[n]-m1[n-1];
    m1[n+2] = 2*m1[n+1]-m1[n];
    
    //second derivations
    std::vector<long double> f1(n, 0), f2(n, 0), f12(n,0);
    for (int i=0;i<2;i++)
        f2[i] = fabsl(m1[i+1]-m1[i]);
    for (int i=2;i<n;i++)
    {
        f2[i] = fabsl(m1[i+1]-m1[i]);
        f1[i-2] = f2[i];
    }
    for (int i=n;i<n+2;i++)
        f1[i-2] = fabsl(m1[i+1]-m1[i]);
    
    for (int i=0;i<n;i++)
        f12[i] = f1[i]+f2[i];
    
    //interpolat
    long double max_f12 = *std::max_element(f12.begin(), f12.end());
    std::vector<long double> b(n, 0), c(n-1, 0), d(n-1, 0);
    for (int i=0;i<n;i++)
        if (f12[i]>1e-8*max_f12)
            b[i] = (f1[i]*m1[i+1]+f2[i]*m1[i+2])/f12[i];
        else
            b[i] = m1[i+1];
    
    long double dx;
    for (int i=0;i<n-1;i++)
    {
        dx = xbefore[i+1]-xbefore[i];
        c[i]=(3*m1[i+2]-2*b[i]-b[i+1])/dx;
        d[i]=(b[i]+b[i+1]-2*m1[i+2])/(dx*dx);
    }
      
    //apply
    yout.resize(xafter.size());

    int acount;
    acount = xafter.size();
    
    int j=0;
    long double px, t;
    for (int i=0;i<acount;i++)
    {
        for (;j>0 && xafter[i]<xbefore[j];j--)
            ;
        for (;j<n-2 && xafter[i]>=xbefore[j+1];j++)
            ;

        px =  (xafter[i]-xbefore[j]);

        
        t = ((px*d[j] + c[j])*px + b[j])*px + ybefore[j];

  
        yout[i] = Interpolation::round<YType>(t);
    }

    //borders check
    YType miny, maxy;
    miny = *std::min_element(ybefore.begin(), ybefore.end());
    maxy = *std::max_element(ybefore.begin(), ybefore.end());

    for (int i=0;i<acount;i++)
    {
         if (yout[i]<miny)
                 yout[i] = miny;
         if (yout[i]>maxy)
                  yout[i] = maxy;
    }

    return true;
}

template<class XType, class YType> bool Interpolation::SplineHermite(std::vector<XType> &xbefore, std::vector<YType> &ybefore, std::vector<XType> &xafter, std::vector<YType> &yout)
{    
    if (xbefore.size()<3 || xbefore.size()!=ybefore.size())
        return false;

    yout.resize(xafter.size());
    
    //coefs
    std::vector<long double> a(xbefore.size()-1), b(xbefore.size()-1), h(xbefore.size()-1);
    
    for (unsigned i=0;i<h.size();++i)
        h[i] = xbefore[i+1]-xbefore[i];
    
    long double alpha, beta, tau;
    for (unsigned i=0;i<a.size();++i)
    {
        if (i==0)
            a[i] = (ybefore[i+1]-ybefore[i])/h[i];
        else
            a[i] = ( (ybefore[i]-ybefore[i-1])/h[i-1] + (ybefore[i+1]-ybefore[i])/h[i] )/2;
                
        if (i==a.size()-1)
            b[i] = (ybefore[i+1]-ybefore[i])/h[i];
        else
            b[i] = ( (ybefore[i+1]-ybefore[i])/h[i] + (ybefore[i+2]-ybefore[i+1])/h[i+1] )/2;
        
                
        if (fabs(ybefore[i]-ybefore[i+1])<1e-6)
            a[i] = b[i] = 0;
        else
        {
            alpha = a[i]/( (ybefore[i+1]-ybefore[i])/h[i] );
            beta = b[i]/( (ybefore[i+1]-ybefore[i])/h[i] );
            
            if (fabs(alpha)<1e-6 || fabs(beta)<1e-6)
                a[i] = b[i] = 0;
            else
                if ( alpha*alpha + beta*beta > 9 )
                {
                    tau = 3/sqrt(alpha*alpha + beta*beta);
                    a[i] = tau*alpha*(ybefore[i+1]-ybefore[i])/h[i];
                    b[i] = tau*beta*(ybefore[i+1]-ybefore[i])/h[i];
                }

        }
        
//        std::cout << "coefs. " << i << " a=" << (double)a[i] << " b=" << (double)b[i] << std::endl;
    }
     
    //calculate
    long double t, t2, t3,/* h2, */h00, h10, h01, h11;
    int /*lastj,*/ j=0;
    for (unsigned i = 0; i < xafter.size(); ++i){
//        lastj = j;
        while (j > 0 && xafter[i] < xbefore[j])
            j--;
        while (j < xbefore.size() - 2 && xafter[i] >= xbefore[j+1])
            j++;

        t = (xafter[i]-xbefore[j])/h[j];
        t2 = t*t;
        t3 = t2*t;
//        h2 = h[j]*h[j];
        h00 = 2*t3 - 3*t2 + 1;
        h10 = t3 - 2*t2 + t;
        h01 = -2*t3 + 3*t2;
        h11 = t3 - t2;
        
//        std::cout << "search for: " << xafter[i] << ". pos="  << j
//                  << " t=" << t << " t2=" << t2 << " t3=" << t3 << std::endl;
//        std::cout << " h=" << h[j] << " h2="<< h2 
//                  << " h00=" << h00 << " h10=" << h10
//                  << " h01=" << h01 << " h11=" << h11 <<std::endl;

        t = h00*ybefore[j] + h10*h[j]*a[j] + h01*ybefore[j+1] + h11*h[j]*b[j];
        yout[i] = /*isconvert ? MyToInt(t) :*/ t;
        
//        std::cout << "res: " << yout[i]  << std::endl
//                  << "----------------------------------" << std::endl;
    }
    
    //borders check
    YType miny, maxy;
    miny = *std::min_element(ybefore.begin(), ybefore.end());
    maxy = *std::max_element(ybefore.begin(), ybefore.end());

    for (int i=0;i<xafter.size();i++)
    {
         if (yout[i]<miny)
                 yout[i] = miny;
         if (yout[i]>maxy)
                  yout[i] = maxy;
    }

    return true;
}

template<class XType, class YType> bool Interpolation::SplineHermite_Olga(const std::vector<XType> &x, const std::vector<YType> &y, const std::vector<XType> &xi, std::vector<YType> &yi)
{
    if (x.size() !=  y.size())
        return false;
    
    yi.resize(xi.size());

    typedef typename std::conditional<std::is_floating_point<XType>::value, XType, double>::type XT;
    typedef typename std::conditional<std::is_floating_point<YType>::value, YType, double>::type YT;
    
    for (uint i = 0; i < xi.size(); i++)
    {
        int klo = 0;
        int khi = x.size()-1;
        
        while(khi-klo > 1)
        {
            int k = (khi+klo)/2;
            if (x[k] > xi[i])
                khi = k;
            else
                klo = k;
        }
        
        XT h = x[khi] - x[klo];

        if (h == 0)
            return false;
     
        YT a,b;
        if (klo == 0)
        {
           a = static_cast<YT>(y[khi] - y[klo])/h;
           b = ( static_cast<YT>(y[khi] - y[klo])/h + static_cast<YT>(y[khi+1] - y[khi])/(x[khi+1] - x[khi]) )/2.;
        }
        else if (khi == x.size()-1)
        {
           a = ( static_cast<YT>(y[klo] - y[klo-1])/(x[klo] - x[klo-1]) + static_cast<YT>(y[khi] - y[klo])/h )/2.;
           b = static_cast<YT>(y[khi] - y[klo])/h;
        }
        else
        {
           a = ( static_cast<YT>(y[klo] - y[klo-1])/(x[klo] - x[klo-1]) + static_cast<YT>(y[khi] - y[klo])/h )/2.;
           b = ( static_cast<YT>(y[khi] - y[klo])/h + static_cast<YT>(y[khi+1] - y[khi])/(x[khi+1] - x[khi]) )/2.;
        }
        
        YT alpha, beta, tau;
        if ( y[khi] == y[klo] )
        {
            a = 0;
            b = 0;
        }
        else
        {
           alpha = a/( (y[khi] - y[klo])/h );
           beta = b/( (y[khi] - y[klo])/h );
           if (alpha == 0 || beta == 0)
           {
              a = 0;
              b = 0;
           }
           else if ( (alpha*alpha + beta*beta) > 9 )
            {
               tau = 3/sqrt(alpha*alpha + beta*beta);
               a = tau*alpha*(y[khi] - y[klo])/h;
               b = tau*beta*(y[khi] - y[klo])/h;
             }
         }
        
        YT t = (xi[i] - x[klo])/h;
        YT t2 = t*t;
        YT t3 = t2*t;
        YT h00 = 2*t3 - 3*t2 + 1;
        YT h10 = t3 - 2*t2 + t;
        YT h01 = -2*t3 + 3*t2;
        YT h11 = t3 - t2;

        yi[i] = round<YType>(h00*y[klo] + h10*h*a + h01*y[khi] + h11*h*b);
    }
    
    return true;
}

template<class XType, class YType> bool Interpolation::SplineAkima(const std::vector<XType> &xbefore, const std::vector<YType> &ybefore, const std::vector<XType> &xafter, std::vector<YType> &yout)
{
    if (xbefore.size()<5 || xbefore.size()!=ybefore.size())
        return false;

    /*
    std::cout << "Akima input" << std::endl;
    for (unsigned i=0;i<xbefore.size();i++)
        std::cout << xbefore[i] << "->" << ybefore[i] << std::endl;
    std::cout << "-----" << std::endl;
     */

    yout.resize(xafter.size());

    int bcount, acount;
    bcount = xbefore.size();
    acount = xafter.size();

    int i, j;

    //calc derivations first
    long double *w, *diff;
    long double *d;
    w = new long double[bcount];
    d = new long double[bcount];
    diff = new long double[bcount];


    for (i = 0;i<=bcount-2;i++)
        diff[i] = (ybefore[i+1]-ybefore[i])/(xbefore[i+1]-xbefore[i]);
    for (i=1;i<=bcount-2;i++)
        w[i] = fabs(diff[i]-diff[i-1]);

    for (i=2;i<=bcount-3;i++)
        if ( fabs(w[i-1] + w[i+1])>1e-3 )
            d[i] = (w[i+1]*diff[i-1]+w[i-1]*diff[i])/(w[i+1]+w[i-1]);
        else
            d[i] = ((xbefore[i+1]-xbefore[i])*diff[i-1]+(xbefore[i]-xbefore[i-1])*diff[i])/(xbefore[i+1]-xbefore[i-1]);

    long double x1, x2, y1, y2, a, b, c;

    x1 = xbefore[1]-xbefore[0];
    x2 = xbefore[2]-xbefore[0];
    y1 = ybefore[1]-ybefore[0];
    y2 = ybefore[2]-ybefore[0];
    a = (y2-x2/x1*y1)/(x2*x2-x1*x2);
    b = (y1-a*x1*x1)/x1;

    d[1] = 2*a*x1 + b;
    d[0] = b;
   /* c = d[2];
    a = (y2-c*x2-x2*x2/(x1*x1)*(y1-c*x1))/(x2*x2*x2-x1*x1*x1);
    b = (y1-c*x1-a*x1*x1*x1)/(x1*x1);

    d[1] = -(3*a*x1*x1+2*b*x1+c);
    d[0] = -(3*a*x2*x2+2*b*x2+c);*/

    x1 = xbefore[bcount-2]-xbefore[bcount-3];
    x2 = xbefore[bcount-1]-xbefore[bcount-3];
    y1 = ybefore[bcount-2]-ybefore[bcount-3];
    y2 = ybefore[bcount-1]-ybefore[bcount-3];

    c = d[bcount-3];
    a = (y2-x2/x1*y1)/(x2*x2-x1*x2);
    b = (y1-a*x1*x1)/x1;
    d[bcount-2] = 2*a*(xbefore[bcount-2]-xbefore[bcount-3])+b;
    d[bcount-1] = 2*a*(xbefore[bcount-1]-xbefore[bcount-3])+b;

    delete[] diff;
    delete[] w;


    //find coeficients by derivations
    long double *coef;
    coef = new long double[4*bcount];
    long double delta, delta2, delta3;

    for (i = 0; i <= bcount-2; i++)
    {
        delta = xbefore[i+1]-xbefore[i];
        delta2 = delta*delta;
        delta3 = delta*delta2;
        coef[4*i+0] = ybefore[i];
        coef[4*i+1] = d[i];
        coef[4*i+2] = (3*(ybefore[i+1]-ybefore[i])-2*d[i]*delta-d[i+1]*delta)/delta2;
        coef[4*i+3] = (2*(ybefore[i]-ybefore[i+1])+d[i]*delta+d[i+1]*delta)/delta3;
    }
    delete[] d;
/*
    {
        //first 2 and latest points - make as ax^2 + bx + c
        long double a, b, c;
        ParabolicCoefs<XType, YType>(xbefore[0]-xbefore[0], ybefore[0],
                                     xbefore[1]-xbefore[0], ybefore[1],
                                     xbefore[2]-xbefore[0], ybefore[2],
                                     a, b, c);
        if (a!=0 || b!=0 || c!=0)
        {
            std::cout << a << " " << b << " " << c << std::endl;

            coef[0] = c;
            coef[1] = b;
            coef[2] = a;
            coef[3] = 0;
        }

        if (bcount>3)
        {
            ParabolicCoefs<XType, YType>(xbefore[0]-xbefore[1], ybefore[0],
                                         xbefore[1]-xbefore[1], ybefore[1],
                                         xbefore[2]-xbefore[1], ybefore[2],
                                         a, b, c);
            if (a!=0 || b!=0 || c!=0)
            {
                std::cout << a << " " << b << " " << c << std::endl;

                coef[0] = c;
                coef[1] = b;
                coef[2] = a;
                coef[3] = 0;
            }
        }

        ParabolicCoefs<XType, YType>(xbefore[bcount-3]-xbefore[bcount-2], ybefore[bcount-3],
                                     xbefore[bcount-2]-xbefore[bcount-2], ybefore[bcount-2],
                                     xbefore[bcount-1]-xbefore[bcount-2], ybefore[bcount-1],
                                     a, b, c);
        if (a!=0 || b!=0 || c!=0)
        {
            std::cout << a << " " << b << " " << c << std::endl;
            
            coef[4*(bcount-2)+0] = c;
            coef[4*(bcount-2)+1] = b;
            coef[4*(bcount-2)+2] = a;
            coef[4*(bcount-2)+3] = 0;
        }
    }
 */

    //calculate points

    int lastj;
    j=0;
    long double px, t;
    for (i=0;i<acount;i++)
    {
        lastj = j;
        for (;j>0 && xafter[i]<xbefore[j];j--)
            ;
        for (;j<bcount-2 && xafter[i]>=xbefore[j+1];j++)
            ;

        px =  (xafter[i]-xbefore[j]);

        /*std::cout <<  "X=" << px << std::endl;

        std::cout << coef[4*j+3]*px*px*px << std::endl;
        std::cout << coef[4*j+2]*px*px << std::endl;
        std::cout << coef[4*j+1]*px << std::endl;
        std::cout << coef[4*j] << std::endl;
*/
        t = ((coef[4*j+3]*px+coef[4*j+2])*px+coef[4*j+1])*px+coef[4*j];

  //      std::cout << "Res=" << t << std::endl;

/*
        qDebug() << "interpolation. " << xafter[i] << "->" << double(t)
                 << ". pos=" << j << ", px=" << double(px)
                 << "coef[3]=" <<  double(coef[4*j+3])
                 << "coef[2]=" <<  double(coef[4*j+2])
                 << "coef[1]=" <<  double(coef[4*j+1])
                 << "coef[0]=" <<  double(coef[4*j]);
 */
        yout[i] = /*isconvert ? MyToInt(t) :*/ t;
    }

    delete[] coef;

    //borders check
    YType miny, maxy;
    miny = maxy = ybefore[0];
    for (i=1;i<bcount;i++)
    {
         miny = std::min<YType>(miny, ybefore[i]);
         maxy = std::max<YType>(maxy, ybefore[i]);
    }

    for (i=0;i<acount;i++)
    {
         if (yout[i]<miny)
                 yout[i] = miny;
         if (yout[i]>maxy)
                  yout[i] = maxy;
    }

    return true;
}


template<class XType, class YType> void Interpolation::ParabolicCoefs(XType x0, YType y0, XType x1, YType y1, XType x2, YType y2, long double &a, long double &b, long double &c)
{
    if (x2==x0 || x1==x0)
    {
        //2 equal points -> linear

        if (x1==x0 && x2==x0)
        {
            //stupid input :)
            a = b = c = 0;
            return;
        }

        if (x1!=x0)
        {
            a = 0;
            b = (long double)(y1-y0)/(x1-x0);
            c = y0 - b*x0;
        }
        else
        {
            a = 0;
            b = (long double)(y2-y0)/(x2-x0);
            c = y0 - b*x0;
        }

        return;
    }


    long double t;
    t = (long double)(x2+x0)*(x1-x0) - (x1*x1-x0*x0);
    if (t)
        a = ((long double)(y2-y0)*(x1-x0)/(x2-x0) - (y1-y0))/t;
    else
    {
        a = b = c =0;
        return;
    }
    //a is ready

    b = ((long double)(y1-y0) - a*(x1*x1-x0*x0))/(x1-x0);
    c = (long double)(y0) - a*x0*x0 - b*x0;
}

inline long double Interpolation::polyHarmonicBase(long double d)
{
    if (d<=0)
        return 0;
    
    return d*d*std::log(d + 1.0e-3L);
}


template<class XType> 
inline long double Interpolation::polyHarmonicBase(const std::vector<XType> &v1, const std::vector<XType> &v2)
{
    long double d=0, t;
    unsigned sz = std::min(v1.size(), v2.size());
    for (unsigned i=0;i<sz;++i)
    {
        t = v1[i]-v2[i];
        d+= t*t;
    }
    return polyHarmonicBase(sqrtl(d));
}

template<class XType, class YType> 
bool Interpolation::buildPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<std::vector<YType> > &ybefore, std::vector<std::vector<long double> > &results, double smooth/*=0.0*/)
{
    if (!ybefore.size())
    {
        qDebug() << "buildPolyHarmonic !ybefore.size()";
        return false;
    }
       
    const unsigned dim = ybefore[0].size();
    for (unsigned i=1;i<ybefore.size();++i)
        if (ybefore[i].size()!=dim)
        {
            qDebug() << "buildPolyHarmonic ybefore[i].size()!=ybefore[0].size()" << i << ybefore[i].size() << ybefore[0].size();
            return false;
        }
    
    
//    qDebug() << "--------------------------------";
//    for (int i=0;i<xbefore.size();i++)
//    {
//        QString s;
//        for (int j=0;j<xbefore[i].size();++j)
//            s += QString::number(xbefore[i][j]) + " ";
//        s += "-> ";
//        for (int j=0;j<ybefore[i].size();++j)
//            s += QString::number(ybefore[i][j]) + " ";
//        qDebug() << s;
//    }
    
    std::vector<long double> tres;
    
    results.clear();
    
    std::vector<std::vector<YType> > ys(dim);
    for (typename std::vector<std::vector<YType> >::const_iterator itr = ybefore.begin();itr!=ybefore.end();++itr)
        for (unsigned i=0;i<dim;++i)
            ys[i].push_back((*itr)[i]);
    
    for (typename std::vector<std::vector<YType> >::const_iterator itr = ys.begin();itr!=ys.end();++itr)
    {
        if (!buildPolyHarmonic(xbefore, *itr, tres, smooth))
        {
            qDebug() << "buildPolyHarmonic !buildPolyHarmonic(xbefore, *itr, tres)";
            return false;
        }
        results.push_back(tres);
//        QString s = "[";
//        for (unsigned i=0;i<tres.size();++i)
//            s+=QString::number((double)tres[i])+",";
//        s+="]";
//        qDebug() << s;
    }
    //qDebug() << "-------------------------------------";
    
    //qDebug() << "buildPolyHarmonic ok";
    
    return true;
}

template<class XType, class YType> 
bool Interpolation::buildPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<YType> &ybefore, std::vector<long double> &results, double smooth/*=0.0*/)
{
    if (xbefore.empty() || xbefore.size()!=ybefore.size())
    {
        qDebug() << "buildPolyHarmonic1 xbefore.empty() || xbefore.size()!=ybefore.size()" << xbefore.size() << ybefore.size();
        return false;
    }
    
    const unsigned N = xbefore.size();
    const unsigned nx = xbefore[0].size();
    
    //not enough points for this dimentions
    if (N<nx+1)
    {
        qDebug() << "buildPolyHarmonic1 N<nx+1" << N << nx;
        return false;
    }
    
    for (unsigned i=1;i<xbefore.size();++i)
        if (xbefore[i].size()!=nx)
        {
            qDebug() << "buildPolyHarmonic1 xbefore[i].size()!=nx" << i << xbefore[i].size() << nx;
            return false;
        }
    
    for (unsigned i=0;i<xbefore.size();++i)
        for (unsigned j=i+1;j<xbefore.size();++j)
            if (cppfunc::equal(xbefore[i], xbefore[j]))
            {
                qDebug() << "buildPolyHarmonic repeat of X's" << i << j;
                return false;
            }
    
    Matrix A(N+nx+1,N+nx+1);
    Matrix Y(N+nx+1,1);
    
    //A = | Phi V'|
    //    | V   0 |
    for (unsigned i=0;i<N;++i)
    {
        A.at(i,i) = -smooth;
        for (unsigned j=i+1;j<N;++j)
        {
            
            A.at(i,j) = polyHarmonicBase<XType>(xbefore[i], xbefore[j]);
            A.at(j,i) = A.at(i,j);
        }
    }
    for (unsigned i=0;i<N;++i)
    {
        A.at(N, i) = A.at(i, N) = 1;
        for (unsigned j=0;j<nx;++j)
            A.at(N+1+j, i) = A.at(i, N+1+j) = xbefore[i][j];
    }
    
    for (unsigned i=N;i<N+nx+1;++i)
        for (unsigned j=N;j<N+nx+1;++j)
            A.at(i,j) = 0;
    
    //Y = |y;0|
    for (unsigned i=0;i<N;++i)
        Y.at(i,0) = ybefore[i];
    for (unsigned i=N;i<N+nx+1;++i)
        Y.at(i,0) = 0;
    
    
    //R is solution of AR=Y
    //Matrix R = Matrix::Inverse(A)*Y;
    Matrix R = MatrixSolve::LU_Decomposition(A, Y);
    
    //if inverse failed
    bool allzero=true;
    for (int i=0;i<R.rows() && allzero;++i)
        for (int j=0;j<R.columns() && allzero;++j)
            if (R.at(i,j)!=0)
                allzero = false;
        
        
//    qDebug() << "A:";
//    for (int i=0;i<A.rows();i++)
//    {
//        QString tmp;
//        for (int j=0;j<A.columns();j++ )
//            tmp += QString::number((double)A.at(i,j) ) + " ";
//        qDebug() << tmp;
//    }
//    
//    qDebug() << "inv A:";
//    Matrix invA = Matrix::Inverse(A);
//    for (int i=0;i<invA.rows();i++)
//    {
//        QString tmp;
//        for (int j=0;j<invA.columns();j++)
//            tmp += QString::number((double)invA.at(i,j) ) + " ";
//        qDebug() << tmp;
//    }
//    
//    qDebug() << "coefs:";
//    for (unsigned i=0;i<N+nx+1;++i)
//        qDebug() << R.at(i,0);
            
        
    if (allzero)
    {   
        qDebug() << "buildPolyHarmonic can't inverse matrix";
        return false;
    }
    
    if ((unsigned)R.rows()!=N+nx+1 || R.columns()!=1)
    {
        qDebug() << "buildPolyHarmonic1 (unsigned)R.rows()!=N+nx+1 || R.columns()!=1" << R.rows() << N+nx+1 << R.columns() << 1;
        return false;
    }
    
    results.resize(N + nx+1, 0);
    for (unsigned i=0;i<N+nx+1;++i)
        results[i] = R.at(i,0);
    
    
     //qDebug() << "buildPolyHarmonic1 ok";
    
    return true;
}

template<class XType, class YType>
bool Interpolation::ApplyPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<std::vector<long double> > &coefs, 
                                      const std::vector<XType> &x, std::vector<YType> &result)
{
    if (!coefs.size())
    {
        qDebug() << "ApplyPolyHarmonic !coefs.size()";
        return false;
    }
    
    YType tres;
    
    result.clear();
    for (std::vector<std::vector<long double> >::const_iterator itr=coefs.begin();itr!=coefs.end();++itr)
    {
        if (!ApplyPolyHarmonic(xbefore, *itr, x, tres))
        {
            qDebug() << "ApplyPolyHarmonic !ApplyPolyHarmonic(xbefore, *itr, x, tres)";
            return false;
        }
        result.push_back(tres);
    }
    
    //qDebug() << "ApplyPolyHarmonic ok";
    
    return true;
}
    
template<class XType, class YType> 
bool Interpolation::ApplyPolyHarmonic(const std::vector<std::vector<XType> > &xbefore, const std::vector<long double> &coefs, 
                                      const std::vector<XType> &x, YType &result)
{
    if (!xbefore.size())
    {
        qDebug() << "ApplyPolyHarmonic1 !xbefore.size()";
        return false;
    }
    
    const unsigned N = xbefore.size();
    const unsigned nx = xbefore[0].size();
    
    //not enough points for this dimentions
    if (N<nx+1)
    {
        qDebug() << "ApplyPolyHarmonic1 N<nx+1" << N << nx;
        return false;
    }
    
    for (unsigned i=1;i<xbefore.size();++i)
        if (xbefore[i].size()!=nx)
        {
            qDebug() << "ApplyPolyHarmonic1 xbefore[i].size()!=nx" << xbefore[i].size() << nx;
            return false;
        }
    
    if (x.size()!=nx)
    {
        qDebug() << "ApplyPolyHarmonic1 x.size()!=nx" << x.size() << nx;
        return false;
    }
    
    if (coefs.size()!=N + nx+1)
    {
        qDebug() << "ApplyPolyHarmonic1 coefs.size()!=N + nx+1" <<coefs.size() << N << nx;
        return false;
    }
    
    long double res = 0;
    //sum Wi*Phi(||Ci-X||)
    for (unsigned i=0;i<N;++i)
        res += coefs[i]*polyHarmonicBase<XType>(xbefore[i], x);
    
    //+v'*[1;x]
    res += coefs[N];
    for (unsigned i=0;i<nx;++i)
        res += coefs[N+1 + i]*x[i];
    
    result = round<YType>(res);
    
    //qDebug() << "ApplyPolyHarmonic1 ok";
    
    return true;
}

#endif	/* INTERPOLATION_H */

