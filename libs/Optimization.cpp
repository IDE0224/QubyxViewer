#include "Optimization.h"
#include "cppfunc.h"

#include "Matrix.h"
#include "Interpolation.h"
#include "Regression.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <type_traits>

#include <QDebug>
//#include <QString>
#include <iostream>
#include <sstream>
#include <fstream>
#include <locale>


template<class T> T Optimization<T>::minvalue_ = 0;
template<class T> T Optimization<T>::maxvalue_ = 255;

template<class T>
T Optimization<T>::round(long double x)
{
    if (std::is_integral<T>::value)
        return x>0 ? x+.5 : x-.5;
    
    return x;
}

template<class T>
void Optimization<T>::setMinMaxValues(T minvalue, T maxvalue)
{
    minvalue_ = minvalue;
    maxvalue_ = maxvalue;
}


template<class T>
void Optimization<T>::fixpoint(std::vector<T> &point)
{
    fixpoint(point, minvalue_, maxvalue_);
}

template<class T>
void Optimization<T>::fixpoint(std::vector<T> &point, T minvalue, T maxvalue)
{
    T mmin, mmax;
    int minpos, maxpos;
    unsigned i;

    minpos = std::min_element(point.begin(), point.end()) - point.begin();
    mmin = point[minpos];

    if (mmin<minvalue)
        for (i=0;i<point.size();i++)
            point[i] += (minvalue-mmin);

    maxpos = std::max_element(point.begin(), point.end()) - point.begin();
    mmax = point[maxpos];

    if (mmax>maxvalue)
    {
        double coef = double(maxvalue-minvalue)/(mmax-minvalue);
        for (i=0;i<point.size();i++)
            point[i] = minvalue + round(coef*(point[i]-minvalue));
    }
}

template<class T>
void Optimization<T>::fixpointCut(std::vector<T> &point)
{
    fixpointCut(point, minvalue_, maxvalue_);
}

template<class T>
void Optimization<T>::fixpointCut(std::vector<T> &point, T minvalue, T maxvalue)
{
    for (unsigned i=0;i<point.size();i++)
    {
        if (point[i]<minvalue)
            point[i] = minvalue;
        if (point[i]>maxvalue)
            point[i] = maxvalue;
    }
}

template<class T>
T Optimization<T>::increaseOnDelta(T value, long double delta, int &sign)
{
//    T res = value - delta;
//    sign = -1;
//    
//    if (res<minvalue_ )
//    {
//        res = value + delta;
//        sign = 1;
//    }
//    
//    return res;
    
    //delta *= 1.+(rand()%101-50)/1000.;
    
    T res = value + delta;
    sign = 1;
    
    if (res>maxvalue_ )
    {
        res = value - delta;
        sign = -1;
    }
    
    return res;
}

template<class T>
bool Optimization<T>::any_of(std::vector<T> a, std::function<bool(T)> func)
{
    for (auto x : a)
    {
        if (func(x)) 
            return true;
    }
    return false;
}

template<class T>
bool Optimization<T>::IsClosest(NearestType nearestType, double value, double limit)
{
    switch (nearestType)
    {
        case NearestType::NearestUpper: return value >= 0.0 && std::abs(value) < std::abs(limit);
        case NearestType::NearestLower: return value <= 0.0 && std::abs(value) < std::abs(limit);
        case NearestType::Nearest:      return std::abs(value) < std::abs(limit);
        default: return false;
    }
    
    return false;
}


