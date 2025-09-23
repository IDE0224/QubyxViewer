#include "displays.h"
#include "SystemInfo.h"

#include <QtCore/qglobal.h>
#include <QString>
#include <QDebug>
#include <QubyxConfiguration.h>

#include <string.h>

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0600
#define UNICODE


#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <winnt.h>
#include <bits/stringfwd.h>

#include "UsefulTools.h"
#include "WinRegistry.h"
#include <QThread>

#define SPI_GETSCREENSAVESECURE   0x0076

namespace OSUtils {

    HWND hWnd = HWND_BROADCAST;

    void turnoffdisplay()
    {
        // blocked by unexpected system sleep on modern OS
        return;


        qDebug() << "turnoffdisplay()";
        if (screensaverIsProtected())
        {
            qDebug() << "screensaver is password-protected - skip";
            return;
        }

        if (hWnd != HWND_BROADCAST)
            DestroyWindow(hWnd);

        char szClassNameMain[] = "PowerOffDlg";
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEXA wcex;
        wcex.cbSize = sizeof (WNDCLASSEXA);
        wcex.style = 0; //CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DefWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = szClassNameMain;
        wcex.hIconSm = LoadIconA(wcex.hInstance, "");

        ATOM atomMain = RegisterClassExA(&wcex);
        Q_UNUSED(atomMain)

        hWnd = CreateWindowA(szClassNameMain, szClassNameMain, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);
        SendMessage(hWnd, WM_SYSCOMMAND, SC_MONITORPOWER, 2);

        //SendMessage(HWND_TOPMOST/*HWND_BROADCAST*/, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
    }

    void turnondisplay()
    {
        // blocked by unexpected system sleep on modern OS
        return;

        qDebug() << "turnondisplay()";
        if (screensaverIsProtected())
        {
            qDebug() << "screensaver is password-protected - skip";
            return;
        }

        if (hWnd != HWND_BROADCAST)
            DestroyWindow(hWnd);

        char szClassNameMain[] = "PowerOffDlg";
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASSEXA wcex;
        wcex.cbSize = sizeof (WNDCLASSEXA);
        wcex.style = 0; //CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DefWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = szClassNameMain;
        wcex.hIconSm = LoadIconA(wcex.hInstance, "");

        ATOM atomMain = RegisterClassExA(&wcex);

        hWnd = CreateWindowA(szClassNameMain, szClassNameMain, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);
        SendMessage(hWnd, WM_SYSCOMMAND, SC_MONITORPOWER, -1);

        POINT pt;
        GetCursorPos(&pt);
        SetCursorPos(pt.x + 1, pt.y + 1);

        double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
        double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
        double fx = pt.x * (65535.0f / fScreenWidth);
        double fy = pt.y * (65535.0f / fScreenHeight);
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        input.mi.dx = fx;
        input.mi.dy = fy;
        ::SendInput(1, &input, sizeof (INPUT));

        //SendMessage(HWND_TOPMOST/*HWND_BROADCAST*/, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
    }

