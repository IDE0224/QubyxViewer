#ifndef LGMEDICALUARTCONNECTIONINFO_H
#define LGMEDICALUARTCONNECTIONINFO_H

#include <map>
#include <QByteArray>
#include "SerialPort.h"

class LgMedicalUartConnectionInfo
{
public:
    struct UartConnectionInfo
    {
        QByteArray edid;
    };

    static std::map<QString, UartConnectionInfo> enumerateUartConnections();

protected:
    static std::map<QString, UartConnectionInfo> uartConnectionsInfo_;

    static QByteArray readEdid(SerialPort &port);
    static QByteArray readEdidRaw(SerialPort &port);
    static void readRest(SerialPort &port);

    static bool checkIsValidEdid(QByteArray rawEdid);
};

#endif // LGMEDICALUARTCONNECTIONINFO_H
