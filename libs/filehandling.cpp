#include "filehandling.h"

#include <QFile>
#include <QDebug>
#include <QSettings>

#ifdef Q_OS_WIN
#include "Aclapi.h"
#include "Sddl.h"
#endif

bool OSUtils::setFullPermissions(QString path, bool folder/*=false*/)
{

#ifdef Q_OS_WIN
    PSID pEveryoneSID = nullptr;
    PACL pACL = nullptr;
    EXPLICIT_ACCESS ea[1];
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;

    // Create a well-known SID for the Everyone group.
    AllocateAndInitializeSid(&SIDAuthWorld, 1,
                     SECURITY_WORLD_RID,
                     0, 0, 0, 0, 0, 0, 0,
                     &pEveryoneSID);

    // Initialize an EXPLICIT_ACCESS structure for an ACE.
    // The ACE will allow Everyone read access to the key.
    ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions = 0xFFFFFFFF;
    ea[0].grfAccessMode = GRANT_ACCESS;
    ea[0].grfInheritance= OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE ;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

    // Create a new ACL that contains the new ACEs.
    SetEntriesInAcl(1, ea, nullptr, &pACL);

    // Initialize a security descriptor.
    PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
                                SECURITY_DESCRIPTOR_MIN_LENGTH);

    InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION);

    // Add the ACL to the security descriptor.
    SetSecurityDescriptorDacl(pSD,
            TRUE,     // bDaclPresent flag
            pACL,
            FALSE);   // not a default DACL

    //Change the security attributes
    SetFileSecurity(const_cast<wchar_t*>(path.toStdWString().c_str()), DACL_SECURITY_INFORMATION, pSD);

    if (pEveryoneSID)
        FreeSid(pEveryoneSID);
    if (pACL)
        LocalFree(pACL);
    if (pSD)
        LocalFree(pSD);

#endif

    QFile::Permissions p = QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther | QFile::ReadOwner |
                           QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner;

    if (folder)
        p |= QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner;
    // qDebug() << "bool setFullPermissions:" << QFile::exists(path) << QFile(path).permissions();
    // qDebug() << p << path;

    return QFile::setPermissions(path, p);
}

QString OSUtils::getAssociatedApplication(QString extension)
{
#ifdef Q_OS_WIN
    qDebug() << "getAssociatedApplication" << extension;

    QSettings root("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    QString app = root.value("." + extension + "/Default").toString();
    qDebug() << "app" << app;
    if (app.isEmpty())
        return "";

    QString command = root.value(app + "/shell/open/command/Default").toString();
    qDebug() << "command" << command;
    return command;

#else
    return "";
#endif
}
