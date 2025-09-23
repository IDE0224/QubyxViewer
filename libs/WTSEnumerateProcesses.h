/* 
 * File:   WTSEnumerateProcesses.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 3 жовтня 2013, 12:24
 */

#ifndef WTSENUMERATEPROCESSES_H
#define	WTSENUMERATEPROCESSES_H

#include <QGlobal.h>

#ifdef Q_OS_WIN

extern "C"
{

#include <Wtsapi32.h>

#if (__GNUC__ ==4 && __GNUC_MINOR__ <= 6)
typedef struct _WTS_PROCESS_INFOW {
    DWORD SessionId;     // session id
    DWORD ProcessId;     // process id
    LPWSTR pProcessName; // name of process
    PSID pUserSid;       // user's SID
} WTS_PROCESS_INFOW, * PWTS_PROCESS_INFOW;

typedef struct _WTS_PROCESS_INFOA {
    DWORD SessionId;     // session id
    DWORD ProcessId;     // process id
    LPSTR pProcessName;  // name of process
    PSID pUserSid;       // user's SID
} WTS_PROCESS_INFOA, * PWTS_PROCESS_INFOA;
#endif

#ifdef UNICODE
#define WTS_PROCESS_INFO  WTS_PROCESS_INFOW
#define PWTS_PROCESS_INFO PWTS_PROCESS_INFOW
#else
#define WTS_PROCESS_INFO  WTS_PROCESS_INFOA
#define PWTS_PROCESS_INFO PWTS_PROCESS_INFOA
#endif

/*------------------------------------------------*/

//#endif

typedef BOOL (__stdcall *WTSEnumerateProcessesW_my) (IN HANDLE hServer, IN DWORD Reserved, IN DWORD Version, OUT PWTS_PROCESS_INFOW * ppProcessInfo, OUT DWORD * pCount);
typedef BOOL (__stdcall *WTSEnumerateProcessesA_my) (IN HANDLE hServer, IN DWORD Reserved, IN DWORD Version, OUT PWTS_PROCESS_INFOA * ppProcessInfo, OUT DWORD * pCount);

typedef BOOL (__stdcall *WTSFreeMemory_my) (IN PVOID pMemory);



}

#endif /* Q_OS_WIN */

#endif	/* WTSENUMERATEPROCESSES_H */