template<class T>
bool Optimization<T>::RosenbrockSearch(std::vector<T> &startpoint, std::vector<T> &resultpoint, SearchFunction* func, long double searchresult, long double enoughresult, long double &result, int iterationcount)
{
    //!!debug comment
    //QString out;
    std::stringstream ss;
    
    int startfixed=0, fixed = 0;
    int psize = startpoint.size();

    std::vector< std::vector<double> > axis(2);
    axis[0].resize(psize);
    axis[1].resize(psize);

    std::vector<T> keypoint(psize);
    std::vector<double> tjump(psize, 0);
    std::vector<T> start(psize);
    std::vector<T> curr(psize);
    std::vector<T> jump(psize);

    long double start_res, curr_res, jump_res;

    keypoint.assign(startpoint.begin(), startpoint.end());
    if (!func->value(keypoint, start_res))
            return false;

    if (start_res<=/*enoughresult*/searchresult)
    {
        //!!debug comment
        /*qDebug() << "-----------------------------";
        qDebug() << "Rosenbrock search";
        for (int t=0;t<psize;t++)
            out += QString::number(startpoint[t]) + (t==psize-1 ? "" : ",");
        qDebug() << "Start position is good enough: (" << out << ")=" << double(start_res);
        qDebug() << "-----------------------------";
         */ 
        /*
        std::cout << "------------" << std::endl; 
        ss.clear();
        ss.str("");
        for (int t=0;t<psize;t++)
            ss << startpoint[t] << (t==psize-1 ? "" : ",");
        std::cout << "Start position is good enough: (" << ss.str() << ") =" << double(start_res) << std::endl; 
         */ 

        result = start_res;
        resultpoint.assign(startpoint.begin(), startpoint.end());
        return true;
    }

    //!!debug comment
    /*
    qDebug() << "-----------------------------";
    qDebug() << "Rosenbrock search";
    for (int t=0;t<psize;t++)
        out += QString::number(startpoint[t]) + (t==psize-1 ? "" : ",");
    qDebug() << "Start position: (" << out << ")=" << double(start_res);
     */ 
    /*
    std::cout << "------------" << std::endl; 
    ss.clear();
    ss.str("");
    for (int t=0;t<psize;t++)
        ss << startpoint[t] << (t==psize-1 ? "" : ",");
    std::cout << "Start position (" << ss.str() << ") =" << double(start_res) << std::endl; 
      */

    for (int i=3;i<=psize;i++)
        axis[0][(fixed+i)%psize] = axis[1][(fixed+i)%psize] = 0;
    
    axis[0][(fixed+1)%psize] = 3.*(maxvalue_-minvalue_)/255;
    axis[0][(fixed+2)%3] = 3.*(maxvalue_-minvalue_)/255;
    axis[1][(fixed+1)%psize] = -3.*(maxvalue_-minvalue_)/255;
    axis[1][(fixed+2)%3] = 3.*(maxvalue_-minvalue_)/255;

    int i=0;
    while(start_res>searchresult && i++<iterationcount)
    {
        start.assign(keypoint.begin(), keypoint.end());


            for (int j=0;j<2 && start_res>searchresult;j++)
            {
                               
                for (int k=0;k<psize;k++)
                    curr[k] = start[k] + axis[j][k];

                fixpoint(curr);

                if (!(func->value(curr, curr_res)))
                        return false;

                //!!debug comment
                /*
                out = "";
                for (int t=0;t<psize;t++)
                    out += QString::number(curr[t]) + (t==psize-1 ? "" : ",");
                qDebug() << j << "axis probe: dE(" << out << ")=" << double(curr_res);
                 */ 
               /*
                ss.clear();
                ss.str("");
                for (int t=0;t<psize;t++)
                    ss << curr[t] << (t==psize-1 ? "" : ",");
                std::cout << "axis probe: dE(" << ss.str() << ")=" << double(curr_res) << std::endl; 
                */

                if (fabs(start_res-curr_res)>searchresult/2)
                {
                    if (curr_res<start_res)
                        for (int k=0;k<psize;k++)
                        {
                            if (curr_res<enoughresult)
                                tjump[k] = .7*(curr[k]-start[k])*sqrt(start_res)/(sqrt(start_res)-sqrt(curr_res));
                            else
                                tjump[k] = 3*(curr[k]-start[k])*sqrt(start_res)/(sqrt(start_res)-sqrt(curr_res));
                            //tjump[k] = .5*(curr[k]-start[k])*start_de/(start_de-curr_de);
                            //tjump[k] = 1.5*(curr[k]-start[k]);
                        }
                    else
                        for (int k=0;k<psize;k++)
                        {
                            if (start_res<enoughresult)
                                tjump[k] = -.25*(curr[k]-start[k])*start_res/(curr_res-start_res);
                            else
                                tjump[k] = -2*(curr[k]-start[k])*start_res/(curr_res-start_res);
                            //tjump[k] = -.25*(curr[k]-start[k]);
                        }
                }
                else
                    for (int k=0;k<psize;k++)
                        tjump[k] = (curr[k]-start[k])/2.;

                fixpoint(jump, -15.*(maxvalue_-minvalue_)/255, 15.*(maxvalue_-minvalue_)/255);
                for (int k=0;k<3;k++)
                    if (abs(tjump[k])>10.*(maxvalue_-minvalue_)/255)
                        tjump[k] = tjump[k]>0 ? 10 : -10;


                double mjump = fabs(tjump[0]);
                for (int k=1;k<psize;k++)
                    mjump = std::max<double>(mjump, fabs(tjump[k]));
                if (round(mjump)==0 && mjump>0)
                    for (int k=0;k<psize;k++)
                        tjump[k] /= mjump;
                else
                    if (mjump>10.*(maxvalue_-minvalue_)/255)
                        for (int k=0;k<psize;k++)
                            tjump[k] /= mjump/(10.*(maxvalue_-minvalue_)/255);


                for (int k=0;k<psize;k++)
                    jump[k] = start[k] + round(tjump[k]);

                fixpoint(jump);

                if (!func->value(jump, jump_res))
                        return false;

                //!!debug comment
                /*
                out = "";
                for (int t=0;t<psize;t++)
                    out += QString::number(jump[t]) + (t==psize-1 ? "" : ",");
                qDebug() << j << "axis jump: dE(" << out << ")=" << double(jump_res);
                 */ 
                /*
                ss.clear();
                ss.str("");
                for (int t=0;t<psize;t++)
                    ss << jump[t] << (t==psize-1 ? "" : ",");
                std::cout << "axis jump: dE(" << ss.str() << ")=" << double(jump_res) << std::endl; 
                  */

                if (jump_res<curr_res && jump_res<start_res)
                {
                    for (int k=0;k<psize;k++)
                        start[k] = jump[k];
                    start_res = jump_res;
                }
                else
                    if (curr_res<start_res)
                    {
                        for (int k=0;k<psize;k++)
                            start[k] = curr[k];
                        start_res = curr_res;
                    }

                result = start_res;

                //!!debug comment
                /*
                out = "";
                for (int t=0;t<psize;t++)
                    out += QString::number(start[t]) + (t==psize-1 ? "" : ",");
                qDebug() << "Choosed point: dE(" << out << ")=" << double(start_res);
                 */ 
                /*
                ss.clear();
                ss.str("");
                for (int t=0;t<psize;t++)
                    ss << start[t] << (t==psize-1 ? "" : ",");
                std::cout << "Choosed point: dE(" << ss.str() << ")=" << double(start_res) << std::endl; 
                  */
            }

            bool diff = false;
            for (int k=0;k<psize;k++)
                diff = diff || (keypoint[k]!=start[k]);

            

            if (!diff)
            {
                    if (start_res>enoughresult)
                    {
                            //!!debug comment
                            //qDebug() << "No difference in result, but bad dE - change fixed point";
                                //std::cout << "No difference in result, but bad dE - change fixed point" << std::endl; 

                            fixed++;
                            fixed %= psize;

                            if (fixed==startfixed)
                                    break;

                            for (int i=3;i<=psize;i++)
                                axis[0][(fixed+i)%psize] = axis[1][(fixed+i)%psize] = 0;

                            axis[0][(fixed+1)%psize] = 3.*(maxvalue_-minvalue_)/255;
                            axis[0][(fixed+2)%3] = 3.*(maxvalue_-minvalue_)/255;
                            axis[1][(fixed+1)%psize] = -3.*(maxvalue_-minvalue_)/255;
                            axis[1][(fixed+2)%3] = 3.*(maxvalue_-minvalue_)/255;
                    }
                    else
                    {
                        //!!debug comment
                            //qDebug() << "Stop as no difference";
                        //std::cout << "Stop as no difference" << std::endl; 
                            break;
                    }
            }
            else
            {

                    //calc axis

                    //1 axis is a movement
                    for (int k=0;k<psize;k++)
                            axis[0][k] = start[k] - keypoint[k];

                    double mjump = fabs(axis[0][0]);
                    for (int k=0;k<psize;k++)
                        mjump = std::max<double>(mjump, fabs(axis[0][k]));
                    if (mjump>3)
                        for (int k=0;k<psize;k++)
                            axis[0][k] /= mjump/3;

                    //2 axis ortogonal
                    {
                        axis[1][fixed] = 0;

                        int a,b;
                        a = axis[0][(fixed+1)%3];
                        b = axis[0][(fixed+2)%3];

                        if (b==0)
                        {
                            axis[1][(fixed+1)%3] = 0;
                            axis[1][(fixed+2)%3] = 2.*(maxvalue_-minvalue_)/255;
                        }
                        else
                        {
                            double c, d;
                            c = 1;
                            d = -a/double(b);

                            if (d<1 && d>0.2)
                            {
                                c *= 1/d;
                                d = 1;
                            }

                            if (d>5)
                            {
                                c *= 5./d;
                                d = 5;
                            }

                            axis[1][(fixed+1)%3] = round(c);
                            axis[1][(fixed+2)%3] = round(d);
                        }
                    }

            }


            for (int k=0;k<psize;k++)
                    keypoint[k]=start[k];
           
    }

    resultpoint.assign(keypoint.begin(), keypoint.end());

    //!!debug comment
    /*
    out = "";
    for (int t=0;t<psize;t++)
        out += QString::number(resultpoint[t]) + (t==psize-1 ? "" : ",");
    qDebug() << "Rosenbrock result." << i << "iterations. (" << out << ")=" << double(start_res);
    qDebug() << "-----------------------------";
     */ 
    /*
    ss.clear();
    ss.str("");
    for (int t=0;t<psize;t++)
        ss << resultpoint[t] << (t==psize-1 ? "" : ",");
    std::cout << "Rosenbrock result. " << i << " iterations. (" << ss.str() << ")=" << double(start_res) << std::endl; 
      */  
    return true;
}

