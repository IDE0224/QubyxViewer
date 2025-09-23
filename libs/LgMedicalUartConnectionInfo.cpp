#include "LgMedicalUartConnectionInfo.h"
#include "QubyxDebug.h"
#include "EDIDParser.h"

#include <QDebug>
#include <set>

std::map<QString, LgMedicalUartConnectionInfo::UartConnectionInfo> LgMedicalUartConnectionInfo::uartConnectionsInfo_;

std::map<QString, LgMedicalUartConnectionInfo::UartConnectionInfo> LgMedicalUartConnectionInfo::enumerateUartConnections()
{
    qDebug() << "LGMedicalDDCCIControl::enumerateUartConnections()";

    std::map<int, QextPortInfo> ports;
    SerialPort::EnumeratePorts(ports);

    qDebug() << "Total ports:" << ports.size();

    std::set<QString> allLgUartPorts;

    for (const auto &kv: ports)
    {
        qDebug() << "LGMedicalDDCCIControl port" << kv.first << kv.second.enumName << kv.second.physName << kv.second.portName << kv.second.vendorID << kv.second.productID;

        if ((kv.second.vendorID==0x451 && kv.second.productID==0x3410) ||
            (kv.second.vendorID==0x43E && kv.second.productID==0x9A39) ||
            (kv.second.vendorID==0x43E && kv.second.productID==0x9C00))
        {
            QString portName = kv.second.portName;

            qDebug() << "LGMedicalDDCCIControl port" << kv.first << portName << "correct vendor/product";
            allLgUartPorts.insert(portName);

            if (uartConnectionsInfo_.count(portName))
            {
                qDebug() << "Port already saved";
                continue;
            }

            SerialPort port;
            bool ok = port.Open(portName, BAUD19200, PAR_NONE, DATA_8, STOP_1, FLOW_OFF);
            qDebug() << "Port opened:" << ok;

            if (!ok)
            {
                qDebug() << "Cant connecto to port" << portName;
                continue;
            }

            auto edid = readEdid(port);
            if (edid.size())
            {
                qDebug() << "Add new UART connection on port" << portName;
                UartConnectionInfo info;
                info.edid = edid;
                uartConnectionsInfo_[portName] = info;
            }
            else
            {
                qDebug() << "No connection on port" << portName;
            }
            port.Close();
        }
    }

    std::vector<QString> unusedPorts;
    for(auto &it: uartConnectionsInfo_)
    {
        bool isFound = false;
        for (auto itLg : allLgUartPorts)
            if (itLg == it.first){
                isFound = true;
                break;
            }
        if (!isFound){
            qDebug() << "remove unused port info" << it.first;
            unusedPorts.push_back(it.first);
        }
    }

    for (auto &portName: unusedPorts)
        uartConnectionsInfo_.erase(portName);

    qDebug() << "Total LG UART ports detected:" << uartConnectionsInfo_.size();

    return uartConnectionsInfo_;
}

QByteArray LgMedicalUartConnectionInfo::readEdid(SerialPort &port)
{
    qDebug() << "LGMedicalDDCCIControl::readEdid";

    for (int i = 0; i < 10; i++) {
        QByteArray rawEdid = readEdidRaw(port);

        if (checkIsValidEdid(rawEdid))
            return rawEdid;

    }
    qDebug() << "readEdid was failed";
    return QByteArray();
}

QByteArray LgMedicalUartConnectionInfo::readEdidRaw(SerialPort &port)
{
    qDebug() << "LgMedicalUartConnectionInfo::readEdidRaw";

    unsigned char package[] = {0x73, 0x74, 0x6e, 0x50, 0xa0, 0x65, 0x64};
    $D(package, 7);

    readRest(port);
    port.Write(package, 7);

    unsigned char pkg[1024];
    int sz = port.Read(pkg, 1024);
    qDebug() << "read size: " << sz;
    if (sz < 128 || sz > 1024) {
        qDebug() << "No EDID data";
        return QByteArray();
    }

    qDebug() << "UART Raw EDID:";
    $D(pkg, sz);

    return QByteArray((char*)pkg, sz);
}

void LgMedicalUartConnectionInfo::readRest(SerialPort &port)
{
    if (!port.IsOpened())
        return;

    auto sz = port.bytesAvailable();
    if (sz>0)
    {
        unsigned char package[1024];
        port.Read(package, sz);
        qDebug() << "garbage in serial port:";
        $D(package, sz);
    }
}

bool LgMedicalUartConnectionInfo::checkIsValidEdid(QByteArray rawEdid)
{
    qDebug() << "LgMedicalUartConnectionInfo::checkIsValidEdid";
    if (rawEdid.size() < 128)
    {
        qDebug() << "EDID is too short" << rawEdid.size();
        return false;
    }

    if (rawEdid.size() % 128)
    {
        qDebug() << "EDID size is not multiplier of 128" << rawEdid.size();
        return false;
    }

    for (size_t blockBegin= 0; uint(blockBegin + 128) <= uint(rawEdid.size()); blockBegin += 128)
    {
        uint8_t checksum=0;
        for (size_t i = 0; i < 128; i++)
            checksum = (checksum + static_cast<uint8_t>(rawEdid[uint(blockBegin + i)]))%256;
        if (checksum)
        {
            qDebug() << "Wrong EDID block checksum at pos" << blockBegin << (blockBegin + 127) << "value:" << checksum;
            return false;
        }
    }

    qDebug() << "Valid EDID";
    return true;
}
