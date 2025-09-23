#ifndef CURRENTDISPLAYS_H
#define	CURRENTDISPLAYS_H

#include "DisplayEnumerator.h"
#include <vector>
#include <map>
#include <QRectF>
#include "DisplayInfo.h"

class CurrentDisplays
{
    CurrentDisplays()
    {
    }

    static DisplayEnumerator displays_;
    static std::vector<int> ids_;
    static void Refresh();

    static std::map<int, QString> currentUserSerials();
    static void restoreUserSerials(std::map<int, QString> serials);
   
public:  
    static unsigned int DisplayCount();
    static const DisplayInfo& Display(unsigned int index);
    static const DisplayInfo& DisplayById(int dislayid);
    static void SetDisplayId(unsigned int index, int id);
    static int DisplayId(unsigned int index);
    static bool IsDisplayConnected(int dislayid);
    static void NonCachedRefresh();
    static void RefreshDisplaysGeometry();
    static bool IsDisplaysChanged();
    static bool RemoveDisplayByIndex(unsigned int index);
    
    static QString GetVideoCardName(int dislayid);
    static QString GetVideoCardKey(int dislayid);
    
    static void setInternalSensor(int displayid, bool hasIS);
    static void setAmbientSensor(int displayid, bool hasAS);
    static void clearInternalSensorForAll();

    static void setSensorCorrelationSupport(int displayid, bool support);
    
    static void setSpecialRegistration(int displayid, bool specialRegistration);
    
    static void setDisplaySerial(int displayid, QString serial);
    static void setDisplayName(int displayid, QString name);
    static void setDisplayCalibrationPathInPercents(int displayid, int patch);
    
    static void AddDisplay(int displayId, const DisplayInfo &dinfo);
    static void Clear();

    /**
     * @brief displayIdByCoordinates
     * @param x
     * @param y
     * @return return displayId, -1 in case of error
     */
    static int displayIdByCoordinates(int x, int y);
    friend class UserScheduling;
};

#endif	/* CURRENTDISPLAYS_H */