template<class T>
bool Optimization<T>::NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                                   std::function<bool(const std::vector<T> &, std::vector<long double> &)> func,
                                   long double enoughresult, long double &result, int iterationcount,
                                   T increaseDelta /*= 1.0e-6*/)
{
    auto errFunc = [] (const std::vector<T> &v, const std::vector<long double> &axisErr)
    {
        long double res = 0.0;
        for (auto err : axisErr)
            res += err*err;
        return std::sqrt(res);
    };
    auto addErrFunc = [] (const std::vector<T> &v) ->long double {return 0;};
    
    return Optimization<T>::NewtonSearch(startpoint, resultpoint, 
                        func, errFunc, addErrFunc,
                        enoughresult, result, iterationcount, increaseDelta);
}

template<class T>
bool Optimization<T>::NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                                   std::function<bool(const std::vector<T> &, std::vector<long double> &)> func,
                                   std::function<long double(const std::vector<T> &)> addErrFunc,
                                   long double enoughresult, long double &result, int iterationcount,
                                   T increaseDelta /*= 1.0e-6*/)
{
    auto errFunc = [] (const std::vector<T> &v, const std::vector<long double> &axisErr)
    {
        long double res = 0.0;
        for (auto err : axisErr)
            res += err*err;
        return std::sqrt(res);
    };
    
    return Optimization<T>::NewtonSearch(startpoint, resultpoint, 
                        func, errFunc, addErrFunc,
                        enoughresult, result, iterationcount, increaseDelta);
}


