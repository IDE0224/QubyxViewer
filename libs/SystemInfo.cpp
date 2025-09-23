/* 
 * File:   SystemInfo.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 2 липня 2013, 11:26
 */

#include "SystemInfo.h"
#include "inline.h"

#include <QProcess>
#include <QDebug>
#include <list>
#include <map>
#include <algorithm>

#ifdef Q_OS_MAC
#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/sysctl.h>
#include <pthread.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#include "WTSEnumerateProcesses.h"
#endif

#ifdef Q_OS_LINUX
#include <pthread.h>
#endif

namespace OSUtils
{
    QString getComputerName()
    {
        QString cName = "";
#ifdef Q_OS_WIN
        std::list<QString> l = qbx::toStdList(QProcess::systemEnvironment());
        QString cnParam = "COMPUTERNAME";
        for (std::list<QString>::iterator it = l.begin(); it != l.end(); ++it){
            QString curLine = *it;
            if (curLine.indexOf(cnParam) != -1) {
                cName.append(curLine.right(curLine.length() - curLine.indexOf('=') - 1));
                break;
            }
        }
#endif
    
#ifdef Q_OS_MAC
        char name[256];
        if (!gethostname(name, 255) ){
            cName = QString::fromLatin1(name);
            if (cName.contains("."))
                cName = cName.left(cName.indexOf("."));
        }
#endif
    
        return cName;
    }
    
    int pidByProcessName(QString name)
    {
        std::vector<std::pair<int, QString>> m = processList();
        
        auto it = std::find_if (m.begin(), m.end(), 
                [name](const std::pair<int, QString>& v)->bool 
                        {return v.second==name;});
        if (it==m.end())
            return -1;
        
        return it->first;
    }
    
    OSUtils::ProcessStatus isAnyOfProcessesRuns(QStringList names, bool currentUserOnly/* = false*/)
    {
        std::vector<std::pair<int, QString>> list;
        QStringList foundAppsFromList;
        names.clear();

        if (currentUserOnly)
            list = processListCurrentUser();
        else
            list = processList();
        
        if  (list.empty()) {
            qDebug() << "ErrorStatus";
            return ErrorStatus;
        }

        OSUtils::ProcessStatus status = NotFoundStatus;
        for (auto i : list){
            for (auto j : names)
                if (QString::compare(i.second, j), Qt::CaseInsensitive){
                    foundAppsFromList += j;
                    status = FoundStatus;
                }
        }
        qDebug() << "OSUtils::ProcessStatus isAnyOfProcessesRuns:" << foundAppsFromList;

        return status;
    }
    
#ifdef Q_OS_WIN    
    std::vector<std::pair<int, QString>> processList()
    {
        std::vector<std::pair<int, QString>> res;
        
        HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);        
        if (h==INVALID_HANDLE_VALUE){
            qDebug() << "can't list processes";
            return res;
        }
        
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof( PROCESSENTRY32 );
        if ( !Process32First( h, &pe32 ) ){
            qDebug() << "Process32First error";
            CloseHandle(h);          
            return res;
        }
        do {
            res.push_back(std::make_pair(pe32.th32ProcessID, QString::fromUtf16((ushort*)pe32.szExeFile)));
//            qDebug() << "!Process:" << pe32.th32ProcessID << QString::fromUtf16((ushort*)pe32.szExeFile);
            

        } while( Process32Next( h, &pe32 ) );

