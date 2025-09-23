#include "profile.h"

#include <QtCore/qglobal.h>
#include <QDebug>


#ifdef Q_OS_WIN

#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <icm.h>

#include <sstream>

#include <QFile>
#include <QTemporaryFile>

#include "filehandling.h"

namespace OSProfile
{

void debugError()
{
    
    DWORD err = GetLastError();
    qDebug() << "?? Error code:" << err;

    char* Error = 0;
    if (!::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        nullptr,
                        err,
                        0,
                        (char*)&Error,
                        0,
                        nullptr) == 0)
    {
        qDebug() << "??Error:" << Error;
    }
    
    if ( Error )
    {
       ::LocalFree( Error );
       Error = 0;
    }


}

QString profilepath()
{
    DWORD sz = 4096;
    wchar_t buff[sz];
    
    if (!GetColorDirectoryW(nullptr, buff, &sz))
    {
        debugError();
        qDebug() << "??Windows GetColorDirectoryW failed";
        return "";
    }
    
    return QString::fromUtf16((ushort*)buff);
}

std::map<QString, QString> defaultProfiles_;

QString currentDisplayProfile(QString osid)
{
    if (defaultProfiles_.find(osid)!=defaultProfiles_.end())
        return defaultProfiles_[osid];
    
    //Get Device DC
    HDC hdc;
    hdc = CreateDCW((const wchar_t*)osid.utf16(), nullptr, nullptr, nullptr);
    if (!hdc)
    {
        qDebug() << "??Windows CreateDCW failed" << osid << GetLastError();
        return "";
    }
     
    DWORD sz = 4096;
    wchar_t filepath[sz];
    //Get profile for DC
    if (!GetICMProfileW(hdc, &sz, filepath))
    {
        debugError();
        qDebug() << "??Windows GetICMProfileW failed" << hdc;
        DeleteDC(hdc);
        return "";
    }
    
    DeleteDC(hdc);
    return QString::fromUtf16((ushort*)filepath);
}

typedef enum tagWCS_PROFILE_MANAGEMENT_SCOPE {
  WCS_PROFILE_MANAGEMENT_SCOPE_SYSTEM_WIDE,
  WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER 
} WCS_PROFILE_MANAGEMENT_SCOPE;

