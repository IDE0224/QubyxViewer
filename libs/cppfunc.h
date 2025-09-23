/* 
 * File:   cppfunc.h
 * Author: QUBYX Software Technologies LTD HK
 */

#ifndef CPPFUNC_H
#define	CPPFUNC_H

#include <vector>
#include <deque>
#include <list>
#include <functional>
#include <type_traits>
#include <cmath>

namespace cppfunc
{
    
    namespace cppfuncPrivate
    {
        template<typename Cont, typename ContValue>
        inline void add(Cont& container, ContValue&& value)
        {
            container.insert(end(container), value);
        }
        
        template<typename ContValue>
        inline void add(std::vector<ContValue>& container, ContValue&& value)
        {
            container.push_back(value);
        }
        
        template<typename ContValue>
        inline void add(std::deque<ContValue>& container, ContValue&& value)
        {
            container.push_back(value);
        }
        
        template<typename ContValue>
        inline void add(std::list<ContValue>& container, ContValue&& value)
        {
            container.push_back(value);
        }
        
        template<typename T1, typename T2>
        inline auto plus(T1 v1, T2 v2) -> decltype((v1+v2))
        {
            return v1+v2;
        }
        
        template<typename T1, typename T2>
        inline auto multiply(T1 v1, T2 v2) -> decltype((v1*v2))
        {
            return v1*v2;
        }
    }
    
template<typename Pred, typename Cont>
bool all(Pred predicate, const Cont& container)
{
    for (auto it = begin(container); it != end(container); ++it)
        if (!predicate(*it))
            return false;
    return true;
}

template<typename Pred, typename Cont>
bool any(Pred predicate, const Cont& container)
{
    for (auto it = begin(container); it != end(container); ++it)
        if (predicate(*it))
            return true;
    return false;
}
        
template<typename Pred, typename Cont>
bool none(Pred predicate, const Cont& container)
{   
    return !any(predicate, container);
}

//ContIn should have insert function
template<typename Transform, typename ContIn>
auto map(Transform transformFunction, const ContIn& inputContainer) 
        -> std::list<typename std::remove_reference<decltype(transformFunction(*begin(inputContainer)))>::type>
{
    std::list<decltype(transformFunction(*begin(inputContainer)))> res;
    auto eIt = end(inputContainer);
    for (auto it = begin(inputContainer); it != eIt; ++it)
        cppfuncPrivate::add(res, transformFunction(*it));
    
    return res;
}

template<typename Transform, typename Cont>
void mapInPlace(Transform transformFunction, Cont& container)
{
    auto eIt = end(container);
    for (auto it = begin(container); it != eIt; ++it)
        *it = transformFunction(*it);
}

//contOut should have insert function
template<typename ZipFunction, typename ContIn1, typename ContIn2>
auto zipWith(ZipFunction zipFunction, const ContIn1& inputContainer1, const ContIn2& inputContainer2) 
        -> std::list<typename std::remove_reference<decltype(zipFunction(*begin(inputContainer1), *begin(inputContainer2)))>::type>
{
    std::list<decltype(zipFunction(*begin(inputContainer1), *begin(inputContainer2)))> res;
    auto eIt1 = end(inputContainer1), eIt2 = end(inputContainer2);
    for (auto it1 = begin(inputContainer1), it2 = begin(inputContainer2); it1 != eIt1  && it2 != eIt2; ++it1, ++it2)
        cppfuncPrivate::add(res, zipFunction(*it1, *it2));
    
    return res;
}

template<typename FoldFunction, typename Cont, typename StartValue>
auto foldl(FoldFunction foldFun, const StartValue& startValue, const Cont& container) 
        -> typename std::remove_reference<decltype(foldFun(startValue, *begin(container)))>::type
{
    auto bIt = begin(container), eIt = end(container);
    decltype(foldFun(startValue, *bIt)) res = startValue;
    for (auto it = bIt; it != eIt; ++it)
    {
        res = foldFun(res, *it);
    }
    return res;
}

template<typename FoldFunction, typename Cont, typename StartValue>
auto foldr(FoldFunction foldFun, const StartValue& startValue, const Cont& container) 
        -> typename std::remove_reference<decltype(foldFun(startValue, *begin(container)))>::type
{
    auto bIt = begin(container), eIt = end(container);
    decltype(foldFun(startValue, *bIt)) res = startValue;
    if (bIt==eIt)
        return res;
    
    --eIt; --bIt;
    for (auto it = eIt; it != bIt; --it)
        res = foldFun(res, *it);
    return res;
}

template<typename Cont>
auto sum(const Cont& container) -> typename std::remove_reference<decltype(*begin(container))>::type
{
    return foldl(cppfuncPrivate::plus<decltype(*begin(container)), decltype(*begin(container))>, static_cast<decltype(*begin(container))>(0), container);
}

template<typename Cont>
auto product(const Cont& container) -> typename std::remove_reference<decltype(*begin(container))>::type
{
    return foldl(cppfuncPrivate::multiply<decltype(*begin(container)), decltype(*begin(container))>, static_cast<decltype(*begin(container))>(1), container);
}

template<typename Pred, typename Cont>
Cont filter(Pred predicate, const Cont& container)
{   
    Cont res;
    auto eIt = end(container);
    for (auto it = begin(container); it != eIt; ++it)
        if (predicate(*it))
            cppfuncPrivate::add(res, *it);
    return res;
}

template<typename Cont1, typename Cont2>
bool equal(Cont1 container1, Cont2 container2)
{
    auto b1 = begin(container1), e1 = end(container1);
    auto b2 = begin(container2), e2 = end(container2);
      
    while(b1!=e1 && b2!=e2)
    {
        if (*b1!=*b2)
            return false;
        
        ++b1;
        ++b2;
    }
    
    return b1==e1 && b2==e2;
}

template<typename Cont1, typename Cont2, typename EpsType>
bool equal_eps(Cont1 container1, Cont2 container2, EpsType eps)
{
    auto b1 = begin(container1), e1 = end(container1);
    auto b2 = begin(container2), e2 = end(container2);
      
    while(b1!=e1 && b2!=e2)
    {
        if (std::abs(*b1 - *b2) > eps)
            return false;
        
        ++b1;
        ++b2;
    }
    
    return b1==e1 && b2==e2;
}

template<typename InType, typename OutType>
std::vector<OutType> vector_cast(const std::vector<InType> &vect)
{
    std::vector<OutType> res(vect.size());
    for (std::size_t i=0;i<vect.size();i++)
        res[i] = vect[i];
    return res;
}

}

#endif	/* CPPFUNC_H */