        CloseHandle(h);
        return res;
    }
    
    std::vector<std::pair<int, QString>> processListCurrentUser()
    {
        qDebug() << "processListCurrentUser() 1";
        std::vector<std::pair<int, QString>> procList;
        
        HANDLE procToken = nullptr;
        if (!OpenProcessToken(GetCurrentProcess(),  TOKEN_QUERY, &procToken) ){
            qDebug() << "OpenProcessToken failed" << GetLastError();
            return procList;
        }
        
        qDebug() << "processListCurrentUser() 2";
        
        DWORD currUserSize = 0;
        GetTokenInformation(procToken, TokenUser, nullptr, 0, &currUserSize);
        qDebug() << "currUserSize = " << currUserSize;
        if (!currUserSize){
            qDebug() << "if (!currUserSize)";
            return procList;
        }
        
        PTOKEN_USER currUser = (PTOKEN_USER) new BYTE[currUserSize];
        memset(currUser, 0, currUserSize);
        DWORD ReturnLength;
        if (!GetTokenInformation(procToken, TokenUser, currUser, /*sizeof(TOKEN_USER)*/currUserSize, &ReturnLength) ){
            qDebug() << "GetTokenInformation failed" << ReturnLength << GetLastError();
            return procList;
        }
        
        qDebug() << "processListCurrentUser() 3";
        
        HINSTANCE dll = LoadLibraryA("Wtsapi32.dll");
        if (!dll){
            qDebug() << "dll not loaded";
            return procList;
        }
        
        qDebug() << "processListCurrentUser() 4";
        
        WTSEnumerateProcessesA_my WTSEnumerateProcessFunc = WTSEnumerateProcessesA_my(GetProcAddress(dll, "WTSEnumerateProcessesA"));
        WTSFreeMemory_my WTSFreeMemoryFunc = WTSFreeMemory_my(GetProcAddress(dll, "WTSFreeMemory"));
        
        qDebug() << "processListCurrentUser() 5";
        
        if (!WTSEnumerateProcessFunc || !WTSFreeMemoryFunc){
            qDebug() << "functions not loaded" << WTSEnumerateProcessFunc << WTSFreeMemoryFunc;
            delete [] currUser;
            FreeLibrary(dll);
            return procList;
        }
        
        qDebug() << "processListCurrentUser() 6";
               
        WTS_PROCESS_INFOA *procs = nullptr;
        DWORD procCount = 0;
        if (!WTSEnumerateProcessFunc(WTS_CURRENT_SERVER_HANDLE, 0, 1, &procs, &procCount)){
            qDebug() << "WTSEnumerateProcesses failed";
            delete [] currUser;
            FreeLibrary(dll);
            return procList;
        }
        
        qDebug() << "procCount" << procCount;
        
        for (uint i = 0; i < procCount; i++){
            if (procs[i].pUserSid && currUser->User.Sid && EqualSid(procs[i].pUserSid, currUser->User.Sid))
                procList.push_back(std::make_pair(int(procs[i].ProcessId), QString(procs[i].pProcessName)));
        }
        
        qDebug() << "processListCurrentUser() 7";
        
        delete [] currUser;
        WTSFreeMemoryFunc(procs);
        FreeLibrary(dll);
        
        qDebug() << "processListCurrentUser() 8";
        
        return procList;
    }

    void killAllProcessesByName(QString name, bool currentUserOnly/* = true*/)
    {
        name = name.toLower();
        std::vector<std::pair<int, QString>> lst = currentUserOnly ? processListCurrentUser() : processList();
        for(auto p : lst) {
            if(p.second.toLower() == name) {
                qDebug() << "going to kill" << p.first << p.second;
                std::string cmd = "taskkill /pid " + std::to_string(p.first);
                int retValue = std::system(cmd.c_str());
                qDebug() << "retValue =" << retValue;
            }
            else
                qDebug() << p.first << p.second << "is a process which shouldn't be killed";
        }
    }
#endif

#ifdef Q_OS_LINUX
    std::vector<std::pair<int, QString>> processList()
    {
        return std::vector<std::pair<int, QString>>();
    }

    std::vector<std::pair<int, QString>> processListCurrentUser()
    {
        return std::vector<std::pair<int, QString>>();
    }
#endif