bool ApplyProfile(QString filepath, QString osid, QString osDevName)
{
    qDebug() << "ApplyProfile" << filepath << osid << osDevName;
    
    filepath = filepath.replace("/", "\\");
    qDebug() << "ApplyProfile fixed filepath" << filepath;
    
//    if (currentDisplayProfile(osid)==filepath)
//    {
//        qDebug() << "ApplyProfile. Profile already installed - stop";
//        return true;
//    }
    
    //Get Device DC
    HDC hdc;
    hdc = CreateDCW((const wchar_t*)osid.utf16(), nullptr, nullptr, nullptr);
    if (!hdc)
    {
        qDebug() << "??Windows CreateDCW failed" << osid << GetLastError();
        return false;
    }
    
    WORD gammaRamp[256*3];
    if (!GetDeviceGammaRamp(hdc, gammaRamp))
        qDebug() << "??Windows GetDeviceGammaRamp failed";
    
    static BOOL (WINAPI* pWcsAssociateColorProfileWithDevice)(WCS_PROFILE_MANAGEMENT_SCOPE,PCWSTR,PCWSTR) = nullptr;
    static BOOL (WINAPI* pWcsDisassociateColorProfileFromDevice)(WCS_PROFILE_MANAGEMENT_SCOPE,PCWSTR,PCWSTR) = nullptr;
    static BOOL (WINAPI* pWcsSetUsePerUserProfiles)(LPCWSTR,DWORD,BOOL) = nullptr;
    static BOOL (WINAPI* pWcsSetCalibrationManagementState)(BOOL) = nullptr;
    static bool vistadetecttried=false;
    
    if (!vistadetecttried)
    {
        vistadetecttried = true;
        pWcsAssociateColorProfileWithDevice = (BOOL (WINAPI*)(WCS_PROFILE_MANAGEMENT_SCOPE,PCWSTR,PCWSTR)) GetProcAddress(LoadLibraryA("mscms"), "WcsAssociateColorProfileWithDevice");
        pWcsDisassociateColorProfileFromDevice = (BOOL (WINAPI*)(WCS_PROFILE_MANAGEMENT_SCOPE,PCWSTR,PCWSTR)) GetProcAddress(LoadLibraryA("mscms"), "WcsDisassociateColorProfileFromDevice");
        pWcsSetUsePerUserProfiles = (BOOL (WINAPI*)(LPCWSTR,DWORD,BOOL))GetProcAddress(LoadLibraryA("mscms"), "WcsSetUsePerUserProfiles");
        pWcsSetCalibrationManagementState = (BOOL (WINAPI*)(BOOL))GetProcAddress(LoadLibraryA("mscms"), "WcsSetCalibrationManagementState");
    }
    
    //enable user icc list
    if (pWcsSetUsePerUserProfiles)
    {

        bool res = (*pWcsSetUsePerUserProfiles)((const wchar_t*)osDevName.utf16(), CLASS_MONITOR, TRUE);
        qDebug() << "ApplyProfile call WcsSetUsePerUserProfiles" << res;
        if (!res)
        {
            qDebug() << "ApplyProfile WcsSetUsePerUserProfiles. getLastError=" << GetLastError();
        }
    }
    
    //remove icc first
    {
        QString tmpfile;
        {
            QTemporaryFile f;

            if (f.open())
            {
                tmpfile = f.fileName();
                f.close();
            }
        }
        
        
        
        if (!tmpfile.isEmpty())
        {
            qDebug() << "ApplyProfile temp file:" << tmpfile;
            QFile::remove(tmpfile);
            if (QFile::copy(filepath, tmpfile))
            {
                qDebug() << "ApplyProfile try to disassociate";
                if (pWcsDisassociateColorProfileFromDevice)
                    (*pWcsDisassociateColorProfileFromDevice)(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER, 
                                                              (const wchar_t*)filepath.utf16(),
                                                              (const wchar_t*)osDevName.utf16());
                DisassociateColorProfileFromDeviceW(nullptr, (const wchar_t*)filepath.utf16(), (const wchar_t*)osDevName.utf16());
        
                UninstallColorProfileW(nullptr, (const wchar_t*)filepath.utf16(), TRUE);
                
                QFile::remove(filepath);
                QFile::copy(tmpfile, filepath);
                QFile::remove(tmpfile);
                OSUtils::setFullPermissions(filepath, true);
            }
        }
    }
      
    //Install profile to the list
    if (!InstallColorProfileW(nullptr, (const wchar_t*)filepath.utf16()))
    {
        debugError();
        qDebug() << "??Windows InstallColorProfileW failed" << filepath;
        DeleteDC(hdc);
        return false;
    }
    
    //Set as default
    bool res=false;
    if (pWcsAssociateColorProfileWithDevice)
    {
        qDebug() << "ApplyProfile use vista WcsAssociateColorProfileWithDevice";
        res = (*pWcsAssociateColorProfileWithDevice)(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER, 
                                                     (const wchar_t*)filepath.utf16(),
                                                     (const wchar_t*)osDevName.utf16());
    }
    else
        res = AssociateColorProfileWithDeviceW(nullptr, (const wchar_t*)filepath.utf16(), (const wchar_t*)osDevName.utf16());
//    bool r2 = AssociateColorProfileWithDeviceA(nullptr, filepath.c_str(), osDevName.c_str());
//    res = res || r2;
    if (!res)
    {
        debugError();
        qDebug() << "??Windows AssociateColorProfileWithDeviceW failed" << filepath << osDevName;
        DeleteDC(hdc);
        return false;
    }
           
    //Set profile to DC
    if (!SetICMProfileW(hdc, (wchar_t*)filepath.utf16()))
    {
        debugError();
        qDebug() << "??Windows SetICMProfileW failed" << hdc << filepath;
        DeleteDC(hdc);
        return false;
    }


    DWORD sz = 4096;
    wchar_t readFilepath[sz];
    //Get profile for DC
    if (!GetICMProfileW(hdc, &sz, readFilepath))
    {
        debugError();
        qDebug() << "??Windows GetICMProfileW failed" << hdc;
        DeleteDC(hdc);
        return "";
    }
    
    qDebug() << "read profile for same HDC - " << QString::fromUtf16((ushort*)readFilepath);
       

    //restore Gamma ramp
    if (!SetDeviceGammaRamp(hdc, gammaRamp))
        qDebug() << "??Windows SetDeviceGammaRamp failed";
    
    DeleteDC(hdc);
      
    qDebug() << "ApplyProfile done successfully.";
    defaultProfiles_[osid] = filepath;
    
    qDebug() << "after ApplyProfile -> current=" << currentDisplayProfile(osid);
    
    return true;
}

#define LCS_sRGB                    0x73524742  /* 'sRGB' */
#define LCS_WINDOWS_COLOR_SPACE     0x57696e20  /* 'Win ' */

bool ApplySystemProfile(QString osid, QString osDevName)
{
     qDebug() << "Apply System Profile";
    
    DWORD sz = 4096;
    wchar_t profilepath[sz];
    
    if (!GetStandardColorSpaceProfileW(nullptr, LCS_WINDOWS_COLOR_SPACE, profilepath, &sz))
    {
        debugError();
        qDebug() << "??Windows GetStandardColorSpaceProfileW failed" << osid;
        return false;
    }    
    
    ApplyProfile(QString::fromUtf16((ushort*)profilepath), osid, osDevName);
    
    return true;
}

}
#endif

#ifdef Q_OS_MAC

#include <ApplicationServices/ApplicationServices.h>
#include <sstream>

#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QProcess>
#include "SleepThread.h"
#include <QCoreApplication>

