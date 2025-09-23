#ifndef OPTIMIZATION_H
#define	OPTIMIZATION_H

#include <vector>
#include <functional>
enum Monothonic
{
    monothonicUnknown,
    monothonicIncrease,
    monothonicDecrease
};

enum class NearestType
{
    Nearest,
    NearestLower,
    NearestUpper
};

template<class T>
class Optimization
{
    static void fixpoint(std::vector<T> &point);
    static void fixpoint(std::vector<T> &point, T minvalue, T maxvalue);
    
    static void fixpointCut(std::vector<T> &point);
    static void fixpointCut(std::vector<T> &point, T minvalue, T maxvalue);
    
    static T increaseOnDelta(T value, long double delta, int &sign);
    
    static bool any_of(std::vector<T> a, std::function<bool(T)> func);
    
    static bool IsClosest(NearestType nearestType, double value, double limit);
    
    static T minvalue_, maxvalue_;
    
public:
    
    class SearchFunction
    {
        friend class Optimization<T>;
        virtual bool value(const std::vector<T> &point, long double &res)=0;
    };
    
    class SearchMultiDimensionFunction
    {
        friend class Optimization<T>;
        virtual bool value(const std::vector<T> &point, std::vector<long double> &res)=0;
    };

    static T round(long double x);
    
    static void setMinMaxValues(T minvalue, T maxvalue);

    static bool RosenbrockSearch(std::vector<T> &startpoint, std::vector<T> &resultpoint, SearchFunction* func,
                                 long double searchresult, long double enoughresult, long double &result, int iterationcount=15);
    
    
    /**
     * Optimization based on Newton method (Jacobian-based gradient search).
     * Current realization search not the minimum of function, but zero-point.
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param func pointer to representation of function (function return error on every axis)
     * @param func function which return error on every axis (signature - bool(const std::vector<long double> &, std::vector<long double> &))
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> func,
                             long double enoughresult, long double &result, int iterationcount=100,
                             T increaseDelta = 1.0e-6);
    
    
    /**
     * Optimization based on Newton method (Jacobian-based gradient search).
     * Current realization search not the minimum of function, but zero-point.
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param axisErrFunc function which return error on every axis
     * @param addErrFunc function which return additional penalty error to total sum (axisErrFunc still used)
     * @param func function which return error on every axis (signature - bool(const std::vector<long double> &, std::vector<long double> &))
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> axisErrFunc,
                             std::function<long double(const std::vector<T> &)> addErrFunc,
                             long double enoughresult, long double &result, int iterationcount=100,
                             T increaseDelta = 1.0e-6);
    
    /**
     * Optimization based on Newton method (Jacobian-based gradient search).
     * Current realization search not the minimum of function, but zero-point.
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param axisErrFunc function which return error on every axis
     * @param errFunc function which return sum error (used curr point and results of axisErrFunc)
     * @param addErrFunc function which return additional penalty error to total sum (axisErrFunc still used)
     * @param func function which return error on every axis (signature - bool(const std::vector<long double> &, std::vector<long double> &))
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool NewtonSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> axisErrFunc,
                             std::function<long double(const std::vector<T> &, const std::vector<long double> &)> errFunc,
                             std::function<long double(const std::vector<T> &)> addErrFunc,
                             long double enoughresult, long double &result, int iterationcount=100,
                             T increaseDelta = 1.0e-6);
    
    /**
     * Optimization based on Newton method (Jacobian-based gradient search).
     * Current realization search not the minimum of function, but zero-point.
     * After Newton search made additional fast search with small perturbations
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param func pointer to representation of function (function return error on every axis)
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @param perturbationMaxDelta maximum perturbation for every axis
     * @param perturbationCount count of perturbation steps
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool NewtonSearchWithPerturbation(const std::vector<T> &startpoint, std::vector<T> &resultpoint, SearchMultiDimensionFunction* func,
                                 long double enoughresult, long double &result, int iterationcount=100, double perturbationMaxDelta=.5, int perturbationCount=10);
    
    
    /**
     * Optimization based on Newton method (Jacobian-based gradient search).
     * This is modification when every step made using Jacobian direction and length calculated with another one optimization
     * Current realization search not the minimum of function, but zero-point.
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param func function which return error on every axis (signature - bool(const std::vector<long double> &, std::vector<long double> &))
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool NewtonDeepSearch(const std::vector<T> &startpoint, std::vector<T> &resultpoint, 
                             std::function<bool(const std::vector<T> &, std::vector<long double> &)> func,
                             long double enoughresult, long double &result, int iterationcount=100);
    
    
    /**
     * Optimization based on Hooke-Jeeves Method.
     * Current realization search not the minimum of function, but zero-point.
     * Hint: If you need search not a zero-point receive from value "current-target".
     * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param func  error function (long double(const std::vector<T> &))
     * @param result received value of funtion with resultpoint
     * @param H [default = 0.5]
     * @param e [default = 10^-6]
     * @param lambda [default = 2]
     * @param iterationcount maximum iterations count
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool HookeJeevesSearch(std::vector<T> startpoint, std::vector<T> &resultpoint, 
                             std::function<long double(const std::vector<T> &)> func,
                             long double &result, long double H = 0.5, long double e = 1.0e-6,
                             long double lambda = 2.0, int iterationcount=10);
    
    
    /**
     * Search of function zero point with linear interpolation between known points
     * * @param startpoint first approximation of result point
     * @param resultpoint resulting point found by method
     * @param func search function (signature - bool(const T&, long double&))
     * @param enoughresult     acceptable value of function (remind: optimization search for 0)
     * @param result received value of funtion with resultpoint
     * @param iterationcount maximum iterations count
     * @param firststep first delat from startpoint for investigation, if 0 - sqrt(max-min) used
     * @param m monothonic type of function. Possible values: monothonicUnknown, monothonicIncrease, monothonicDecrease. Default: monothonicUnknown
     * @return is search finished succesfully, real result saved to resultpoint
     */
    static bool LinearSearch(const T& startpoint, T& resultpoint,
                             std::function<bool(const T&, long double&)> func,
                             long double enoughresult, long double& result, int iterationcount=100,
                             T firststep=0, Monothonic m = monothonicUnknown, NearestType nearestType = NearestType::Nearest);
    
    static bool LinearSRSearch(const T& startpoint, T& resultpoint,
                             std::function<bool(const T&, long double&)> func,
                             long double enoughresult, long double& result, int iterationcount=100,
                             T firststep=0, Monothonic m = monothonicUnknown, NearestType nearestType = NearestType::Nearest);

    /**
     * @brief BinarySearch
     * @param minPoint minimal acceptable value
     * @param maxPoint maximum acceptable value
     * @param resultpoint found point
     * @param leftRightFunc function that direct search, false -> choose left side, true -> choose right side
     * @param e limitation of (max-min) distance for search
     * @return if search was successful
     */
    static bool BinarySearch(T minPoint, T maxPoint, T& resultpoint,
                             std::function<bool(const T&)> leftRightFunc,
                             long double e = 1e-6);
};

#endif	/* OPTIMIZATION_H */

