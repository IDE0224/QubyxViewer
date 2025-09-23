#ifndef SERIALPORT_H
#define	SERIALPORT_H

#include <map>
#include "qextserialport.h"
#include "qextserialenumerator.h"

class SerialPort
{
public:
    SerialPort();
    virtual ~SerialPort();
    bool Open(int nPort, BaudRateType dwBaud = BAUD9600, ParityType parity = PAR_NONE, DataBitsType DataBits = DATA_8,
            StopBitsType stopbits = STOP_1, FlowType fc = FLOW_OFF, long timeout=100);
    bool Open(QString portName, BaudRateType dwBaud = BAUD9600, ParityType parity = PAR_NONE, DataBitsType DataBits = DATA_8,
            StopBitsType stopbits = STOP_1, FlowType fc = FLOW_OFF, long timeout=100);
    void Close();
    bool IsOpened();
    void SetTimeOut(long ms);
    int Read(char* buf, int maxsize);
    int Write(const char* buf, int size);
    int Read(unsigned char* buf, int maxsize);
    int Write(const unsigned char* buf, int size);
    void ReOpen();

    int ReadLine(char* buf, int maxsize);
    int ReadLine(unsigned char* buf, int maxsize);

    qint64 bytesAvailable();

    static int GetPortNumber(QString portName);
    static const QString GetPortName(std::map<int, QextPortInfo> &ports, int port);
    static void EnumeratePorts(std::map<int, QextPortInfo> &ports);

private:
    QextSerialPort *port_;
};

#endif	/* _SERIALPORT_H */
