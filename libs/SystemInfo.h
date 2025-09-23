/* 
 * File:   SystemInfo.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 2 липня 2013, 11:26
 */

#ifndef SYSTEMINFO_H
#define	SYSTEMINFO_H

#include <QString>
#include <map>
#include <vector>

namespace OSUtils
{
    enum ProcessStatus {NotFoundStatus, FoundStatus, ErrorStatus};
    
    QString getComputerName();
    std::vector<std::pair<int, QString>> processList();
    std::vector<std::pair<int, QString>> processListCurrentUser();
    int pidByProcessName(QString name);
    ProcessStatus isAnyOfProcessesRuns(QStringList names, bool currentUserOnly = false);
    bool activateWindow(QString windowTitle);
    uint64_t currentThreadId();
    void killAllProcessesByName(QString name, bool currentUserOnly = true);
}

#endif	/* SYSTEMINFO_H */

