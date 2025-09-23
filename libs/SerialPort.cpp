#include "SerialPort.h"

#include "qextserialenumerator.h"
#include "qextserialport.h"
#include <QList>
#include "QubyxDebug.h"
#include "GlobalData.h"
#include "GlobalNames.h"

#include <QDebug>
#include <QTime>

int SerialPort::GetPortNumber(QString portName)
{
    std::map<int, QextPortInfo> ports;
    EnumeratePorts(ports);

    for (std::map<int, QextPortInfo>::iterator itr=ports.begin();itr!=ports.end();++itr)
        if (itr->second.portName==portName)
            return itr->first;

    return 0;
}

const QString SerialPort::GetPortName(std::map<int, QextPortInfo> &ports, int port)
{$C;
    std::map<int, QextPortInfo>::iterator portItr = ports.find(port);
    if (portItr == ports.end())
        return "";

    return portItr->second.portName;
}

void SerialPort::EnumeratePorts(std::map<int, QextPortInfo> &ports)
{$C;
    QList<QextPortInfo> portList;
    int portNumber;

    portList = QextSerialEnumerator::getPorts();
    qDebug() << "@@ portList.count() = " << portList.count();
    if (!portList.count())
        return;

    ports.clear();
    portNumber = 0;
    for (auto portListItr = portList.begin(); portListItr!=portList.end(); ++portListItr){
        qDebug() << "@@ portNumber = " << portNumber;
        ports.insert(std::make_pair(portNumber, *portListItr));
        ++portNumber;
    }
}


SerialPort::SerialPort()
{
    port_ = nullptr;
}

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open(int nPort, BaudRateType Baud, ParityType parity, DataBitsType DataBits,
                      StopBitsType stopbits, FlowType fc, long timeout)
{
    std::map<int, QextPortInfo> ports;

    Close();

    EnumeratePorts(ports);
    //for (std::map<int, QextPortInfo>::iterator itr = ports.begin(); itr!=ports.end(); ++itr)
    //    qDebug()<<"@@ {"<<itr->first<<','<<itr->second.portName<<","<<itr->second.physName<<"}";
    if (ports.empty())
        return false;

    if (nPort < 0 || nPort >= static_cast <int> (ports.size())){
        qDebug() << "Wrong port number!!!" << nPort << ports.size();
        nPort = 0;
    }

    return Open(GetPortName(ports, nPort), Baud, parity, DataBits, stopbits, fc, timeout);
}

bool SerialPort::Open(QString portName, BaudRateType baud, ParityType parity, DataBitsType DataBits, StopBitsType stopbits, FlowType fc, long timeout)
{
    port_ = new QextSerialPort(portName, QextSerialPort::Polling);
    if (!port_)
        return false;

    port_->setBaudRate(baud);
    port_->setParity(parity);
    port_->setDataBits(DataBits);
    port_->setStopBits(stopbits);
    port_->setFlowControl(fc);
    port_->setTimeout(timeout);

    if (!port_->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        qDebug() << "can't open serial port:" << port_->errorString();

    return IsOpened();
}

void SerialPort::Close()
{
    if (!port_)
        return;

    if (port_->isOpen())
        port_->close();

    delete port_;
    port_ = nullptr;
}

bool SerialPort::IsOpened()
{
    if (!port_){
        qDebug() << "bool SerialPort::IsOpened() object not exists";
        return false;
    }

    bool isOpen = port_->isOpen();
    qDebug() << "bool SerialPort::IsOpened() " << isOpen << QTime::currentTime();
    return isOpen;
}

void SerialPort::SetTimeOut(long ms)
{
    if (IsOpened())
        port_->setTimeout(ms);
}

int SerialPort::Read(char* buf, int maxsize)
{
    qDebug() << "Serial port Read: " << buf;
    if (!IsOpened()) {
        qDebug() << "SerialPort::Read returns 0 due to !IsOpened()";
        return 0;
    }

    char result[maxsize];
    qint64 size = 0;
    qint64 blockSize = 1;
    while (blockSize > 0 && size < maxsize){
        char newBuf[maxsize];
        blockSize = port_->read((char*)(newBuf), maxsize);
        for (qint64 i = 0; i < blockSize; i++)
            result[size + i] = newBuf[i];

        size += blockSize;
    }

    qDebug() << "read serial port: result" << result;
    std::copy(result, result+size, buf);
    $D(buf, size);
    return size;
}

int SerialPort::Write(const char* buf, int size)
{
    if (!IsOpened())
        return 0;
    return port_->write(buf, size);
}

int SerialPort::Read(unsigned char* buf, int maxsize)
{
    return Read((char*)(buf), maxsize);
}

int SerialPort::Write(const unsigned char* buf, int size)
{
    return Write((const char*)(buf), size);
}

void SerialPort::ReOpen()
{
    BaudRateType baud = port_->baudRate();
    ParityType parity = port_->parity();
    DataBitsType dbt = port_->dataBits();
    StopBitsType stopbits = port_->stopBits();
    FlowType fc = port_->flowControl();
    QString name = port_->portName();
    Close();
//    Sleep(250);
    Open(name, baud, parity, dbt, stopbits, fc);
//    Sleep(250);
}

int SerialPort::ReadLine(char* buf, int maxsize)
{
    if (!IsOpened())
        return 0;

    return port_->readLine(buf, maxsize);
}

int SerialPort::ReadLine(unsigned char* buf, int maxsize)
{
    return ReadLine((char*)(buf), maxsize);
}

qint64 SerialPort::bytesAvailable()
{
    if (!IsOpened())
        return 0;

    return port_->bytesAvailable();
}
