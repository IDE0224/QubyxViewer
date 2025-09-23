#include "WinRegistry.h"

#include <QtCore/qglobal.h>

#ifdef Q_OS_WIN


const WinRegistry::branch_t WinRegistry::ROOT = HKEY_CLASSES_ROOT;
const WinRegistry::branch_t WinRegistry::CURRENT_CONFIG = HKEY_CURRENT_CONFIG;
const WinRegistry::branch_t WinRegistry::CURRENT_USER = HKEY_CURRENT_USER;
const WinRegistry::branch_t WinRegistry::LOCAL_MACHINE = HKEY_LOCAL_MACHINE;
const WinRegistry::branch_t WinRegistry::PERFORMANCE_DATA = HKEY_PERFORMANCE_DATA;
const WinRegistry::branch_t WinRegistry::USERS = HKEY_USERS;

const WinRegistry::mode_t WinRegistry::QUERY = KEY_QUERY_VALUE;
const WinRegistry::mode_t WinRegistry::SET = KEY_SET_VALUE;
const WinRegistry::mode_t WinRegistry::WIN64REG = KEY_WOW64_64KEY;


WinRegistry::Key::Key()
    : Ok_(true), HKey_(nullptr)
{
}

void WinRegistry::Key::open(branch_t branch, const QString &keyName, mode_t mode)
{
    LONG retval;


    retval = RegOpenKeyExA(HKEY(branch), keyName.toLatin1().data(), 0, mode, &HKey_);
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return;
    }

    Ok_ = true;
}

void WinRegistry::Key::close()
{
    LONG retval;


    retval = RegCloseKey(HKey_);
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return;
    }

    Ok_ = true;
}

unsigned int WinRegistry::Key::valuesAmount()
{
    LONG retval;
    DWORD amount;


    retval = RegQueryInfoKeyA(HKey_, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &amount, nullptr, nullptr, nullptr, nullptr);
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return -1;
    }

    Ok_ = true;
    return amount;
}

QString WinRegistry::Key::queryValue(const QString &valueName)
{
    LONG retval;
    BYTE *data;
    DWORD dataSize;
    QString value;


    retval = RegQueryValueExA(HKey_, valueName.toLatin1().data(), nullptr, nullptr, nullptr, &dataSize);
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return "";
    }

    data = new BYTE[dataSize];

    retval = RegQueryValueExA(HKey_, valueName.toLatin1().data(), nullptr, nullptr, data, &dataSize);
    if (retval != ERROR_SUCCESS)
    {
        delete[] data;
        Ok_ = false;
        return "";
    }

    value = (char*)data;

    delete [] data;
    Ok_ = true;
    return value;
}

WinRegistry::valueinfo_t WinRegistry::Key::queryValueInfo(const QString &valueName)
{
    LONG retval;
    BYTE *data;
    DWORD dataSize, valueType;
    valueinfo_t valueInfo;


    retval = RegQueryValueExA(HKey_, valueName.toLatin1().data(), nullptr, nullptr, nullptr, &dataSize);
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return valueinfo_t();
    }

    data = new BYTE[dataSize];

    retval = RegQueryValueExA(HKey_, valueName.toLatin1().data(), nullptr, &valueType, data, &dataSize);
    if (retval != ERROR_SUCCESS)
    {
        delete[] data;
        Ok_ = false;
        return valueinfo_t();
    }

    valueInfo.value = (char*)data;
    valueInfo.type = (datatype_t)valueType;

    delete [] data;
    Ok_ = true;
    return valueInfo;
}

void WinRegistry::Key::setValue(const QString &valueName, void *data, datatype_t dataType)
{
    LONG retval;


    retval = RegSetValueExA(HKey_, valueName.toLatin1().data(), NULL, dataType, (BYTE*)data, valueName.length());
    if (retval != ERROR_SUCCESS)
    {
        Ok_ = false;
        return;
    }

    Ok_ = true;
}

bool WinRegistry::Key::ok()
{
    return Ok_;
}


#endif
