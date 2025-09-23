#include "GlobalData.h"

DataBus GlobalData::bus_;

QString GlobalData::getString(QString value)
{
    return GlobalData::GetValue <QString> (value);
}

bool GlobalData::setString(QString key, QString value)
{
    return GlobalData::SetValue <QString> (key, value);
}
