#ifndef WINREGISTRY_H
#define	WINREGISTRY_H

#include <QtCore/qglobal.h>

#ifdef Q_OS_WIN


#include <windows.h>
#include <QString>


class WinRegistry
{
public:
    typedef HKEY branch_t;
    typedef unsigned int mode_t;

    static const branch_t ROOT;
    static const branch_t CURRENT_CONFIG;
    static const branch_t CURRENT_USER;
    static const branch_t LOCAL_MACHINE;
    static const branch_t PERFORMANCE_DATA;
    static const branch_t USERS;

    static const mode_t QUERY;
    static const mode_t SET;
    static const mode_t WIN64REG;


    enum datatype_t
    {
        ANY = 0x0000ffff,
        BINARY = REG_BINARY,
        DOUBLE_WORD = REG_DWORD,
        EXPANDABLE_STRING = REG_EXPAND_SZ,
        MULTILINE_STRING = REG_MULTI_SZ,
        NONE = REG_NONE,
        QUARTERUPLED_WORD = REG_QWORD,
        STRING = REG_SZ
    };

    struct valueinfo_t
    {
        QString value;
        datatype_t type;
    };


    class Key
    {
    private:
        bool Ok_;
        HKEY HKey_;

    public:
        Key();
        void open(branch_t branch, const QString &keyName, mode_t mode=QUERY);
        void close();
        unsigned int valuesAmount();
        QString queryValue(const QString &valueName);
        valueinfo_t queryValueInfo(const QString &valueName);
        void setValue(const QString &valueName, void *data, datatype_t valueType);
        bool ok();
    };
};


#endif

#endif	/* WINREGISTRY_H */