#ifdef Q_OS_MAC
    int GetBSDProcessList( kinfo_proc **procList, size_t *procCount )
    {
        int                 err;
        kinfo_proc *        result;
        bool                done;
        static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
        // Declaring name as const requires us to cast it when passing it to
        // sysctl because the prototype doesn't include the const modifier.
        size_t              length;

        // We start by calling sysctl with result == nullptr and length == 0.
        // That will succeed, and set length to the appropriate length.
        // We then allocate a buffer of that size and call sysctl again
        // with that buffer.  If that succeeds, we're done.  If that fails
        // with ENOMEM, we have to throw away our buffer and loop.  Note
        // that the loop causes use to call sysctl with nullptr again; this
        // is necessary because the ENOMEM failure case sets length to
        // the amount of data returned, not the amount of data that
        // could have been returned.
        result = nullptr;
        done = false;
        *procCount = 0;

        do {
            // Call sysctl with a nullptr buffer.
            length = 0;
            err = sysctl( (int *) name, ( sizeof( name ) / sizeof( *name ) ) - 1, nullptr, &length, nullptr, 0 );
            if (err == -1)
                err = errno;

            // Allocate an appropriately sized buffer based on the results
            // from the previous call.
            if ( err == 0 ){
                result = (kinfo_proc*)malloc( length );
                if ( result == nullptr )
                    err = ENOMEM;
            }

            // Call sysctl again with the new buffer.  If we get an ENOMEM
            // error, toss away our buffer and start again.
            if ( err == 0 ){
                err = sysctl( (int *) name, ( sizeof( name ) / sizeof( *name ) ) - 1, result, &length, nullptr, 0 );
                if ( err == -1 )
                    err = errno;
                if (err == 0)
                    done = true;
                else if ( err == ENOMEM ){
                    free( result );
                    result = nullptr;
                    err = 0;
                }
            }
        } while ( err == 0 && !done );

        // Clean up and establish post conditions.
        if ( err != 0 && result != nullptr ){
            free( result );
            result = nullptr;
        }

        *procList = result;
        if ( err == 0 )
            *procCount = length / sizeof( kinfo_proc );

        return err;
    }
    
    std::vector<std::pair<int, QString>> processList()
    {
        std::vector<std::pair<int, QString>> res;
 
        kinfo_proc* pList = nullptr;
        size_t processCount = 0;

        if ( GetBSDProcessList( &pList, &processCount ) )
            processCount = 0;

        for ( size_t processIndex = 0; processIndex < processCount; processIndex++ )
            res.push_back(std::make_pair(pList[processIndex].kp_proc.p_pid, QString::fromLatin1(pList[processIndex].kp_proc.p_comm)));

        if ( pList != nullptr )
            free( pList );

        return res;
    }
    
    std::vector<std::pair<int, QString>> processListCurrentUser()
    {
        return processList();
    }

    // TODO!!!
    // void killAllProcessesByName(QString name, bool currentUserOnly/* = true*/)    {    }

#endif    
    
    bool activateWindow(QString windowTitle)
    {
        qDebug() << "activateWindow" << windowTitle;
#ifdef Q_OS_WIN
        HWND handle = FindWindowA(nullptr, windowTitle.toStdString().c_str());
        qDebug() << "handle" << handle;
        
        if (handle){
            ShowWindow(handle, SW_RESTORE);
            InvalidateRect (handle, nullptr, TRUE);
            UpdateWindow (handle);
            SetForegroundWindow(handle);
            return true;
        }
        return false;
#endif
        
#ifdef Q_OS_MAC
        return false;
#endif

#ifdef Q_OS_LINUX
        return false;
#endif
    }

    uint64_t currentThreadId()
    {
    #ifdef Q_OS_WIN
        return GetCurrentThreadId();
    #elif defined(Q_OS_MAC)
        uint64_t tid;
        pthread_threadid_np(nullptr, &tid);

        return tid;
    #elif defined(Q_OS_LINUX)
        return pthread_self();
    #endif
    }
}