UInt16 myreverse16(UInt16 v)
{
	return ((v & 0x00ff) *256) + v/256;
}

UInt32 myreverse32(UInt32 v)
{
	return ((v & 0x000000ff) * 16777216) + (v & 0x0000ff00) *256 + (v & 0x00ff0000) /256 + v/16777216;
}

namespace OSProfile
{

QString profilepath()
{
    return "/Library/ColorSync/Profiles";
}

QString currentDisplayProfile(QString osid)
{
    CFUUIDRef displayUid = CGDisplayCreateUUIDFromDisplayID(osid.toULong());

    if (!displayUid)
    {
        qDebug() << "currentDisplayProfile can't get uuid by osid" << osid;
        return "";
    }

    CFDictionaryRef profileInfo = ColorSyncDeviceCopyDeviceInfo(kColorSyncDisplayDeviceClass, displayUid);

    if (!profileInfo)
    {
        qDebug() << "currentDisplayProfile can't copy deviceinfo";
        CFRelease(displayUid);
        return "";
    }


    CFDictionaryRef factoryProfiles = (CFDictionaryRef)CFDictionaryGetValue(profileInfo, kColorSyncFactoryProfiles);

    if (!factoryProfiles)
    {
        qDebug() << "currentDisplayProfile can't fetch factory profiles dictionary";
        CFRelease(profileInfo);
        CFRelease(displayUid);
        return "";
    }

    CFStringRef defaultProfileId = (CFStringRef)CFDictionaryGetValue(factoryProfiles, kColorSyncDeviceDefaultProfileID);
    if (!defaultProfileId)
    {
        qDebug() << "currentDisplayProfile can't fetch default profile id";
        CFRelease(profileInfo);
        CFRelease(displayUid);
        return "";
    }

    CFURLRef profileURL = nullptr;
    CFDictionaryRef customProfiles = (CFDictionaryRef)CFDictionaryGetValue(profileInfo, kColorSyncCustomProfiles);
    if (customProfiles)
        profileURL = (CFURLRef)CFDictionaryGetValue(customProfiles, defaultProfileId);

    if (!profileURL)
    {
        qDebug() << "currentDisplayProfile profile is not custom";

        CFDictionaryRef factoryProfile = (CFDictionaryRef)CFDictionaryGetValue(factoryProfiles, defaultProfileId);
        if (factoryProfile)
            profileURL =(CFURLRef)CFDictionaryGetValue(factoryProfile, kColorSyncDeviceProfileID);
    }

    if (!profileURL)
    {
        qDebug() << "currentDisplayProfile profile also not a factory";
        CFRelease(profileInfo);
        CFRelease(displayUid);
        return "";
    }

    char path[PATH_MAX] = {0,};
    CFURLGetFileSystemRepresentation(profileURL, true, (UInt8*)path, PATH_MAX);

    QString res = QString::fromLatin1(path);
    qDebug() << "currentDisplayProfile fetched path" << res;

    CFRelease(profileInfo);
    CFRelease(displayUid);

    return res;
}

bool ApplyProfile(QString filepath, QString osid, QString osDevName)
{
    QProcess p;
    QString path = qApp->applicationDirPath();
    if (!path.endsWith("/"))
        path += "/";
    p.start(path + "profileapply", QStringList() << osid << filepath);
    p.waitForFinished(5000);
    qDebug() << p.readAllStandardOutput();
    qDebug() << p.readAllStandardError();
    qDebug() << p.state();
    if (p.state()==QProcess::Running)
    {
        qDebug() << "profileapply freezed - kill";
        p.kill();
        return false;
    }

    qDebug() << "ApplyProfile " << osid << filepath << "=" << p.exitCode();
    return p.exitCode()==0;
}

bool ApplySystemProfile(QString osid, QString osDevName)
{
    QString resourcesPath = QCoreApplication::applicationDirPath() + "/../Resources";
    QString genericRgbProfilePath = resourcesPath + "/" + "GenericRGB.icc";
    qDebug() << "ApplySystemProfile profile path:" << genericRgbProfilePath;

    return ApplyProfile(genericRgbProfilePath, osid, osDevName);
}

//bool ApplySystemProfile(QString osid, QString osDevName)
//{
//    ColorSyncProfileRef rgbprofile = ColorSyncProfileCreateWithName(kColorSyncGenericRGBProfile);
//    CFErrorRef error;
//    CFURLRef profileURL = ColorSyncProfileGetURL(rgbprofile, &error);

//    if (!profileURL)
//    {
//        qDebug() << "ApplySystemProfile can't get url for generic rgb";

//        CFRelease(rgbprofile);
//        return false;
//    }

//    char path[PATH_MAX] = {0,};
//    CFURLGetFileSystemRepresentation(profileURL, true, (UInt8*)path, PATH_MAX);
//    CFRelease(rgbprofile);

//    QString qpath = QString::fromLatin1(path);

//    qDebug() << "ApplySystemProfile path - " << qpath;

//    return ApplyProfile(qpath, osid, osDevName);
//}

}

#endif