    void blocksleep() {
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, nullptr, SPIF_SENDWININICHANGE);
        SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
        stopScreensaver();
    }

    DEFINE_GUID(GUID_CLASS_MONITOR, 0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);

    void takedisplays(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
            std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey, std::vector<QString> &videoadaptername) {
        geometry.clear();
        resolution.clear();
        edids.clear();
        osIds.clear();
        osDevNames.clear();

        std::vector<QRect> tgeometry;
        std::vector<QSize> tresolution;
        std::vector<QByteArray> tedids;
        std::vector<QString> tosIds, tosDevNames, tvideoadapterkey, tvideoadaptername;


        DISPLAY_DEVICE dd;
        dd.cb = sizeof (dd);
        DWORD dev = 0, devMon;
        DEVMODE devmode;

        unsigned char EDID[1024];
        int itop, ileft, iwidth, iheight;
        QString iosIds, iosDevs;

        while (EnumDisplayDevices(nullptr, dev++, &dd, 0)) {
            //qDebug() << "EnumDisplayDevices. " << QString::fromStdWString(std::wstring(dd.DeviceString)) << QString::number(dd.StateFlags, 16);
            if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
                wchar_t device[2048];
                wcscpy(device, dd.DeviceName);
                devMon = 0;

                //            qDebug() << "EnumDisplayDevices. device        = "<<QString::fromStdWString(std::wstring(device));
                //            qDebug() <<"                     Device String = "<<QString::fromStdWString(std::wstring(dd.DeviceString));
                //            qDebug() <<"                     Key           = "<<QString::fromStdWString(std::wstring(dd.DeviceKey));
                //            qDebug() <<"                     Name          = "<<QString::fromStdWString(std::wstring(dd.DeviceName));

                QString deviceStr = QString::fromStdWString(std::wstring(dd.DeviceKey));

                deviceStr.chop(deviceStr.length() - deviceStr.indexOf("}", 0));
                deviceStr.remove(0, deviceStr.indexOf("{", 0) + 1);

                QString gDeviceKey = deviceStr;
                QString gDeviceName = QString::fromStdWString(std::wstring(dd.DeviceString));

                //            qDebug()<<"Graphic - "<<gDeviceKey;
                //            qDebug()<<"Graphic - "<<gDeviceName;

                QThread::msleep(200);
                while (EnumDisplayDevices(device, devMon++, &dd, 0)) {
                    //                qDebug() << "--EnumDisplayDevices 2. flags=" << QString::number(dd.StateFlags, 16) 
                    //                        << QString::fromStdWString(std::wstring(dd.DeviceID))
                    //                        << QString::fromStdWString(std::wstring(dd.DeviceString))
                    //                        << QString::fromStdWString(std::wstring(dd.DeviceName));
                    bool normal = (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) && (dd.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER);
                    //if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) && (dd.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER))
                    {
                        int edidsize = 0;

                        QString devid = QString::fromStdWString(std::wstring(dd.DeviceID));
                        devid = devid.right(devid.length() - devid.indexOf("\\", devid.indexOf("\\") + 1) - 1);

                        //                    qDebug() << "--EnumDisplayDevices 2. flags=" << QString::number(dd.StateFlags, 16);
                        //                    qDebug() << "--EnumDisplayDevices 2. devid=" << devid;

                        memset(&devmode, 0, sizeof (DEVMODE));
                        devmode.dmSize = sizeof (DEVMODE);
                        if (!EnumDisplaySettingsEx(device, ENUM_CURRENT_SETTINGS, &devmode, 0))
                            if (!EnumDisplaySettingsEx(device, ENUM_REGISTRY_SETTINGS, &devmode, 0)) {
                                qDebug() << "--EnumDisplayDevices 2. Check failed";
                                continue;
                            }

                        //                   qDebug() << "--EnumDisplayDevices 2. Check passed";

                        itop = devmode.dmPosition.y;
                        ileft = devmode.dmPosition.x;
                        iwidth = devmode.dmPelsWidth;
                        iheight = devmode.dmPelsHeight;
                        iosIds = QString::fromStdWString(std::wstring(device));
                        iosDevs = QString::fromStdWString(std::wstring(dd.DeviceID));


                        //                    qDebug() << "--EnumDisplayDevices 2. position=" << ileft << itop;

                        //!!!!!!!!!!
                        //read EDID
                        HDEVINFO devInfo = nullptr;
                        SP_DEVINFO_DATA devInfoData;
                        memset(&devInfoData, 0, sizeof (devInfoData));
                        devInfoData.cbSize = sizeof (devInfoData);

                        devInfo = SetupDiGetClassDevsEx(
                                &GUID_CLASS_MONITOR, //class GUID
                                nullptr, //enumerator
                                nullptr, //HWND
                                DIGCF_PRESENT, // Flags //DIGCF_ALLCLASSES|
                                nullptr, // device info, create a new one.
                                nullptr, // machine name, local machine
                                nullptr);

                        if (devInfo) {
                            int ii = 0;
                            WCHAR instanceid[2048];
                            instanceid[0] = 0;
                            while (SetupDiEnumDeviceInfo(devInfo, ii++, &devInfoData))
                                if (SetupDiGetDeviceRegistryProperty(devInfo, &devInfoData, SPDRP_DRIVER, nullptr, (BYTE*) instanceid, sizeof (instanceid), nullptr))
                                    if (devid == QString::fromStdWString(std::wstring(instanceid))) {
                                        //                                    qDebug() << "----SetupDiEnumDeviceInfo. After check";

                                        HKEY hDevRegKey;
                                        hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
                                        if (hDevRegKey) {
                                            CHAR CHE[1024];
                                            DWORD CHL = 1024;
                                            DWORD type;
                                            if (RegQueryValueExA(hDevRegKey, "EDID", nullptr, &type, (BYTE*) CHE, &CHL) == ERROR_SUCCESS) {
                                                edidsize = CHL;
                                                memcpy(EDID, CHE, edidsize);
                                            }
                                        }
                                        //                                    qDebug() << "----SetupDiEnumDeviceInfo. EDID size=" << edidsize;
                                        break;
                                    }

                            SetupDiDestroyDeviceInfoList(devInfo);
                        }
                        if (normal) {
                            geometry.push_back(QRect(ileft, itop, iwidth, iheight));
                            resolution.push_back(QSize(iwidth, iheight));
                            edids.push_back(QByteArray((char*) EDID, edidsize));
                            osIds.push_back(iosIds);
                            osDevNames.push_back(iosDevs);
                            videoadapterkey.push_back(gDeviceKey);
                            videoadaptername.push_back(gDeviceName);

                            qDebug() << "Write info from register read:" << (char*)EDID << "_" << QString::fromLatin1((char*)EDID)
                                     << "_" << QByteArray((char*) EDID, edidsize).data();
                            QStringList list;
                            for (auto it : videoadapterkey)
                                list.push_back(it);
                            qDebug() << "vak" << list;
                            list.clear();
                            for (auto it : videoadaptername)
                                list.push_back(it);
                            qDebug() << "van" << list;
                            //                        qDebug() << "--EnumDisplayDevices 2. Normal Added";
                        } else {
                            if (edidsize) {
                                tgeometry.push_back(QRect(ileft, itop, iwidth, iheight));
                                tresolution.push_back(QSize(iwidth, iheight));
                                tedids.push_back(QByteArray((char*) EDID, edidsize));
                                tosIds.push_back(iosIds);
                                tosDevNames.push_back(iosDevs);
                                tvideoadapterkey.push_back(gDeviceKey);
                                tvideoadaptername.push_back(gDeviceName);
                                //                            qDebug() << "--EnumDisplayDevices 2. Added as Not Attached";
                            }
                        }
                    }
                }
            }
        }

        if (geometry.empty() && !tgeometry.empty()) {
            geometry = tgeometry;
            resolution = tresolution;
            edids = tedids;
            osIds = tosIds;
            osDevNames = tosDevNames;
            videoadapterkey = tvideoadapterkey;
            videoadaptername = tvideoadaptername;
        }
    }

    bool screensaverIsRunning() {
        BOOL pvParam = FALSE;
        SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &pvParam, 0);
        return pvParam;
    }

    bool sleepIsProtected()
    {
        qDebug() << "sleepIsProdected()";

        HKEY hKey;
        DWORD dwValue = 0;
        DWORD dwSize = sizeof(DWORD);
        LONG lResult;

        // Open the registry key
        lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), 0, KEY_READ, &hKey);
        if (lResult != ERROR_SUCCESS)
        {
            qDebug() << "Impossible to open Control Panel\\Desktop";
            return false;
        }

        // Query the DWORD value
        lResult = RegQueryValueEx(hKey, TEXT("DelayLockInterval"), NULL, NULL, (LPBYTE)&dwValue, &dwSize);
        if (lResult != ERROR_SUCCESS)
        {
            qDebug() << "Impossible to open DelayLockInterval";
            return false;
        }

        // Close the registry key
        RegCloseKey(hKey);

        qDebug() << "dwValue" << dwValue;

        bool locked = dwValue <= 300; // check if system will lock in 5 minutes or less
        qDebug() << "Sleep locked" << locked;
        return locked;
    }

    bool screensaverIsProtected() {
        BOOL pvParam = FALSE;
        SystemParametersInfo(SPI_GETSCREENSAVESECURE, 0, &pvParam, 0);
        bool screensaverProtected = pvParam;

        qDebug() << "screensaver protected" << screensaverProtected;

        bool sleepProtected = sleepIsProtected();
        return screensaverProtected || sleepProtected;
    }

    BOOL CALLBACK KillScreenSaverFunc(HWND hwnd, LPARAM lParam) {
        if (IsWindowVisible(hwnd))
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        return TRUE;
    }

    void stopScreensaver()
    {
        //qDebug() << "stopScreensaver()";
        if (!screensaverIsRunning() || screensaverIsProtected() )
            return;

        //qDebug() << "Screensaver is running";
        
        HDESK hdesk; 
        hdesk = OpenDesktop(TEXT("Screen-saver"), 0, FALSE, DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS); 
        if (hdesk) 
        { 
            EnumDesktopWindows(hdesk, KillScreenSaverFunc, 0);
            CloseDesktop(hdesk); 
        } 
        else 
        { 
            if (screensaverIsRunning() )
                KillScreenSaverFunc(GetForegroundWindow(), 0); 
        } 
    }

}

