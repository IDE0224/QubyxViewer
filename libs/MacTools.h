#ifndef MACTOOLS_H
#define MACTOOLS_H

#include <qglobal.h>
#include <QString>
#include <map>

#ifdef Q_OS_MAC

namespace MacTools {
    void hideDockBar();
    void showDockBar();
    bool getMacOsVersion(int &major, int &minor, int &patch);

    struct MacDisplayInfo
    {
        QString vendor;
        QString vendor_short;
        QString name;
        QString model;
        QString serialNumber;
        double width_mm = 0;
        double height_mm = 0;
    };

    std::map<QString, MacDisplayInfo> additionalDisplayInfo();
}

#endif

#endif // MACTOOLS_H
