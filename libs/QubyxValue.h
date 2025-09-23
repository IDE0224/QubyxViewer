#ifndef QUBYXABSTRACTVALUE_H
#define	QUBYXABSTRACTVALUE_H
#include <typeinfo>
class QubyxAbstractValue;

template<class T> class QubyxValue;

template<class T> void setValue(const T& value, QubyxAbstractValue* absv)
{
    if (dynamic_cast<QubyxValue<T>*>(absv))
        dynamic_cast<QubyxValue<T>*>(absv)->value_ = value;
}
template<class T> T getValue(QubyxAbstractValue* absv)
{
    if (dynamic_cast<QubyxValue<T>*>(absv))
        return dynamic_cast<QubyxValue<T>*>(absv)->value_;

    return T();
}

template<class T> T* getPointToValue(QubyxAbstractValue* absv)
{
    if (dynamic_cast<QubyxValue<T>*>(absv))
        return &(dynamic_cast<QubyxValue<T>*>(absv)->value_);
    
    return nullptr;
}

class QubyxAbstractValue
{
public:
    virtual ~QubyxAbstractValue() {}
    virtual bool CheckType(const std::type_info &typeinfo) = 0;
};

template<class T>
class QubyxValue: public QubyxAbstractValue
{
    T value_;
public:
    QubyxValue(const T &value) : value_(value)
    {
    }

     virtual bool CheckType(const std::type_info &typeinfo)
     {
         return typeid(T)==typeinfo;
     }

     template<class T1> friend void setValue(const T1& value, QubyxAbstractValue* absv);
     template<class T1> friend T1 getValue(QubyxAbstractValue* absv);
     template<class T1> friend T1* getPointToValue(QubyxAbstractValue* absv);
};

#endif	/* _QUBYXABSTRACTVALUE_H */

