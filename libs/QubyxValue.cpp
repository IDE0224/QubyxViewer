/*#include "QubyxValue.h"


QubyxAbstractValue::QubyxAbstractValue()
{
}

QubyxAbstractValue::~QubyxAbstractValue()
{
}

template<class T> void setValue(T value, QubyxAbstractValue* absv)
{
    if (dynamic_cast<QubyxValue<T>*>(absv))
        dynamic_cast<QubyxValue<T>*>(absv)->value_ = value;
}

template<class T> T getValue(QubyxAbstractValue* absv)
{
    if (dynamic_cast<QubyxValue<T>*>(absv))
        return dynamic_cast<QubyxValue<T>*>(absv)->value_;
    
    return T();
}*/