#endif

#ifdef Q_OS_MAC

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/graphics/IOFramebufferShared.h>
#include <IOKit/graphics/IOGraphicsInterface.h>
#include <IOKit/graphics/IOGraphicsTypes.h>
#include <IOKit/graphics/IOGraphicsLib.h>

#include <sys/sysctl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


#include <unistd.h>

#include <algorithm>
#include <QDebug>

#include <QProcess>
#include <QStringlist>
#include "QubyxConfiguration.h"

#include "mac_displays_helpers.h"

void printKeys (const void* key, const void* value, void* context) {
        CFShow(key);
}

void iterate_dict(CFDictionaryRef dict)
{
    CFDictionaryApplyFunction(dict, printKeys, NULL);
}

namespace OSUtils {

    void takedisplays(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
                      std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey,  std::vector<QString> &videoadaptername)
    {
        geometry.clear();
        resolution.clear();
        edids.clear();
        osIds.clear();
        osDevNames.clear();

        short MAXDISPLAYS = 8;
        CGDisplayCount theCGDisplayCount;
        CGDirectDisplayID onlineDisplays[MAXDISPLAYS];
        unsigned int display_loop;
        CGDisplayErr error;

        unsigned char EDID[256];
        int itop, ileft, iwidth, iheight;

        error = CGGetOnlineDisplayList(MAXDISPLAYS, onlineDisplays, &theCGDisplayCount);
        if (error != kCGErrorSuccess) {
            return; // false;
        }

        QMap<unsigned, double> ratios;
#ifdef USE_MM
         ratios = getPixelRatios();
#endif

        for (display_loop = 0; display_loop < theCGDisplayCount; display_loop++) {
            io_service_t service;
            CFDictionaryRef dict;

            service = IOServicePortFromCGDisplayID(onlineDisplays[display_loop]);
            dict = IODisplayCreateInfoDictionary(service, kNilOptions);

            qDebug() << "displayid: " << int(onlineDisplays[display_loop]);
            qDebug() << "service: " << int(service);
            qDebug() << "dict: " << (void*) dict;
            int size = 0;


            if (dict && CFDictionaryContainsKey(dict, CFSTR(kIODisplayEDIDKey))) {
                CFDataRef ref = (CFDataRef) CFDictionaryGetValue(dict, CFSTR(kIODisplayEDIDKey));
                size = std::min<int>(256, CFDataGetLength(ref));
                qDebug() << "EDID size: " << size;
                CFDataGetBytes(ref, CFRangeMake(0, size), EDID);
            }
            else
            {
                qDebug() << " no kIODisplayEDIDKey key";
                iterate_dict(dict);
            }

            if (!size) {
                CFDataRef edid;
                edid = (CFDataRef) IORegistryEntrySearchCFProperty(service, kIOServicePlane,
                        CFSTR(kIODisplayEDIDKey),
                        kCFAllocatorDefault,
                        kIORegistryIterateRecursively | kIORegistryIterateParents);
                if (edid) {
                    size = std::min<int>(256, CFDataGetLength(edid));
                    qDebug() << "Registry EDID size: " << size;
                    CFDataGetBytes(edid, CFRangeMake(0, size), EDID);
                    CFRelease(edid);
                }
                else
                {
                    qDebug() << "IORegistryEntrySearchCFProperty failed";
                }
            }


            QByteArray edid;
            if (size)
                edid = QByteArray((char*) (EDID), size);
            
            CGRect rect = CGDisplayBounds(onlineDisplays[display_loop]);
            itop = rect.origin.y;
            ileft = rect.origin.x;
            iwidth = rect.size.width;
            iheight = rect.size.height;
            geometry.push_back(QRect(ileft, itop, iwidth, iheight));

            if (ratios.find(onlineDisplays[display_loop])!=ratios.end())
            {
                double ratio = ratios[onlineDisplays[display_loop]];
                iwidth *= ratio;
                iheight *= ratio;
            }
            resolution.push_back(QSize(iwidth, iheight));

            edids.push_back(edid);
            osIds.push_back(QString::number(onlineDisplays[display_loop]));
            osDevNames.push_back("");

            QString cardKey;
            QString cardName;

            MacGetCardInfo(edid, cardKey, cardName);

            videoadapterkey.push_back(cardKey);
            videoadaptername.push_back(cardName);

            qDebug() << "Set key  - " << cardKey;
            qDebug() << "Set name - " << cardName;

                       

            CFRelease(dict);
        }


    }

