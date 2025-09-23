#ifndef DATABUS_H
#define	DATABUS_H

#include <QDebug>
#include <QString>
#include <map>
#include "QubyxValue.h"

class DataBus
{
    std::map<QString, QubyxAbstractValue*> data_;
public:
    DataBus();
    virtual ~DataBus();

    void ClearData();

    bool IsValueExist(QString valueName)
    {
        return data_.find(valueName)!=data_.end();
    }

    template<class T>  bool SetValue(QString valueName, const T &value)
    {
        if (data_.find(valueName)!=data_.end())
        {
            if (!data_[valueName]->CheckType(typeid(T)))
            {
                qDebug()<<"DataBus Error - Value with name "<<valueName<<" already exist and wrong type!!!";
                return false;
            }
            else
              setValue<T>(value,data_[valueName]);

        }
        else
            data_[valueName] = new  QubyxValue<T>(value);

        return true;
    }

    template<class T>  T GetValue(QString valueName, T defaultValue=T())
    {
        if (data_.find(valueName)==data_.end())
            return defaultValue;

        if (!data_[valueName]->CheckType(typeid(T)))
        {
            qDebug()<<"DataBus Error - Value with name "<<valueName<<" has wrong type!!!";
            return defaultValue;
        }

        return getValue<T>(data_[valueName]);
    }
    
    template<class T>  T* GetPointToValue(QString valueName)
    {
       
        if (data_.find(valueName)==data_.end())
            return nullptr;

        if (!data_[valueName]->CheckType(typeid(T)))
        {
            qDebug()<<"DataBus Error - Value with name "<<valueName<<" has wrong type!!!";
            return nullptr;
        }

        return getPointToValue<T>(data_[valueName]);
    }

    void RemoveValue(QString valueName)
    {
        auto it = data_.find(valueName);
        if (it==data_.end())
            return;
        delete it->second;
        data_.erase(it);
    }
};

#endif	/* DATABUS_H */

