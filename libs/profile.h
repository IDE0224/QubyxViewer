#ifndef PROFILE_H
#define	PROFILE_H

#include <vector>
#include <QString>

namespace OSProfile
{
    QString profilepath();
    QString currentDisplayProfile(QString osid);

    bool ApplySystemProfile(QString osid, QString osDevName);
    bool ApplyProfile(QString filepath, QString osid, QString osDevName);
}

#endif	/* PROFILE_H */