    io_service_t IOServicePortFromCGDisplayID(CGDirectDisplayID displayID)
    {
        qDebug() << "IMacDisplay::IOServicePortFromCGDisplayID";
        io_iterator_t iter;
        io_service_t serv, servicePort = 0;

        CFMutableDictionaryRef matching = IOServiceMatching("IODisplayConnect");

        // releases matching for us
        kern_return_t err = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                 matching,
                                 &iter);
        if (err)
        {
            qDebug() << "err = " << err;
            return 0;
        }

        uint32_t vendor = CGDisplayVendorNumber(displayID);
        uint32_t model = CGDisplayModelNumber(displayID);
        uint32_t sn = CGDisplaySerialNumber(displayID);


        qDebug() << "search display for:";
        qDebug() << "vendor - " << vendor;
        qDebug() << "model - " << model;
        qDebug() << "serial - " << sn;

        qDebug() << "check for same vendor/model/serial, e.g. Totoku";
        short MAXDISPLAYS = 8;
        CGDisplayCount theCGDisplayCount;
        CGDirectDisplayID onlineDisplays[MAXDISPLAYS];
        CGError error = CGGetOnlineDisplayList(MAXDISPLAYS, onlineDisplays, &theCGDisplayCount);
        if (error != kCGErrorSuccess)
            theCGDisplayCount = 0;
        for (unsigned display_loop = 0; display_loop < theCGDisplayCount; display_loop++)
        {
//            if (onlineDisplays[display_loop]==displayID)
//                continue;

            uint32_t nvendor = CGDisplayVendorNumber(onlineDisplays[display_loop]);
            uint32_t nmodel = CGDisplayModelNumber(onlineDisplays[display_loop]);
            uint32_t nsn = CGDisplaySerialNumber(onlineDisplays[display_loop]);

            if (nvendor==vendor
               && nmodel==model
               && nsn==sn)
            {
                qDebug() << "display with the same vendor/model/sn found - drop to deprecated iokit search";
                return CGDisplayIOServicePort(displayID);
            }
        }


