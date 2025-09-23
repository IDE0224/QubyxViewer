#ifndef DISPLAYS_H
#define	DISPLAYS_H

#include <QByteArray>
#include <QString>
#include <QRect>
#include <QSize>
#include <vector>
#include <map>

#ifdef Q_OS_MAC
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace OSUtils
{

    void turnoffdisplay();
    void turnondisplay();
    void blocksleep();

    /*
     * copy edids as .toAnsii()
     *
     */
    void takedisplays(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
                      std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey,  std::vector<QString> &videoadaptername);
    
    bool screensaverIsRunning();
    
    /**
     * Check if screensaver is protected by password.
     * Not works with Windows Server 2003 and Windows XP/2000
     */
    bool screensaverIsProtected();
    void stopScreensaver();
    
    bool extractMacVideoCardFromDictionary(QString dict, unsigned char* edid, unsigned edidSize, QString& key, QString& name);

#ifdef Q_OS_MAC
    QString MacDictionaryRead(); 
    void MacGetCardInfo(QByteArray &edid, QString &cardKey, QString &cardName);
    io_service_t IOServicePortFromCGDisplayID(CGDirectDisplayID displayID);
#endif
}

#endif	/* DISPLAYS_H */

