/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "viewerwindowinfo.h"
#include "libs/CurrentDisplays.h"
#include "libs/profile.h"

ViewerWindowInfo::ViewerWindowInfo(int dispId, bool isVisible, bool isMain) : QObject(nullptr)
{
    setDisplayId(dispId);

    const DisplayInfo& disp = CurrentDisplays::DisplayById(dispId);
    
    setGeometry(QRect(disp.left(), disp.top(), disp.width(), disp.height()));
    setName(disp.manufacture() + " " + disp.name() + " " + disp.serial());
    setLutEnabled(false);
    setDisplayProfile(OSProfile::currentDisplayProfile(disp.OSId));

    setIsMainDisplay(isMain);
    setVisible(isVisible);
}

ViewerWindowInfo::~ViewerWindowInfo()
{

}
