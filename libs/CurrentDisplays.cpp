#include "CurrentDisplays.h"
#include <QDebug>

DisplayEnumerator CurrentDisplays::displays_;
std::vector<int> CurrentDisplays::ids_;

static bool isUsed_ = false;

void CurrentDisplays::Refresh()
{ 
    //if (displays_.Count()>0)
     //   return;
    
    if (isUsed_ == true)
        return;

    displays_.EnumerateDisplays();
    ids_.resize(displays_.Count(), -1);
    isUsed_ = true;
}

std::map<int, QString> CurrentDisplays::currentUserSerials()
{
    std::map<int, QString> res;
    for (unsigned i=0;i<ids_.size() && i<displays_.Count();++i)
    {
        auto& di = displays_[i];
        if (!di.serialFromUser.isEmpty())
            res[ids_[i]] = di.serialFromUser;
    }

    return res;
}

void CurrentDisplays::restoreUserSerials(std::map<int, QString> serials)
{
    for (unsigned i=0;i<displays_.Count();++i)
    {
        auto& di = displays_[i];
        if (serials.find(ids_[i])!=serials.end())
            di.serialFromUser = serials[ids_[i]];
    }
}

void CurrentDisplays::NonCachedRefresh()
{
    auto serials = currentUserSerials();
    displays_.ClearInformation();
    displays_.EnumerateDisplays();
    ids_.resize(displays_.Count(), -1);
    isUsed_ = true;
    restoreUserSerials(serials);
}

void CurrentDisplays::RefreshDisplaysGeometry()
{
    qDebug() << "CurrentDisplays::RefreshDisplaysGeometry() begin";
    DisplayEnumerator disps;
    disps.EnumerateDisplays();
    
    for (uint i = 0; i < displays_.Count(); i++)
        for (uint j = 0; j < disps.Count(); j++)
            if (displays_[i].detectSerial() == disps[j].detectSerial() && displays_[i].OSId == disps[j].OSId)
            {
                displays_[i].setTop(disps[j].top());
                displays_[i].setLeft(disps[j].left());
                displays_[i].setHeight(disps[j].height());
                displays_[i].setWidth(disps[j].width());
                break;
            }
    qDebug() << "CurrentDisplays::RefreshDisplaysGeometry() end";
}

unsigned int CurrentDisplays::DisplayCount()
{
    Refresh();
    return displays_.Count();
}

const DisplayInfo& CurrentDisplays::Display(unsigned int index)
{
    Refresh();
    return displays_[index];
}

const DisplayInfo& CurrentDisplays::DisplayById(int dislayid)
{
    Refresh();

    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==dislayid)
            return displays_[i];

    if (displays_.Count()<1)
    {
        static DisplayInfo di;
        return di;
    }
    
    return displays_[0];
}

void CurrentDisplays::SetDisplayId(unsigned int index, int id)
{
    if (index>=ids_.size())
        return;
    ids_[index] = id;
}

int CurrentDisplays::DisplayId(unsigned int index)
{
    if (index>=ids_.size())
        return -1;
    return ids_[index];
}

bool CurrentDisplays::IsDisplayConnected(int dislayid)
{
    bool result = false;

    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==dislayid)
            result = true;

    return result;
}

bool CurrentDisplays::IsDisplaysChanged()
{
#ifdef EXTRA_DEBUG_OUTPUT_AND_COPY_DB
    qDebug() << "CurrentDisplays::IsDisplaysChanged()";
#endif
    DisplayEnumerator newdisp;
    newdisp.EnumerateDisplays();
    
    if (displays_.Count()!=newdisp.Count())
    {
        displays_ = newdisp;
        return true;
    }
    
    for (unsigned i=0;i<displays_.Count();++i)
    {
        //qDebug()<<"disp 1 - "<<displays_[i].serial<<" disp 2 - "<<newdisp[i].serial;
        
        if (displays_[i].detectSerial() != newdisp[i].detectSerial())
        {
            displays_ = newdisp;
            return true;
        }
    }
    
    return false;
}

bool CurrentDisplays::RemoveDisplayByIndex(unsigned int index)
{
    return displays_.RemoveDisplay(index);
}

QString CurrentDisplays::GetVideoCardName(int dislayid)
{
    return displays_.GetVideocardNameByKey(DisplayById(dislayid).VideoAdapterKey);
}

QString CurrentDisplays::GetVideoCardKey(int dislayid)
{
    return DisplayById(dislayid).VideoAdapterKey;
}

void CurrentDisplays::setInternalSensor(int displayid, bool hasIS)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].hasInternalSensor_ = (hasIS ? DisplayInfo::ISExists : DisplayInfo::ISNotExists);
}

void CurrentDisplays::setAmbientSensor(int displayid, bool hasAS)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].hasAmbientSensor_ = (hasAS ? DisplayInfo::ISExists : DisplayInfo::ISNotExists);
}

void CurrentDisplays::clearInternalSensorForAll()
{
    for (unsigned i=0;i<displays_.Count();i++)
        displays_[i].hasInternalSensor_ = DisplayInfo::ISUnknown;
}

void CurrentDisplays::setSensorCorrelationSupport(int displayid, bool support)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].specialCorrelation_ = support;
}

void CurrentDisplays::setSpecialRegistration(int displayid, bool specialRegistration)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].specialRegistration_ = specialRegistration;
}

void CurrentDisplays::setDisplaySerial(int displayid, QString serial)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].serial() = serial;
}

void CurrentDisplays::setDisplayName(int displayid, QString name)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].name() = name;
}

void CurrentDisplays::setDisplayCalibrationPathInPercents(int displayid, int patch)
{
    for (unsigned i=0;i<displays_.Count();i++)
        if (ids_[i]==displayid)
            displays_[i].calPatchSize_ = patch;
}

void CurrentDisplays::AddDisplay(int displayId, const DisplayInfo &dinfo)
{
    ids_.push_back(displayId);
    displays_.AddDisplay(dinfo);
}

void CurrentDisplays::Clear()
{
    displays_.ClearInformation();
    ids_.clear();
}

int CurrentDisplays::displayIdByCoordinates(int x, int y)
{
    for (unsigned i=0;i<displays_.Count();i++)
    {
        const DisplayInfo& di = displays_[i];
        if (x >= di.left() && x <= di.left() + di.width()
            && y >= di.top() && y <= di.top() + di.height())
            return ids_[i];
    }

    return -1;
}
