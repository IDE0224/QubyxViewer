#include <stdio.h>
#include "qextserialport.h"

#include <QtCore/qglobal.h>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <setupapi.h>
#include <winioctl.h>
#endif

/*!
Default constructor.  Note that the name of the device used by a QextSerialPort constructed with
this constructor will be determined by #defined constants, or lack thereof - the default behavior
is the same as _TTY_LINUX_.  Possible naming conventions and their associated constants are:

\verbatim

Constant         Used By         Naming Convention
----------       -------------   ------------------------
Q_OS_WIN        Windows         COM1, COM2
_TTY_IRIX_       SGI/IRIX        /dev/ttyf1, /dev/ttyf2
_TTY_HPUX_       HP-UX           /dev/tty1p0, /dev/tty2p0
_TTY_SUN_        SunOS/Solaris   /dev/ttya, /dev/ttyb
_TTY_DIGITAL_    Digital UNIX    /dev/tty01, /dev/tty02
_TTY_FREEBSD_    FreeBSD         /dev/ttyd0, /dev/ttyd1
_TTY_OPENBSD_    OpenBSD         /dev/tty00, /dev/tty01
_TTY_LINUX_      Linux           /dev/ttyS0, /dev/ttyS1
<none>           Linux           /dev/ttyS0, /dev/ttyS1
\endverbatim

This constructor assigns the device name to the name of the first port on the specified system.
See the other constructors if you need to open a different port.
*/
QextSerialPort::QextSerialPort(QextSerialPort::QueryMode mode)
    : QIODevice()
{

#ifdef Q_OS_WIN
    setPortName("COM1");

#elif defined(_TTY_IRIX_)
    setPortName("/dev/ttyf1");

#elif defined(_TTY_HPUX_)
    setPortName("/dev/tty1p0");

#elif defined(_TTY_SUN_)
    setPortName("/dev/ttya");

#elif defined(_TTY_DIGITAL_)
    setPortName("/dev/tty01");

#elif defined(_TTY_FREEBSD_)
    setPortName("/dev/ttyd1");

#elif defined(_TTY_OPENBSD_)
    setPortName("/dev/tty00");

#else
    setPortName("/dev/ttyS0");
#endif

    construct();
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Constructs a serial port attached to the port specified by name.
name is the name of the device, which is windowsystem-specific,
e.g."COM1" or "/dev/ttyS0".
*/
QextSerialPort::QextSerialPort(const QString& name, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setQueryMode(mode);
    setPortName(name);
    platformSpecificInit();
}

/*!
Constructs a port with default name and specified settings.
*/
QextSerialPort::QextSerialPort(const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Constructs a port with specified name and settings.
*/
QextSerialPort::QextSerialPort(const QString& name, const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setPortName(name);
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Common constructor function for setting up default port settings.
(115200 Baud, 8N1, Hardware flow control where supported, otherwise no flow control, and 0 ms timeout).
*/
void QextSerialPort::construct()
{
    lastErr = E_NO_ERROR;
    Settings.BaudRate = BAUD115200;
    Settings.DataBits = DATA_8;
    Settings.Parity = PAR_NONE;
    Settings.StopBits = STOP_1;
    Settings.FlowControl = FLOW_HARDWARE;
    Settings.Timeout_Millisec = 500;
    mutex = new qbx::RecursiveMutex();
    setOpenMode(QIODevice::NotOpen);
}

void QextSerialPort::setQueryMode(QueryMode mechanism)
{
    _queryMode = mechanism;
}

/*!
Sets the name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/
void QextSerialPort::setPortName(const QString& name)
{
#ifdef Q_OS_WIN
    qDebug() << "QextSerialPort::setPortName" << name;
    port = fullPortNameWin(name);
#else
    port = name;
#endif
    qDebug() << "QextSerialPort::setPortName to" << port;
}

/*!
Returns the name set by setPortName().
*/
QString QextSerialPort::portName() const
{
    return port;
}

/*!
  Reads all available data from the device, and returns it as a QByteArray.
  This function has no way of reporting errors; returning an empty QByteArray()
  can mean either that no data was currently available for reading, or that an error occurred.
*/
QByteArray QextSerialPort::readAll()
{
    int avail = this->bytesAvailable();
    return (avail > 0) ? this->read(avail) : QByteArray();
}

/*!
Returns the baud rate of the serial port.  For a list of possible return values see
the definition of the enum BaudRateType.
*/
BaudRateType QextSerialPort::baudRate(void) const
{
    return Settings.BaudRate;
}

/*!
Returns the number of data bits used by the port.  For a list of possible values returned by
this function, see the definition of the enum DataBitsType.
*/
DataBitsType QextSerialPort::dataBits() const
{
    return Settings.DataBits;
}

/*!
Returns the type of parity used by the port.  For a list of possible values returned by
this function, see the definition of the enum ParityType.
*/
ParityType QextSerialPort::parity() const
{
    return Settings.Parity;
}

/*!
Returns the number of stop bits used by the port.  For a list of possible return values, see
the definition of the enum StopBitsType.
*/
StopBitsType QextSerialPort::stopBits() const
{
    return Settings.StopBits;
}

/*!
Returns the type of flow control used by the port.  For a list of possible values returned
by this function, see the definition of the enum FlowType.
*/
FlowType QextSerialPort::flowControl() const
{
    return Settings.FlowControl;
}

/*!
Returns true if device is sequential, otherwise returns false. Serial port is sequential device
so this function always returns true. Check QIODevice::isSequential() documentation for more
information.
*/
bool QextSerialPort::isSequential() const
{
    return true;
}

QString QextSerialPort::errorString()
{
    switch (lastErr)
    {
    case E_NO_ERROR: return "No Error has occurred";
    case E_INVALID_FD: return "Invalid file descriptor (port was not opened correctly)";
    case E_NO_MEMORY: return "Unable to allocate memory tables (POSIX)";
    case E_CAUGHT_NON_BLOCKED_SIGNAL: return "Caught a non-blocked signal (POSIX)";
    case E_PORT_TIMEOUT: return "Operation timed out (POSIX)";
    case E_INVALID_DEVICE: return "The file opened by the port is not a valid device";
    case E_BREAK_CONDITION: return "The port detected a break condition";
    case E_FRAMING_ERROR: return "The port detected a framing error (usually caused by incorrect baud rate settings)";
    case E_IO_ERROR: return "There was an I/O error while communicating with the port";
    case E_BUFFER_OVERRUN: return "Character buffer overrun";
    case E_RECEIVE_OVERFLOW: return "Receive buffer overflow";
    case E_RECEIVE_PARITY_ERROR: return "The port detected a parity error in the received data";
    case E_TRANSMIT_OVERFLOW: return "Transmit buffer overflow";
    case E_READ_FAILED: return "General read operation failure";
    case E_WRITE_FAILED: return "General write operation failure";
    case E_FILE_NOT_FOUND: return "The " + this->portName() + " file doesn't exists";
    default: return QString("Unknown error: %1").arg(lastErr);
    }
}

/*!
Sets the baud rate of the serial port.  For a list of possible values returned by
this function, see the definition of the enum BaudRateType.
*/
void QextSerialPort::setBaudRate(BaudRateType baudRate)
{
    Settings.BaudRate = baudRate;
    if (isOpen()) {
        platformSpecificInit();
    }
}

/*!
Sets the number of data bits used by the serial port.  For a list of possible values returned by
this function, see the definition of the enum DataBitsType.
*/
void QextSerialPort::setDataBits(DataBitsType dataBits)
{
    Settings.DataBits = dataBits;
    if (isOpen()) {
        platformSpecificInit();
    }
}

/*!
Sets the parity of the serial port.  For a list of possible values returned by
this function, see the definition of the enum ParityType.
*/
void QextSerialPort::setParity(ParityType parity)
{
    Settings.Parity = parity;
    if (isOpen()) {
        platformSpecificInit();
    }
}

/*!
Sets the number of stop bits used by the serial port.  For a list of possible values returned by
this function, see the definition of the enum StopBitsType.
*/
void QextSerialPort::setStopBits(StopBitsType stopBits)
{
    Settings.StopBits = stopBits;
    if (isOpen()) {
        platformSpecificInit();
    }
}

/*!
Sets the flow control used by the serial port.  For a list of possible values returned by
this function, see the definition of the enum FlowType.
*/
void QextSerialPort::setFlowControl(FlowType flow)
{
    Settings.FlowControl = flow;
    if (isOpen()) {
        platformSpecificInit();
    }
}

/*!
Sets the read and write timeouts for the port to millisec milliseconds.
Note that this is a per-character timeout, i.e. the port will wait this long for each
individual character, not for the whole read operation.
*/
void QextSerialPort::setTimeout(long millisec)
{
    Settings.Timeout_Millisec = millisec;
}

/*!
Standard destructor.
*/
QextSerialPort::~QextSerialPort()
{
    if (isOpen()) {
        close();
    }
    platformSpecificDestruct();
    delete mutex;
}

void QextSerialPort::platformSpecificDestruct() {}

/*!
Windows-specific function to convert port name to full Windows port name.
Converts "COM1" to "\\.\COM1" for proper Windows serial port access.
*/
QString QextSerialPort::fullPortNameWin(const QString & name)
{
    QString fullName = name;
    
    // If the name doesn't start with \\.\, add it
    if (!fullName.startsWith("\\\\.\\")) {
        if (fullName.startsWith("COM")) {
            fullName = "\\\\.\\" + fullName;
        }
    }
    
    return fullName;
}

/*!
Platform-specific initialization function.
*/
void QextSerialPort::platformSpecificInit()
{
#ifdef Q_OS_WIN
    // Windows-specific initialization
    Win_Handle = INVALID_HANDLE_VALUE;
    eventMask = 0;
    _bytesToWrite = 0;
    bytesToWriteLock = new QReadWriteLock();
    pendingWrites.clear();
    
    // Initialize OVERLAPPED structure
    memset(&overlap, 0, sizeof(OVERLAPPED));
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
    // Unix/Linux initialization
    fd = 0;
#endif
}

/*!
Opens the serial port with the specified mode.
*/
bool QextSerialPort::open(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode)
    
    if (isOpen()) {
        return true;
    }
    
#ifdef Q_OS_WIN
    // Windows implementation
    Win_Handle = CreateFileA(port.toLocal8Bit().constData(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                            NULL);
    
    if (Win_Handle == INVALID_HANDLE_VALUE) {
        lastErr = E_INVALID_DEVICE;
        return false;
    }
    
    // Configure serial port settings
    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    
    if (!GetCommState(Win_Handle, &dcb)) {
        CloseHandle(Win_Handle);
        Win_Handle = INVALID_HANDLE_VALUE;
        lastErr = E_INVALID_DEVICE;
        return false;
    }
    
    // Set baud rate
    switch (Settings.BaudRate) {
        case BAUD110:    dcb.BaudRate = CBR_110; break;
        case BAUD300:    dcb.BaudRate = CBR_300; break;
        case BAUD600:    dcb.BaudRate = CBR_600; break;
        case BAUD1200:   dcb.BaudRate = CBR_1200; break;
        case BAUD2400:   dcb.BaudRate = CBR_2400; break;
        case BAUD4800:   dcb.BaudRate = CBR_4800; break;
        case BAUD9600:   dcb.BaudRate = CBR_9600; break;
        case BAUD14400:  dcb.BaudRate = CBR_14400; break;
        case BAUD19200:  dcb.BaudRate = CBR_19200; break;
        case BAUD38400:  dcb.BaudRate = CBR_38400; break;
        case BAUD56000:  dcb.BaudRate = CBR_56000; break;
        case BAUD57600:  dcb.BaudRate = CBR_57600; break;
        case BAUD115200: dcb.BaudRate = CBR_115200; break;
        default: dcb.BaudRate = CBR_115200; break;
    }
    
    // Set data bits
    switch (Settings.DataBits) {
        case DATA_5: dcb.ByteSize = 5; break;
        case DATA_6: dcb.ByteSize = 6; break;
        case DATA_7: dcb.ByteSize = 7; break;
        case DATA_8: dcb.ByteSize = 8; break;
        default: dcb.ByteSize = 8; break;
    }
    
    // Set parity
    switch (Settings.Parity) {
        case PAR_NONE: dcb.Parity = NOPARITY; dcb.fParity = FALSE; break;
        case PAR_ODD:  dcb.Parity = ODDPARITY; dcb.fParity = TRUE; break;
        case PAR_EVEN: dcb.Parity = EVENPARITY; dcb.fParity = TRUE; break;
        default: dcb.Parity = NOPARITY; dcb.fParity = FALSE; break;
    }
    
    // Set stop bits
    switch (Settings.StopBits) {
        case STOP_1: dcb.StopBits = ONESTOPBIT; break;
        case STOP_2: dcb.StopBits = TWOSTOPBITS; break;
        default: dcb.StopBits = ONESTOPBIT; break;
    }
    
    // Set flow control
    switch (Settings.FlowControl) {
        case FLOW_OFF:     dcb.fOutxCtsFlow = FALSE; dcb.fRtsControl = RTS_CONTROL_DISABLE; dcb.fOutX = FALSE; dcb.fInX = FALSE; break;
        case FLOW_HARDWARE: dcb.fOutxCtsFlow = TRUE; dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; dcb.fOutX = FALSE; dcb.fInX = FALSE; break;
        case FLOW_XONXOFF: dcb.fOutxCtsFlow = FALSE; dcb.fRtsControl = RTS_CONTROL_DISABLE; dcb.fOutX = TRUE; dcb.fInX = TRUE; break;
        default: dcb.fOutxCtsFlow = FALSE; dcb.fRtsControl = RTS_CONTROL_DISABLE; dcb.fOutX = FALSE; dcb.fInX = FALSE; break;
    }
    
    if (!SetCommState(Win_Handle, &dcb)) {
        CloseHandle(Win_Handle);
        Win_Handle = INVALID_HANDLE_VALUE;
        lastErr = E_INVALID_DEVICE;
        return false;
    }
    
    // Set timeouts
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = Settings.Timeout_Millisec;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = Settings.Timeout_Millisec;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(Win_Handle, &timeouts);
    
    lastErr = E_NO_ERROR;
    setOpenMode(mode);
    return true;
#else
    // Unix/Linux implementation would go here
    lastErr = E_INVALID_DEVICE;
    return false;
#endif
}

/*!
Closes the serial port.
*/
void QextSerialPort::close()
{
    if (isOpen()) {
#ifdef Q_OS_WIN
        if (Win_Handle != INVALID_HANDLE_VALUE) {
            CloseHandle(Win_Handle);
            Win_Handle = INVALID_HANDLE_VALUE;
        }
#endif
        setOpenMode(QIODevice::NotOpen);
    }
}

/*!
Returns the size of the device.
*/
qint64 QextSerialPort::size() const
{
    return 0; // Serial ports don't have a size
}

/*!
Returns the number of bytes available for reading.
*/
qint64 QextSerialPort::bytesAvailable() const
{
    if (!isOpen()) {
        return 0;
    }
    
#ifdef Q_OS_WIN
    DWORD errors;
    COMSTAT status;
    if (ClearCommError(Win_Handle, &errors, &status)) {
        return status.cbInQue;
    }
#endif
    return 0;
}

/*!
Returns the number of bytes waiting to be written.
*/
qint64 QextSerialPort::bytesToWrite() const
{
    if (!isOpen()) {
        return 0;
    }
    
#ifdef Q_OS_WIN
    return _bytesToWrite;
#else
    return 0;
#endif
}

/*!
Waits for ready read with timeout.
*/
bool QextSerialPort::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs)
    // Basic implementation - could be enhanced with proper event handling
    return false;
}

/*!
Reads data from the serial port.
*/
qint64 QextSerialPort::readData(char *data, qint64 maxSize)
{
    if (!isOpen()) {
        return -1;
    }
    
#ifdef Q_OS_WIN
    DWORD bytesRead = 0;
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    if (ReadFile(Win_Handle, data, maxSize, &bytesRead, &overlapped)) {
        CloseHandle(overlapped.hEvent);
        return bytesRead;
    }
    
    CloseHandle(overlapped.hEvent);
    lastErr = E_READ_FAILED;
    return -1;
#else
    lastErr = E_READ_FAILED;
    return -1;
#endif
}

/*!
Writes data to the serial port.
*/
qint64 QextSerialPort::writeData(const char *data, qint64 maxSize)
{
    if (!isOpen()) {
        return -1;
    }
    
#ifdef Q_OS_WIN
    DWORD bytesWritten = 0;
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    if (WriteFile(Win_Handle, data, maxSize, &bytesWritten, &overlapped)) {
        CloseHandle(overlapped.hEvent);
        return bytesWritten;
    }
    
    CloseHandle(overlapped.hEvent);
    lastErr = E_WRITE_FAILED;
    return -1;
#else
    lastErr = E_WRITE_FAILED;
    return -1;
#endif
}

/*!
Windows event handler.
*/
void QextSerialPort::onWinEvent(void*)
{
    // Windows event handling implementation
    // This would typically handle serial port events
}