        while ((serv = IOIteratorNext(iter)) != 0)
        {
            qDebug() << "while ((serv = IOIteratorNext(iter)) != 0)";
            CFDictionaryRef info;
            CFIndex vendorID, productID, serialID;
            CFNumberRef vendorIDRef, productIDRef, snRef;
            Boolean success;

            info = IODisplayCreateInfoDictionary(serv,
                                 kIODisplayOnlyPreferredName);

//            CFShow(info);
            vendorIDRef = (CFNumberRef)CFDictionaryGetValue(info, CFSTR(kDisplayVendorID));
            productIDRef = (CFNumberRef)CFDictionaryGetValue(info, CFSTR(kDisplayProductID));
            snRef = (CFNumberRef)CFDictionaryGetValue(info, CFSTR(kDisplaySerialNumber));

            success = CFNumberGetValue(vendorIDRef, kCFNumberCFIndexType,
                                       &vendorID);
            qDebug() << "success" << success;
            success &= CFNumberGetValue(productIDRef, kCFNumberCFIndexType,
                                        &productID);
            qDebug() << "success" << success;
            if (snRef)
            {
                success &= CFNumberGetValue(snRef, kCFNumberCFIndexType,
                                            &serialID);
                qDebug() << "success after serial" << success;
            }
            else
            {
                serialID = 0;
                qDebug() << "no builtin serial";
            }
            qDebug() << "vendorId" << vendorID;
            qDebug() << "productId" << productID;
            qDebug() << "serialId" << serialID;


            if (!success)
            {
                CFRelease(info);
                continue;
            }

            if (vendor != vendorID ||
                model != productID ||
                sn != serialID)
            {
                qDebug() << "value not match";
                CFRelease(info);
                continue;
            }

            // we're a match
            servicePort = serv;
            CFRelease(info);
            qDebug() << "value match";
            break;

        }

