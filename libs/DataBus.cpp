#include "DataBus.h"
#include <QDebug>
#include <map>

DataBus::DataBus()
{
}

DataBus::~DataBus()
{
    ClearData();
}

void DataBus::ClearData()
{
    for (std::map<QString, QubyxAbstractValue*>::iterator it = data_.begin() ; it != data_.end(); ++it)
        delete it->second;
    data_.clear();
}