template<class T>
bool Optimization<T>::NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> axisErrFunc,
                             std::function<long double(const std::vector<T> &, const std::vector<long double> &)> errFunc,
                             std::function<long double(const std::vector<T> &)> addErrFunc,
                             long double enoughresult, long double &result, int iterationcount/*=100*/,
                             T increaseDelta/* = 1.0e-6*/)
{
//    bool debug = false && std::abs(startpoint[0]) < 1.0e-3 && std::abs(startpoint[1] - 1.0) < 1.0e-3 && std::abs(startpoint[2]) < 1.0e-3;
//    if (debug)
//        qDebug() << "NewtonSearch 0 1 0";
    
    
    std::stringstream ss;
    
#ifdef _DEBUG    
    std::ofstream f("matrices.txt", std::ios::out | std::ios::app);
    f << "-----------------------------------" << std::endl;
    f << "startpoint: ";
    for (unsigned i=0;i<startpoint.size();i++)
        f << startpoint[i] << " ";
    f << std::endl;
#endif
        
    int sz = startpoint.size();
    std::vector<T> currpoint(startpoint), lastpoint(startpoint);
    std::vector<long double> lastError, currError;
    //long double e= 1e-6*(maxvalue_-minvalue_), terr;
    long double e= increaseDelta*(maxvalue_-minvalue_), terr;
    int sig;
    Matrix J(sz, sz), Diff(sz, 1), R;
    int iterations;
    
    resultpoint = startpoint;
#ifdef _DEBUG        
        {
            f << "[";
            f.flush();
        }
#endif 
    if (!axisErrFunc(startpoint, lastError))
    {
        qDebug() << "!!!!!!!!!!! newton search failed because of axisErrFunc"; 
        return false;
    }
#ifdef _DEBUG        
        {
            f << "]"<<std::endl;
            f.flush();
        }
#endif     
//    result = 0;
//    for (int r=0;r<sz;++r)
//        result +=  lastError[r]*lastError[r];
//    result = sqrtl(result) + addErrFunc(startpoint);
    result = errFunc(startpoint, lastError) + addErrFunc(startpoint);
    
//#ifdef _DEBUG        
//        {
//            f << "chosen point=[";
//            for (unsigned i=0;i<startpoint.size();i++)
//                f << startpoint[i] << (i==startpoint.size()-1 ? "] " : ",");
//            f << "error function=[";
//            for (unsigned i=0;i<lastError.size();i++)
//                f << lastError[i] << (i==lastError.size()-1 ? "] " : ",");
//            f << std::endl;
//        }
//#endif     
    
#ifdef _DEBUG        
        {
            f << "Start. Error = " << result << ", lastpoint=[";
            for (unsigned i=0;i<startpoint.size();i++)
                f << startpoint[i] << (i==startpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif               
    
    std::vector<std::vector<T>> oldPoints = {startpoint};

    for (iterations=0;iterations<iterationcount && result>enoughresult;++iterations)
    {
//        if (debug) qDebug() << "iteration:" << iterations;
        //calc jacobian
        for (int r=0;r<sz;++r)
        {

            bool repeat=true;
            for (int m=1;m<=10 && repeat;++m)
            {

                currpoint = lastpoint;
                currpoint[r] = increaseOnDelta(lastpoint[r], e*m, sig);
    //            qDebug() << "currpoint:" << (double)currpoint[0] << (double)currpoint[1] << (double)currpoint[2]
    //                     << "lastpoint:" << (double)lastpoint[0] << (double)lastpoint[1] << (double)lastpoint[2];

    //#ifdef _DEBUG
    //            f << r << " " << currpoint[r]-lastpoint[r] << " " << sig << std::endl;
    //#endif
                axisErrFunc(currpoint, currError);


    //#ifdef _DEBUG
    //        {
    //            f << "test point=[";
    //            for (unsigned i=0;i<currpoint.size();i++)
    //                f << currpoint[i] << (i==currpoint.size()-1 ? "] " : ",");
    //            f << "error function=[";
    //            for (unsigned i=0;i<currError.size();i++)
    //                f << currError[i] << (i==currError.size()-1 ? "] " : ",");
    //            f << std::endl;
    //        }
    //#endif

    //#ifdef _DEBUG
    //        {
    //            f << "error=[";
    //            for (unsigned i=0;i<currError.size();i++)
    //                f << currError[i] << (i==currError.size()-1 ? "]" : ",");
    //            f << " => ";//std::endl;
    //        }
    //#endif

                for (int i=0;i<sz;i++)
                {
                    if (fabs(currError[i]-lastError[i])>1e-12)
                        repeat = false;

                    J.at(i, r) = sig*(currError[i]-lastError[i])/(e*m);
                }
            }
            
//#ifdef _DEBUG        
//        {
//            if (iterations==0)
//            {
//                f << "J=[";
//                for (unsigned i=0;i<sz;i++)
//                    f << J.at(i, r) << "=" << sig << "*("<<currError[i]<<"-"<<lastError[i]<<")/"<<e << (i==currError.size()-1 ? "]" : "\t");
//                f << std::endl;
//            }
//        }
//#endif              
            
//#ifdef _DEBUG              
//            f <<"J " << J.at(0, r) << std::endl;
//#endif
        }
        
//        if (debug) qDebug() << "J:";
//        for (int i=0;i<3;i++)
//            if (debug) qDebug() << (double)J.at(i, 0) << (double)J.at(i, 1) << (double)J.at(i, 2);
        
        for (int i=0;i<sz;i++)
            Diff.at(i,0) = lastError[i];
            
        //find move
        //R = Matrix::Inverse(J)*Diff;
        R = MatrixSolve::LU_Decomposition(J, Diff);
        
//        qDebug() << "R:" << R.at(0, 0) << R.at(1, 0) << R.at(2, 0);
//        qDebug() << "J1:" << J.at(0, 0) << J.at(0, 1) << J.at(0, 2);
//        qDebug() << "J2:" << J.at(1, 0) << J.at(1, 1) << J.at(1, 2);
//        qDebug() << "J3:" << J.at(2, 0) << J.at(2, 1) << J.at(2, 2);
        
//#ifdef _DEBUG        
//        {
//            Matrix inv = Matrix::Inverse(J);
//            f << "J: " << std::endl;
//            for (unsigned i=0;i<J.rows();i++)
//            {
//                for (unsigned j=0;j<J.columns();j++)
//                        f << J.at(i,j) << " ";
//                f << std::endl;      
//            }
//            
//            f << "J^-1: " << std::endl;
//            for (unsigned i=0;i<inv.rows();i++)
//            {
//                for (unsigned j=0;j<inv.columns();j++)
//                        f << inv.at(i,j) << " ";
//                f << std::endl;      
//            }
//        }
//#endif
        
        if (R.isNull())
        {
#ifdef _DEBUG               
            f << "R.isNull()" << std::endl;
#endif            
            R = Matrix(sz,1);
            for (int r=0;r<sz;++r)
                R.at(r, 0) = 0;
        }

        
        
        
        /*
        ss.clear();
        ss.str("");
        for (int t=0;t<sz;t++)
                ss << R.at(t, 0) << (t==sz-1 ? "" : ",");
        std::cout << "R (" << ss.str() << ")" << std::endl; 
        */  
        
        //calculate new point
        currpoint = lastpoint;
        double coef = 1;
//        for (int r=0;r<sz;++r)
//            if (fabs(R.at(r,0))>(maxvalue_-minvalue_)/4)
//                coef = std::max<double>(coef, fabs(R.at(r,0))*10/(maxvalue_-minvalue_));
        
        {
            /*
            bool allTooBig=true;
            while(allTooBig)
            {
                for (int r=0;r<sz;++r)
                    allTooBig = allTooBig && fabs(R.at(r,0)/coef)>maxvalue_-minvalue_;
                if (allTooBig)
                    coef *=10;
            }*/
            /*
            bool anyTooBig=true;
            while(anyTooBig)
            {
                anyTooBig = false;
                for (int r=0;r<sz;++r)
                    if (R.at(r,0)>0)
                    {
                        if (lastpoint[r]>=minvalue_+1e-6 && lastpoint[r]-R.at(r,0)/coef<=minvalue_)
                            anyTooBig = true;
                    }
                    else
                    {
                        if (lastpoint[r]<=maxvalue_- 1e-6 && lastpoint[r]-R.at(r,0)/coef>=maxvalue_)
                            anyTooBig = true;
                    }
                    
                if (anyTooBig)
                    coef *=2;
            }*/
        }
        
        for (int r=0;r<sz;++r)
            lastpoint[r] -= R.at(r,0)/coef;
        
//        if (debug) qDebug() << "R:" << (double)R.at(0,0) << (double)R.at(1,0) << (double)R.at(2,0);
        
//#ifdef _DEBUG        
//        {
//            f << "point=[";
//            for (unsigned i=0;i<lastpoint.size();i++)
//                f << lastpoint[i] << (i==lastpoint.size()-1 ? "]" : ",");
//            f << std::endl;
//        }
//#endif           
        
        fixpointCut(lastpoint);
        
        
//        bool same=true;
//        for (int r=0;r<sz && same;++r)
//            if (fabs(currpoint[r]-lastpoint[r])>1e-12)
//                same = false;
//        
//        if (same)
//        {
//            
//#ifdef _DEBUG        
//            f << "~~~" << std::endl;
//#endif    
//                
//            if (rand()%2)
//            {
//                lastpoint = resultpoint;
//                for (int r=0;r<sz;r++)
//                    lastpoint[r] += (rand()%20001/10000.-1.)*0.01/(maxvalue_-minvalue_);
//                fixpointCut(lastpoint);
//            }
//            else
//            {
//                lastpoint = currpoint;
//                double coef = 1;
//                for (int r=0;r<sz;++r)
//                    if (fabs(R.at(r,0))>(maxvalue_-minvalue_)/4)
//                        coef = std::max<double>(coef, fabs(R.at(r,0))*100/(maxvalue_-minvalue_));
//                for (int r=0;r<sz;++r)
//                    lastpoint[r] -= R.at(r,0)/coef;
//                fixpoint(lastpoint);
//            }
//        }
                       
//        {
//            long double d = 0;
//            for (int r=0;r<sz && d<1e-9;++r)
//                d = std::max<long double>(d, fabsl(currpoint[r]-lastpoint[r]));
//            
//            if (d<1e-9)
//                break;
//        }
        
        axisErrFunc(lastpoint, lastError);
        
//#ifdef _DEBUG        
//        {
//            f << "chosen point=[";
//            for (unsigned i=0;i<lastpoint.size();i++)
//                f << lastpoint[i] << (i==lastpoint.size()-1 ? "] " : ",");
//            f << "error function=[";
//            for (unsigned i=0;i<lastError.size();i++)
//                f << lastError[i] << (i==lastError.size()-1 ? "] " : ",");
//            f << std::endl;
//        }
//#endif          
        
        terr = errFunc(lastpoint, lastError) + addErrFunc(lastpoint);
//        terr = 0;
//        for (int r=0;r<sz;++r)
//            terr +=  lastError[r]*lastError[r];
//        terr = sqrtl(terr); + addErrFunc(lastpoint);
        if (terr<result)
        {
            resultpoint = lastpoint;
            result = terr;
        }

        bool loopFound = false;
        for (auto v: oldPoints)
        {
            bool f = true;
            for (int i=0;i<v.size() && f;i++)
                if (std::abs(v[i] - lastpoint[i]) > 1e-15)
                    f = false;
            if (f)
                loopFound = true;
        }

        if (loopFound)
        {
            //qDebug() << "loop found!!!";
            break;
        }
        oldPoints.push_back(lastpoint);
          
//        if (debug) qDebug() << "lastpoint:" << (double)lastpoint[0] << (double)lastpoint[1] << (double)lastpoint[2] <<
//                "  error:" << (double)terr;
        
#ifdef _DEBUG        
        {
            f << "Error = " << terr << ", point'=[";
            for (unsigned i=0;i<resultpoint.size();i++)
                f << lastpoint[i] << (i==resultpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif  
        
//#ifdef _DEBUG        
//        {
//            f << "It = " << iterations << ", Error = " << terr << ", lastpoint=[";
//            for (unsigned i=0;i<lastpoint.size();i++)
//                f << lastpoint[i] << (i==lastpoint.size()-1 ? "]" : ",");
//            f << std::endl;
//        }
//#endif           
        
        /*
        ss.clear();
        ss.str("");
        for (int t=0;t<lastpoint.size();t++)
                ss << lastpoint[t] << (t==lastpoint.size()-1 ? "" : ",");
        std::cout << "Newton (" << ss.str() << ")=";
        
        ss.clear();
        ss.str("");
        for (int t=0;t<lastError.size();t++)
                ss << lastError[t] << (t==lastError.size()-1 ? "" : ",");
        std::cout << ss.str() << std::endl; 
        */
    }
    
    /*
    ss.clear();
    ss.str("");
    for (int t=0;t<resultpoint.size();t++)
        ss << resultpoint[t] << (t==resultpoint.size()-1 ? "" : ",");
    std::cout << "Newton result. " << iterations << " iterations. (" << ss.str() << ")=" << result << std::endl; 
    */
    
#ifdef _DEBUG        
        {
            f << "Result. Error = " << result << ", point=[";
            for (unsigned i=0;i<resultpoint.size();i++)
                f << resultpoint[i] << (i==resultpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif     
    
    //return result<=enoughresult;
    return true;
}

bool nextpoint(std::vector<int>& point, int maxit)
{
    int i;
    for (i=(int)point.size()-1;i>=0;--i)
        if (point[i]<maxit-1)
            break;
        else
            point[i] = 0;
    
    if (i<0)
        return false;
    
    ++point[i];
    return true;
}

template<class T>
bool Optimization<T>::NewtonSearchWithPerturbation(const std::vector<T> &startpoint, std::vector<T> &resultpoint, SearchMultiDimensionFunction* func,
                                 long double enoughresult, long double &result, int iterationcount/*=100*/, double perturbationMaxDelta/*=.5*/, int perturbationCount/*=10*/)
{
    if (NewtonSearch(startpoint, resultpoint, 
                    [func] (const std::vector<T> &point, std::vector<long double> &res)
                    {
                        return func->value(point, res);
                    }, 
                    enoughresult, result, iterationcount))
        return true;
    
    qDebug() << "NewtonSearchWithPerturbation. first point=" << 
            (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << 
            " error=" << (double)result;
    
    double pert = perturbationMaxDelta/perturbationCount;
    int sz = startpoint.size();
    std::vector<T> point,currpoint;
    std::vector<long double> lastError;
    
    long double err = result;
    currpoint = resultpoint;
    
    std::vector<int> pt(sz, 0);
    std::vector<T> diff(sz, 0);
    do
    {
        for (int r=0;r<sz;r++)
            diff[r] = 2*pert*pt[r]/double(perturbationCount-1)-pert;
        
        //add 
        point = currpoint;
        for (int r=0;r<sz;r++)
            point[r] += diff[r];
        fixpointCut(point);
    
        func->value(point, lastError);
        err = 0;
        for (int r=0;r<sz;++r)
            err +=  powl(lastError[r],2);
        err = sqrtl(err);

        if (err<result)
        {
            resultpoint = point;
            result = err;
            qDebug() << "NewtonSearchWithPerturbation. new point=" << 
                    (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << 
                    " error=" << (double)result;
        }
        
        //sub 
        point = currpoint;
        for (int r=0;r<sz;r++)
            point[r] -= diff[r];
        fixpointCut(point);
    
        func->value(point, lastError);
        err = 0;
        for (int r=0;r<sz;++r)
            err +=  powl(lastError[r],2);
        err = sqrtl(err);

        if (err<result)
        {
            resultpoint = point;
            result = err;
            qDebug() << "NewtonSearchWithPerturbation. new point=" << 
                    (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << 
                    " error=" << (double)result;
        }
    }while(nextpoint(pt, perturbationCount));
    
    
//    for (int i=0;i<perturbationCount && result>enoughresult;++i)
//    {
//        //add 
//        point = currpoint;
//        for (int r=0;r<sz;r++)
//            point[r] += (i+1)*pert;
//        fixpointCut(point);
//    
//        func->value(point, lastError);
//        err = 0;
//        for (int r=0;r<sz;++r)
//            err +=  powl(lastError[r],2);
//        err = sqrtl(err);
//
//        if (err<result)
//        {
//            resultpoint = point;
//            result = err;
//            qDebug() << "NewtonSearchWithPerturbation. new point=" << 
//                    (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << 
//                    " error=" << (double)result;
//        }
//        
//        //sub 
//        point = currpoint;
//        for (int r=0;r<sz;r++)
//            point[r] -= (i+1)*pert;
//        fixpointCut(point);
//    
//        func->value(point, lastError);
//        err = 0;
//        for (int r=0;r<sz;++r)
//            err +=  powl(lastError[r],2);
//        err = sqrtl(err);
//
//        if (err<result)
//        {
//            resultpoint = point;
//            result = err;
//            qDebug() << "NewtonSearchWithPerturbation. new point=" << 
//                    (double)resultpoint[0] << (double)resultpoint[1] << (double)resultpoint[2] << 
//                    " error=" << (double)result;
//        }
//    }
    
    return result<=enoughresult;
}

template<class T>
T linearBinarySearch(T left, T right, std::function<double(T)> value, double epsilon)
{
    double leftv = value(left);
    double rightv = value(right);
    
    if (leftv*rightv>0)
        return left;
    
    T center;
    double centerv;
    while(fabs(left-right)>epsilon)
    {
        center = (left+right)/2;
        centerv = value(center);
        
        if (centerv*leftv>0)
        {
            left = center;
            leftv = centerv;
        }
        else
        {
            right = center;
            rightv = centerv;
        }
    }
    
    return (left+right)/2;
}

template<class T>
bool Optimization<T>::NewtonDeepSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint,
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> func,
                             long double enoughresult, long double &result, int iterationcount/*=100*/)
{

#ifdef _DEBUG    
    std::ofstream f("matrices_deep.txt", std::ios::out | std::ios::app);
    f << "-----------------------------------" << std::endl;
    f << "startpoint: ";
    for (unsigned i=0;i<startpoint.size();i++)
        f << startpoint[i] << " ";
    f << std::endl;
#endif
        
    int sz = startpoint.size();
    std::vector<T> currpoint(startpoint), lastpoint(startpoint);
    std::vector<long double> lastError, currError;
    long double e= 1e-6*(maxvalue_-minvalue_), terr;
    int sig;
    Matrix J(sz, sz), Diff(sz, 1), R;
    int iterations;
    
    resultpoint = startpoint;
    if (!func(startpoint, lastError))
        return false;
    result = 0;
    for (int r=0;r<sz;++r)
        result +=  lastError[r]*lastError[r];
    result = sqrtl(result);
    
#ifdef _DEBUG        
        {
            f << "Start. Error = " << result << ", lastpoint=[";
            for (unsigned i=0;i<startpoint.size();i++)
                f << startpoint[i] << (i==startpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif               
    
    for (iterations=0;iterations<iterationcount && result>enoughresult;++iterations)
    {
        //calc jacobian
        for (int r=0;r<sz;++r)
        {
            currpoint = lastpoint;
            currpoint[r] = increaseOnDelta(lastpoint[r], e, sig);

            func(currpoint, currError);
            
            for (int i=0;i<sz;i++)
                J.at(i, r) = sig*(currError[i]-lastError[i])/e;
        }
        for (int i=0;i<sz;i++)
            Diff.at(i,0) = lastError[i];
            
        //find move
        //R = Matrix::Inverse(J)*Diff;
        R = MatrixSolve::LU_Decomposition(J, Diff);
               
        if (R.isNull())
        {
#ifdef _DEBUG               
            f << "R.isNull()" << std::endl;
#endif            
            R = Matrix(sz,1);
            for (int r=0;r<sz;++r)
                R.at(r, 0) = 0;
        }

        
        
               
        //calculate new point
        currpoint = lastpoint;
        double coef = 1;
        
        {
            //find length on direction
            std::vector<double> d;
            for (int r=0;r<sz;++r)
                d.push_back(R.at(r, 0));
            
            double minp, maxp;
            const auto checkfunc = [&] (double coef) -> double
                            {
                                bool allout = true;
                                double cv;
                                for (int i=0;i<sz && allout;++i)
                                {
//                                    cv = static_cast<double>(lastpoint[i]) - d[i]*coef;
                                    cv = static_cast<double>(lastpoint[i]) - d.at(i)*coef;
                                    allout = allout && (cv<minvalue_ || cv>maxvalue_);
                                }
                                return allout ? -1 : 1;
                            };
            minp = linearBinarySearch<double>(-1000, 0, checkfunc, 1e-12);
            maxp = linearBinarySearch<double>(0, 1000, checkfunc, 1e-12);
#ifdef _DEBUG               
            f << "acceptable coefs:" << minp << "->" << maxp << std::endl;
#endif             
            
            std::vector<double> start(1, 0), res(1, 0);
            double oldmin = minvalue_, oldmax = maxvalue_;
            Optimization<double>::setMinMaxValues(minp, maxp);
            long double newtonResult;
            std::vector<long double> err;
            Optimization<double>::NewtonSearch(start, res, 
                             [&] (const std::vector<double> &v, std::vector<long double> &res) -> bool
                             {
//#ifdef _DEBUG               
//            f << "----------" << v[0] << std::endl;
//#endif                   
                                std::vector<T> point(lastpoint);
                                for (int i=0;i<sz;++i)
                                    point[i] -= d[i]*v[0];
                                Optimization<T>::fixpointCut(point, oldmin, oldmax);
                                if (!func(point, err))
                                    return false;
                                double r = 0;
                                for (int i=0;i<sz;++i)
                                    r +=  err[i]*err[i];
                                res.resize(1);
                                res[0] = r;
                                return true;
                             },
                             enoughresult, newtonResult, 100);
            setMinMaxValues(oldmin, oldmax);
            
            coef = res[0];
#ifdef _DEBUG               
            f << "result coef:" << coef << std::endl;
#endif              
        }
        
        for (int r=0;r<sz;++r)
            lastpoint[r] -= R.at(r,0)*coef;
        
//#ifdef _DEBUG        
//        {
//            f << "point=[";
//            for (unsigned i=0;i<lastpoint.size();i++)
//                f << lastpoint[i] << (i==lastpoint.size()-1 ? "]" : ",");
//            f << std::endl;
//        }
//#endif           
        
        fixpointCut(lastpoint);
        
        
//        bool same=true;
//        for (int r=0;r<sz && same;++r)
//            if (fabs(currpoint[r]-lastpoint[r])>1e-12)
//                same = false;
//        
//        if (same)
//        {
//            
//#ifdef _DEBUG        
//            f << "~~~" << std::endl;
//#endif    
//                
//            if (rand()%2)
//            {
//                lastpoint = resultpoint;
//                for (int r=0;r<sz;r++)
//                    lastpoint[r] += (rand()%20001/10000.-1.)*0.01/(maxvalue_-minvalue_);
//                fixpointCut(lastpoint);
//            }
//            else
//            {
//                lastpoint = currpoint;
//                double coef = 1;
//                for (int r=0;r<sz;++r)
//                    if (fabs(R.at(r,0))>(maxvalue_-minvalue_)/4)
//                        coef = std::max<double>(coef, fabs(R.at(r,0))*100/(maxvalue_-minvalue_));
//                for (int r=0;r<sz;++r)
//                    lastpoint[r] -= R.at(r,0)/coef;
//                fixpoint(lastpoint);
//            }
//        }
                       
//        {
//            long double d = 0;
//            for (int r=0;r<sz && d<1e-9;++r)
//                d = std::max<long double>(d, fabsl(currpoint[r]-lastpoint[r]));
//            
//            if (d<1e-9)
//                break;
//        }
        
        func(lastpoint, lastError);
        terr = 0;
        for (int r=0;r<sz;++r)
            terr +=  lastError[r]*lastError[r];
        terr = sqrtl(terr);
        if (terr<result)
        {
            resultpoint = lastpoint;
            result = terr;
        }
        
#ifdef _DEBUG        
        {
            f << "It = " << iterations << ", Error = " << terr << ", lastpoint=[";
            for (unsigned i=0;i<lastpoint.size();i++)
                f << lastpoint[i] << (i==lastpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif           
        if (fabs(coef)<1e-12)
            break;
    }
       
#ifdef _DEBUG        
        {
            f << "Result. Error = " << result << ", point=[";
            for (unsigned i=0;i<resultpoint.size();i++)
                f << resultpoint[i] << (i==resultpoint.size()-1 ? "]" : ",");
            f << std::endl;
        }
#endif     
    
    return result<=enoughresult;
}

template<class T>
bool Optimization<T>::HookeJeevesSearch(std::vector<T> startpoint, std::vector<T> &resultpoint, 
                             std::function<long double(const std::vector<T> &)> func,
                             long double &result, long double H/* = 0.5*/, long double e/* = 1.0e-6*/,
                             long double lambda/* = 2.0*/, int iterationcount/*=10*/)
{
    std::vector<long double> currpoint, lastpoint;
  
    currpoint.assign(startpoint.begin(), startpoint.end());
    lastpoint.assign(startpoint.begin(), startpoint.end() );
     
    int sizeN = startpoint.size();
   //result = e;
    result = func(startpoint);
    std::vector<T> h(sizeN, H);
    
    int iteration = 0;
    while (cppfunc::any([e](T x){return x > e;}, h) && iteration<iterationcount )
    {
        for (int c=0;c<sizeN && cppfunc::any([e](T x){return x > e;}, h);)
        { 
            auto xd1 = currpoint;
            auto xd2 = currpoint;

            xd1[c] += h[c];
            xd2[c] -= h[c];

            xd1[c] = std::min<long double>(Optimization<T>::maxvalue_, xd1[c]);
            xd2[c] = std::max<long double>(Optimization<T>::minvalue_, xd2[c]);

            long double err1 = func(std::vector<T>(xd1.begin(), xd1.end() ));
            long double err2 = func(std::vector<T>(xd2.begin(), xd2.end() ));

            if (err2 < err1)
            {
                xd1[c] = xd2[c];
                err1 = err2;
            }

            if (err1 <= result && std::abs(xd1[c]-currpoint[c]) > e/2 )
            {
                currpoint = lastpoint  = xd1;
                result = err1;
                c++;
            }
            else
            {
                if (h[c] > e)
                    h[c] /= 2.0;
                else
                    c++;
            }
        }
        
        bool flag = true;
        int itcount = 0;
        while (flag && itcount<iterationcount)
        {
            flag = false;

//            bool changes=false;
            for (unsigned i=0;i<currpoint.size();i++)
            {
                currpoint[i] = startpoint[i] + lambda*(lastpoint[i] - startpoint[i]);
                currpoint[i] = std::min<long double>(Optimization<T>::maxvalue_,
                                                     std::max<long double>(Optimization<T>::minvalue_, currpoint[i]));
                // WARNING change var not used
//                if (currpoint[i]!=lastpoint[i])
//                    changes = true;
                startpoint[i] = lastpoint[i];
            }
            
            for (int c=0;c<sizeN;++c)
            {
                auto xd1 = currpoint;
                auto xd2 = currpoint;

                xd1[c] += h[c];
                xd2[c] -= h[c];

                xd1[c] = std::min<long double>(Optimization<T>::maxvalue_, xd1[c]);
                xd2[c] = std::max<long double>(Optimization<T>::minvalue_, xd2[c]);
//                bool ch1 = (xd1[c]!=currpoint[c]);
//                bool ch2 = (xd2[c]!=currpoint[c]);

                long double err1 = func(std::vector<T>(xd1.begin(), xd1.end() ) );
                long double err2 = func(std::vector<T>(xd2.begin(), xd2.end() ) );

                if (err2 < err1)
                {
                    xd1[c] = xd2[c];
                    err1 = err2;
//                    ch1 = ch2;
                }
                
                //if (err1 <= result && (changes || ch1))
                if (err1 < result)
                {
                    flag = true;
                    currpoint = lastpoint  = xd1;
                    result = err1;
                }
            }
            ++itcount;
        }
        
        currpoint = lastpoint;
        
        ++iteration;
    }
    
    resultpoint.assign(lastpoint.begin(), lastpoint.end() );
    
    return true;
}

template<class T>
bool Optimization<T>::LinearSearch(const T& startpoint, T& resultpoint,
                             std::function<bool(const T&, long double&)> func,
                             long double enoughresult, long double& result, int iterationcount/*=100*/,
                             T firststep/*=0*/, Monothonic m /*= monothonicUnknown*/, NearestType nearestType /*= Nearest*/)
{
    std::map<T,long double> points;
    std::vector<std::pair<T,long double>> lpoints;
    T x(startpoint);
    long double res;
    
    //min max are fakes usually
    points[minvalue_] = 0;
    points[maxvalue_] = 0;
    bool minFake = true, maxFake = true;
    
    //start point
    if (!func(x, res))
        return false;
    
    points[x] = res;
    
    //if (fabs(res)<enoughresult)
    if (IsClosest(nearestType, res, enoughresult) )
    {
        qDebug() << "LinearSearch 1";
        resultpoint = x;
        result = res;
        return true;
    }
    
    //probe point
    {
        int step = firststep==0 ? round(sqrt(maxvalue_-minvalue_)) : firststep;
        
        if (step>(maxvalue_-minvalue_)/2)
            step /= 2;
        
        if (m==monothonicIncrease && res>0)
            step = -step;
        if (m==monothonicDecrease && res<0)
            step = -step;       
        if (m==monothonicUnknown && startpoint-minvalue_>maxvalue_-startpoint)
            step = -step;
        
        x = startpoint+step;
        if (x<minvalue_ || x>maxvalue_)
        {
            step = -step;
            x = startpoint+step;
        }

        if (!func(x, res))
            return false;
    
        points[x] = res;
        
        //if (fabs(res)<enoughresult)
        if (IsClosest(nearestType, res, enoughresult) )
        {
            qDebug() << "LinearSearch 2";
            resultpoint = x;
            result = res;
            return true;
        }
    }
    
    qDebug() << "LinearSearch 3";
    
    for (int i=0;i<iterationcount;++i)
    {
        lpoints.clear();
        for (const auto& it : points)
            lpoints.push_back(it);
                
        //update fakes
        if (minFake)
        {
            auto p1 = *(lpoints.begin()+1);
            auto p2 = *(lpoints.begin()+2);
            points[minvalue_] = p1.second + (p2.second-p1.second)*(minvalue_-p1.first)/(p2.first-p1.first);
            lpoints[0] = *points.find(minvalue_);
            //qDebug() << "new min" << (double)minvalue_ << "->" << (double)lpoints[0].second;
        }
        
        if (maxFake)
        {
            auto p1 = *(lpoints.end()-2);
            auto p2 = *(lpoints.end()-3);
            points[maxvalue_] = p1.second + (p2.second-p1.second)*(maxvalue_-p1.first)/(p2.first-p1.first);
            lpoints[lpoints.size()-1] = *points.find(maxvalue_);
            //qDebug() << "new max" << (double)maxvalue_ << "->" << (double)lpoints[lpoints.size()-1].second;
        }
        
//        qDebug() << "----";
//        for (const auto& it: lpoints)
//            qDebug() << (double)it.first << "->" << (double)it.second;
//        qDebug() << "----";
        
        //search interval
        std::vector<decltype(lpoints.begin())> intervals;
        auto it=lpoints.begin();
        auto end = lpoints.end()-1;
        while(it!=end)
        {
            if ((it->second)*((it+1)->second)<=0)
            {
                intervals.push_back(it);
                if (m!=monothonicUnknown)
                    break;
            }
            ++it;
        }
        
        qDebug() << "LinearSearch 4";
        
        //qDebug() << "position" << it-lpoints.begin() << "( end - " << (end-lpoints.begin()) << ")";
       
        T measurePoint = minvalue_;
        
        if (intervals.empty())
        {
            //if no such interval
            
            if (minFake && fabs(points[minvalue_])<fabs(points[maxvalue_]) ) 
            {
                measurePoint = minvalue_;
                //qDebug() << "measure min";
            }
            else
                if (maxFake && fabs(points[maxvalue_])<fabs(points[minvalue_]) ) 
                {
                    measurePoint = maxvalue_;
                    //qDebug() << "measure max";
                }
                else
                {
                    //no such solution - stop
                    //qDebug() << "no solution stop";
                    //if (fabs(points[minvalue_])<fabs(points[maxvalue_]))
                    if (IsClosest(nearestType, points[minvalue_], points[maxvalue_]) )
                    {
                        qDebug() << "LinearSearch 5";
                        resultpoint = minvalue_;
                        result = points[minvalue_];
                        //return true;
                        break;
                    }
                    
                    qDebug() << "LinearSearch 6";
                    resultpoint = maxvalue_;
                    result = points[maxvalue_];
                    //return true;
                    break;
                }
                
        }
        else
        {
            qDebug() << "LinearSearch 7";
            //interval found
            
            it = intervals[rand()%intervals.size()];
            auto it1 = it+1;            
            qDebug() << (double)it->first  << (double)it1->first << (double)it->second << (double)it1->second;
            qDebug() << (double)(it->first + (long double)(it1->first - it->first)*(0-it->second)/(it1->second-it->second) );
            measurePoint = round(it->first + (long double)(it1->first - it->first)*(0-it->second)/(it1->second-it->second)); //0 as target
            qDebug() << "measure internal point - " << (double)measurePoint;
        }
        
        qDebug() << "LinearSearch 8";
        
        //if integral check for early stop
        if (std::is_integral<T>::value)
            if (points.find(measurePoint)!=points.end())
                if (!(measurePoint==minvalue_ && minFake) &&
                   !(measurePoint==maxvalue_ && maxFake) )
                {
                    qDebug() << "LinearSearch 9";
                    break;
//                    resultpoint = measurePoint;
//                    result = points[measurePoint];
//                    return true;
                }
        
        qDebug() << "LinearSearch 10";
        //measure and analyze
        if (!func(measurePoint, res))
            return false;

        points[measurePoint] = res;
        if (measurePoint==minvalue_)
            minFake = false;
        else
            if (measurePoint==maxvalue_)
                maxFake = false;
        
        
        //if (fabs(res)<enoughresult)
        if (IsClosest(nearestType, res, enoughresult) )
        {
            qDebug() << "LinearSearch 11";
            resultpoint = measurePoint;
            result = res;
            return true;
        }
    }
    
    qDebug() << "LinearSearch 12";
    
    //iterations finished - find closest
    T best=minvalue_;
    long double t, bd = 1.0e10;
    
    bool bestFound = false;
    for (const auto &it : points)
    {
        if (it.first==minvalue_ && minFake) continue;
        if (it.first==maxvalue_ && maxFake) continue;
        qDebug() << "LinearSearch 12.1 :" << (int)it.first << (double)it.second << (int)best << (double)bd;
        t = it.second;
        //if (t > 0.0 && (std::abs(bd) > std::abs(t) || !bestFound) )
        if (IsClosest(nearestType, t, bd) )
        {
            qDebug() << "LinearSearch 13";
            best = it.first;
            bd = t;
            bestFound=true;
        }
    }
    
    if (!bestFound)
    {
        for (const auto &it : points)
        {
            if (it.first==minvalue_ && minFake) continue;
            if (it.first==maxvalue_ && maxFake) continue;
            t = fabs(it.second);
            //if (t<bd)
            if (IsClosest(NearestType::Nearest, t, bd) )
            {
                qDebug() << "LinearSearch 14";
                best = it.first;
                bd = t;
            }
        }
    }
    
    // recheck nearby points
    auto it = points.find(best);
    if (it != points.end() && bd > 0.0)
    {
        std::vector<T> midPoints;
        if (it != points.begin() )
        {
            auto left = it;
            left--;
            if (it->second*left->second <= 0.0)
                midPoints.push_back( (it->first + left->first)/2 );
        }
        
        auto right = it;
        right++;
        if (right != points.end() && it->second*right->second <= 0.0)
            midPoints.push_back( (it->first + right->first)/2 );
        
        qDebug() << "current best point: " << (int)best << "   value:" << (double)bd;
        for (auto p : midPoints)
        {
            if (points.find(p) == points.end() )
            {
                long double value;
                if (!func(p, value) )
                    return false;
                points[p] = value;
                
                qDebug() << "try point" << (int)p;
                
                if (IsClosest(nearestType, value, bd) )
                {
                    best = p;
                    bd = value;
                    qDebug() << "found better" << (double)value;
                }
            }
        }
    }
    
    qDebug() << "LinearSearch 15";
    resultpoint = best;
    result = points[best];
    return true;
}


template<class T>
bool Optimization<T>::LinearSRSearch(const T& startpoint, T& resultpoint,
                             std::function<bool(const T&, long double&)> func,
                             long double enoughresult, long double& result, int iterationcount/*=100*/,
                             T firststep/*=0*/, Monothonic m/* = monothonicUnknown*/, NearestType nearestType/* = NearestType::Nearest*/)
{
    std::map<T,long double> points;
    std::vector<std::pair<T,long double>> lpoints;
    T x(startpoint);
    long double res;
       
    //start point
    if (!func(x, res))
        return false;
    
    points[x] = res;
    
    //if (fabs(res)<enoughresult)
    if (IsClosest(nearestType, res, enoughresult) )
    {
        qDebug() << "LinearSRSearch 1";
        resultpoint = x;
        result = res;
        return true;
    }
    
    //probe point
    {
        int step = firststep==0 ? round(sqrt(maxvalue_-minvalue_)) : firststep;
        
        if (step>(maxvalue_-minvalue_)/2)
            step /= 2;
        
        if (m==monothonicIncrease && res>0)
            step = -step;
        if (m==monothonicDecrease && res<0)
            step = -step;       
        if (m==monothonicUnknown && startpoint-minvalue_>maxvalue_-startpoint)
            step = -step;
        
        x = startpoint+step;
        if (x<minvalue_ || x>maxvalue_)
        {
            step = -step;
            x = startpoint+step;
        }

        if (!func(x, res))
            return false;
    
        points[x] = res;
        
        //if (fabs(res)<enoughresult)
        if (IsClosest(nearestType, res, enoughresult) )
        {
            qDebug() << "LinearSRSearch 2";
            resultpoint = x;
            result = res;
            return true;
        }
    }
    
    qDebug() << "LinearSRSearch 3";
    
    
    for (int i=0;i<iterationcount;++i)
    {
        std::vector<long double> xs, ys;
        std::vector<long double> coefs;
        
        for (auto el : points)
        {
            xs.push_back(el.second);
            ys.push_back(el.first);            
        }
        long double z;
        
        if (Regression<long double, long double>::NPowerRegression(1, xs, ys, coefs))
        {
            z = Regression<long double, long double>::ApplyRegression(0, coefs);
            if (z<minvalue_)
                z = minvalue_;
            if (z>maxvalue_)
                z = maxvalue_;
            x = round(z);
            
            
            if (points.find(x)!=points.end())
            {
                qDebug() << "same point - " << (double)x;
                resultpoint = x;
                result = points[x];
                return true;
            }
        }
        else
        {
            int step = firststep==0 ? round(sqrt(maxvalue_-minvalue_)) : firststep;        
            if (step>(maxvalue_-minvalue_)/2)
                step /= 2;

            if (m==monothonicIncrease && res>0)
                step = -step;
            if (m==monothonicDecrease && res<0)
                step = -step;       
            if (m==monothonicUnknown && startpoint-minvalue_>maxvalue_-startpoint)
                step = -step;

            x = startpoint+step;
            if (x<minvalue_ || x>maxvalue_)
            {
                step = -step;
                x = startpoint+step;
            }
        }
        
        
        
        
        if (!func(x, res))
            return false;    
        points[x] = res;
        
        resultpoint = x;
        result = res;
        
        if (!IsClosest(nearestType, res, enoughresult) )
        {
            if (m==monothonicIncrease)
                ++resultpoint;
            if (m==monothonicDecrease)
                --resultpoint;
            if (resultpoint<minvalue_)
                resultpoint = minvalue_;
            if (resultpoint>maxvalue_)
                resultpoint = maxvalue_;
        }
        
    }
    
    return true;
}

template<class T>
bool Optimization<T>::BinarySearch(T minPoint, T maxPoint, T &resultpoint, std::function<bool (const T &)> leftRightFunc, long double e)
{
    resultpoint = minPoint + (maxPoint-minPoint)/2;
    while(maxPoint-minPoint>e)
    {
        if (leftRightFunc(resultpoint))
            minPoint = resultpoint;
        else
            maxPoint = resultpoint;

        resultpoint = minPoint + (maxPoint-minPoint)/2;
    }

    return true;
}

template class Optimization<int>;
template class Optimization<double>;
template class Optimization<long double>;