        IOObjectRelease(iter);
        qDebug() << "servicePort" <<servicePort;
        return servicePort;
    }

    void turnoffdisplay() {
        io_registry_entry_t reg = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/IOResources/IODisplayWrangler");
        if (reg) {
            IORegistryEntrySetCFProperty(reg, CFSTR("IORequestIdle"), kCFBooleanTrue);
            IOObjectRelease(reg);
        }
    }

    void turnondisplay() {
        io_registry_entry_t reg = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/IOResources/IODisplayWrangler");
        if (reg) {
            IORegistryEntrySetCFProperty(reg, CFSTR("IORequestIdle"), kCFBooleanFalse);
            IOObjectRelease(reg);
        }
    }

    void blocksleep() {
        UpdateSystemActivity(OverallAct);
        //system("defaults -currentHost write com.apple.screensaver idleTime 0");
        stopScreensaver();
    }

    QString MacDictionaryRead() {
        QProcess proc;
        QStringList args;

        args << "SPDisplaysDataType" << "-xml" << "-detailLevel" << "mini";
        proc.start("/usr/sbin/system_profiler", args);
        proc.waitForFinished();

        return proc.readAll();
    }

    void MacGetCardInfo(QByteArray &edid, QString &cardKey, QString &cardName)
    {
        qDebug() << "mac 1";
        static QString dictdata = MacDictionaryRead();

        bool res = extractMacVideoCardFromDictionary(dictdata, (unsigned char*)edid.data(), edid.size(), cardKey, cardName);
        if (!res)
        {
            cardKey = "unknown";
            cardName = "Unknown";
        }
    }
    
    bool screensaverIsRunning()
    {
        return pidByProcessName("ScreenSaverEngin") != -1;
    }

    bool screensaverIsProtected()
    {
        return false;
    }

    void stopScreensaver()
    {
        // kill process: /System/Library/Frameworks/ScreenSaver.framework/Resources/ScreenSaverEngine.app/Contents/MacOS/ScreenSaverEngine
        int pid = pidByProcessName("ScreenSaverEngin");
        if (pid != -1)
            kill(pid, SIGTERM);
        
        turnondisplay();
    }

}//end of namespace
#endif


