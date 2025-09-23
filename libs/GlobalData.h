#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include "DataBus.h"
#include <map>

class GlobalData
{
    static DataBus bus_;
    GlobalData() = default;

public:
    template<class T> static bool SetValue(QString key, const T &value)
    {
        return bus_.SetValue<T>(key, value);
    }

    template<class T> static T GetValue(QString key)
    {
        return bus_.GetValue<T>(key);
    }

    static QString getString(QString valueName);
    static bool    setString(QString key, QString value);
};

#endif /* _GLOBALDATA_H */