namespace OSUtils
{
const QString VIDEOCARD_ID_KEY     = "spdisplays_device-id";
const QString VIDEOCARD_VENDOR_KEY = "spdisplays_vendor";
const QString VIDEOCARD_MODEL_KEY  = "sppci_model";
const QString DISPLAY_EDID_KEY     = "_spdisplays_edid";
const QString DISPLAY_EDID_KEY2     = "_IODisplayEDID";


bool checkEdidForVideoCard(QubyxConfiguration::iterator start, unsigned char *edid, unsigned edidSize)
{
    for (auto itr=start.firstchild();!itr.IsNull();++itr)
    {
        if (itr.value() == DISPLAY_EDID_KEY)
        {
            QByteArray ba = QByteArray::fromHex((++itr).value().trimmed().toUtf8());
            if (memcmp(edid, ba.data(), std::min<unsigned>(edidSize, ba.size())) == 0)
                return true;
        }

        if (itr.value() == DISPLAY_EDID_KEY2)
        {
            QByteArray ba = QByteArray::fromBase64((++itr).value().trimmed().toUtf8());
            if (memcmp(edid, ba.data(), std::min<unsigned>(edidSize, ba.size())) == 0)
                return true;
        }
    }

    for (auto itr=start.firstchild();!itr.IsNull();++itr)
        if (checkEdidForVideoCard(itr, edid, edidSize))
            return true;

    return false;
}

bool findVideoCard(QubyxConfiguration::iterator start, unsigned char *edid, unsigned edidSize, QString& key, QString& name)
{
    QString tk, tn;
    for (auto itr=start.firstchild();!itr.IsNull();++itr)
    {
        if (itr.value() == VIDEOCARD_ID_KEY)
            tk = (++itr).value();

        if (itr.value() == VIDEOCARD_MODEL_KEY || itr.value() == VIDEOCARD_VENDOR_KEY)
        {
            if (!tn.isEmpty())
                tn += " ";
            tn += (++itr).value();
        }
    }

    if ((!tk.isEmpty() || !tn.isEmpty())
       && checkEdidForVideoCard(start, edid, edidSize))
    {
        key = tk;
        name = tn;
        return true;
    }

    for (auto itr=start.firstchild();!itr.IsNull();++itr)
        if (itr.name()=="dict" || itr.name()=="array")
            if (findVideoCard(itr, edid, edidSize, key, name))
                return true;
    return false;
}

bool extractMacVideoCardFromDictionary(QString dict, unsigned char *edid, unsigned edidSize, QString& key, QString& name)
{
    QubyxConfiguration conf;
    conf.SetContent(dict);

    key = name = "";
    bool res = findVideoCard(conf.GetStartElement(), edid, edidSize, key, name);
    //in macos 10.9 edid has additional 0 byte on start
    if (!res)
        res = findVideoCard(conf.GetStartElement(), edid+1, edidSize-1, key, name);
    //also different check for macos 10.7
    if (!res)
    {
        unsigned char edid2[edidSize+1];
        edid2[0]=0;
        memcpy(edid2+1, edid, edidSize);
        res = findVideoCard(conf.GetStartElement(), edid2, edidSize+1, key, name);
    }
    return res;
}
}
