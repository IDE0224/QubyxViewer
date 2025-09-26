#include <vector>
#include <QString>
#include <QDebug>
#include <QtGui/QGuiApplication>
#include <QScreen>

#include "DisplayEnumerator.h"

#include "displays.h"
#include "DisplayInfo.h"
#include "EDIDParser.h"
#include "QubyxDebug.h"
#include <cstring>
#include <map>

#include "LgMedicalUartConnectionInfo.h"

#ifdef Q_OS_WIN
#include "Graphics1DisplayStaticWrapper.h"
#endif

#ifdef Q_OS_MAC
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include "MacTools.h"
#endif

DisplayEnumerator::DisplayEnumerator()
{
}

DisplayEnumerator::~DisplayEnumerator()
{
}

void DisplayEnumerator::EnumerateDisplays()
{
    displays_.clear();
    //std::vector<int> top, left, width, height;
    std::vector<QRect> geometry;
    std::vector<QSize> resolution;
    std::vector<QByteArray> edids;
    std::vector<QString> osIds, osDevs;
    std::vector<QString> videoadapterkey;
    std::vector<QString> videoadaptername;
    
    OSUtils::takedisplays(geometry, resolution, edids, osIds, osDevs, videoadapterkey, videoadaptername);

    if (geometry.size()!=resolution.size() || geometry.size()!=edids.size() || geometry.size()!=osIds.size())
        return;

#ifdef Q_OS_WIN
    Graphics1DisplayStaticWrapper graphics1Wrapper;
    if (graphics1Wrapper.load() && graphics1Wrapper.enumerateDisplays())
        graphics1Wrapper.updateDisplayInfo(geometry, resolution, edids, osIds, osDevs, videoadapterkey, videoadaptername);
#endif

    FillInfo(geometry, resolution, edids, osIds, osDevs, videoadapterkey, videoadaptername);

    joinWideDisplays();
    joinEizoDisplays();

// FIXME test pattern
//#ifdef Q_OS_WIN
    for (auto screen = displays_.begin(); screen != displays_.end(); ++screen){
        if (isWideLeftPart(screen.operator*()) || isWideRightPart(screen.operator*()) || isEizoPart(screen.operator*()))
            continue;

        for (auto qtScreen : qGuiApp->screens())
            if (qtScreen){
                auto geo = qtScreen->geometry();
                if (geo.topLeft() == QPoint(screen->left(), screen->top())){
                    screen->setWidth(geo.width());
                    screen->setHeight(geo.height());
                    continue;
                }
            }
    }
//#endif
}

unsigned int DisplayEnumerator::Count()
{
    return displays_.size();
}

const DisplayInfo& DisplayEnumerator::GetDisplay(unsigned int pos)
{
    if (pos<displays_.size())
        return displays_[pos];
    return empty_;
}

void DisplayEnumerator::AddDisplay(const DisplayInfo& info)
{
    FillInfo(info);
}

DisplayInfo& DisplayEnumerator::operator[](unsigned int pos)
{
    //return GetDisplay(pos);
    if (pos<displays_.size())
        return displays_[pos];
    return empty_;
}

void DisplayEnumerator::ClearInformation()
{
    displays_.clear();
}

bool DisplayEnumerator::RemoveDisplay(unsigned int pos)
{
    qDebug()<<"Displays - "<<displays_.size();
    qDebug()<<"Pos - "<<pos;
    if (pos<displays_.size())
    {
        std::vector<DisplayInfo>::iterator it = displays_.begin();
        
        unsigned count = 0;
        while(count < pos && it!=displays_.end())
        {
            ++it;
            ++count;
        }
        
        qDebug()<<"before remove - "<<displays_.size();
        if (it!=displays_.end())
                displays_.erase(it);
        qDebug()<<"after remove - "<<displays_.size();
        return true;
    }
    else
        return false;
}

QString DisplayEnumerator::GetVideocardNameByKey(const QString &key)
{
    for (std::vector<VideoCardInfo>::iterator cardIt = videoadapters_.begin(); cardIt != videoadapters_.end(); ++cardIt)
        if (cardIt->CardKey == key)
            return cardIt->CardName;
    
    return "";
}

void DisplayEnumerator::FillInfo(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
                      std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey,  std::vector<QString> &videoadaptername)
{
    DisplayInfo info;
    VideoCardInfo vinfo;
    
    for (uint i = 0; i < geometry.size(); i++) {
        info.setRect(geometry[i]);
        info.setResolution(resolution[i]);
        info.EDIDsize = uint(std::min(1024, int(edids[i].size())));
        info.OSId = osIds[i];
        info.OSDevName = osDevNames.size()>i ? osDevNames[i] : "";
        
        info.VideoAdapterKey = videoadapterkey[i];
        info.VideoAdapterName = videoadaptername[i];

        vinfo.CardKey  = videoadapterkey[i];
        vinfo.CardName = videoadaptername[i];

        memcpy(info.EDID, edids[i].data(), info.EDIDsize);
        // Debug: EDID information for display enumeration
        // qDebug() << "EDID: " << edids[i].toHex();
        // qDebug() << "EDID size: " << edids[i].size();
        // qDebug() << "Position: " << info.left << info.top << "-" << info.width << info.height;
        // qDebug() << "OS id:" << info.OSId;
        displays_.push_back(info);
        videoadapters_.push_back(vinfo);
    }

#ifdef Q_OS_MAC
    std::map<QString, MacTools::MacDisplayInfo> displaysAdditionalInfo;
#endif
    
    //extract values from edid;
    for (unsigned i = 0; i < displays_.size(); i++)
    {
        if (displays_[i].EDIDsize > 0)
        {
            EDIDParser edid(displays_[i].EDID, displays_[i].EDIDsize, displays_[i].OSId);

            displays_[i].setName       (edid.name());
            displays_[i].setManufacture(edid.manufacturer());
            displays_[i].setSerial     (edid.serial());

            displays_[i].vendor3Symbols = edid.getShortManufacturer();
            displays_[i].name0xFC       = edid.getEDIDString(0xFC);
            displays_[i].name0xFE       = edid.getEDIDString(0xFE);
            displays_[i].productCode[0] = displays_[i].EDID[10];
            displays_[i].productCode[1] = displays_[i].EDID[11];

            std::vector<EDIDParser::Resolution> sizeMm = edid.realSizesMm();
            if (sizeMm.size())
                displays_[i].setSize_mm(sizeMm[0].x, sizeMm[0].y);

            if (displays_[i].serial().isEmpty()){
                displays_[i].setSerial(displays_[i].OSId);
            }
#ifdef Q_OS_MAC
            if (displays_[i].name() == displays_[i].OSId && displays_[i].serial() == displays_[i].OSId){
                displays_[i].setSerial(QString::number(CGDisplaySerialNumber(displays_[i].OSId.toUInt())));
            }
#endif
        }
        else
        {
            auto& currDisp = displays_[i];
#ifdef Q_OS_MAC
            if (displaysAdditionalInfo.empty())
                displaysAdditionalInfo = MacTools::additionalDisplayInfo();

            if (displaysAdditionalInfo.count(currDisp.OSId))
            {
                auto dispInfo = displaysAdditionalInfo[currDisp.OSId];
                currDisp.setName(dispInfo.name);
                currDisp.setManufacture(dispInfo.vendor);
                if (dispInfo.serialNumber.isEmpty())
                {
                    currDisp.setSerial(currDisp.OSId);
                    currDisp.setSerial(QString::number(CGDisplaySerialNumber(currDisp.OSId.toUInt())));
                }
                else
                {
                    currDisp.setSerial(dispInfo.serialNumber);
                }
                currDisp.setSize_mm(static_cast<int>(dispInfo.width_mm), static_cast<int>(dispInfo.height_mm) );
                if (!dispInfo.vendor_short.isEmpty())
                {
                    currDisp.vendor3Symbols = dispInfo.vendor_short;
                    currDisp.setManufacture(EDIDParser::longManufacturer(dispInfo.vendor_short));
                }
            }
#else
            currDisp.setSerial(currDisp.OSId);
#endif
        }

        displays_[i].setDetectSerial(displays_[i].serial());
    }

    // 1 step, find all LG displays, and place it in one vector
    std::map <uint, bool> all_lg_displays;
    for (uint i = 0; i < displays_.size(); ++i)
        if (displays_[i].manufacture() == "LG")
            all_lg_displays[i] = !displays_[i].serial().isEmpty(); // T - serial exist

    // 2 step, find all displays that we can, using UART LG communications
    auto lg_uart_displays = LgMedicalUartConnectionInfo::enumerateUartConnections();

    // 3 step, fill new map <serial number, full edid>
    std::map <QString, QByteArray> uart_serial_list;
    for (auto it : lg_uart_displays){
        auto edid = it.second.edid;
        QString newSerial("");
        for (uint e = 2; e < edid.size(); e++){
            if (edid[e - 2] == char(0x00) && edid[e - 1] == char(0x00) && edid[e] == char(0xFF)){
                for (uint c = 0; c < 13; c++){
                    char nc = edid[e + c + 2];
                    if ((nc >= 48 && nc <= 57) || (nc >= 65 && nc <= 90))
                        newSerial += nc;
                }
                break;
            }
        }
        if (!newSerial.isEmpty())
            uart_serial_list[newSerial] = edid;
    }

    // 4 step, remove from lg_all_list already used serial number
    std::map <QString, QByteArray> uart_unused_serial_list;
    for (auto it = uart_serial_list.begin(); it != uart_serial_list.end(); ++it){
        uint match(UINT_MAX);
        for (auto it_all = all_lg_displays.begin(); it_all != all_lg_displays.end(); ++it_all){
            if (displays_[it_all->first].serial() == it->first){
                match = it_all->first;
                break;
            }
        }
        if (match != UINT_MAX)
            all_lg_displays.erase(match);
        else
            uart_unused_serial_list[it->first] = it->second;
    }

    // 5 step, fill all not serial display with uart serial, in random way
    for (auto i = all_lg_displays.begin(); i != all_lg_displays.end(); ++i){
        if (i->second)
            continue;

        QString new_place_serial("");
        for (auto it = uart_serial_list.begin(); it != uart_serial_list.end(); ++it){
            // find 0xFC from uart edid
            QString display_name("");
            for (uint e = 2; e < it->second.size(); e++){
                if (it->second[e - 2] == char(0x00) && it->second[e - 1] == char(0x00) && it->second[e] == char(0xFF)){
                    for (uint c = 0; c < 13; c++){
                        char nc = it->second[e + c + 2];
                        if ((nc >= 48 && nc <= 57) || (nc >= 65 && nc <= 90))
                            display_name += nc;
                    }
                    break;
                }
            }
            if (displays_[i->first].name() == display_name){
                new_place_serial = it->first;
                displays_[i->first].setSerial(it->first);
                i->second = true;
                break;
            }
        }
        if (!new_place_serial.isEmpty())
            uart_serial_list.erase(new_place_serial);
    }

    uint unfind_counter = 1;
    for (auto i = all_lg_displays.begin(); i != all_lg_displays.end(); ++i){
        if (!i->second){
            displays_[i->first].setSerial("DISPLAY" + QString::number(unfind_counter));
            unfind_counter++;
        }
    }
}

void DisplayEnumerator::FillInfo(const DisplayInfo &info)
{
    VideoCardInfo vinfo;
     
    vinfo.CardKey  = info.VideoAdapterKey;
    vinfo.CardName = info.VideoAdapterName;
     
    displays_.push_back(info);
    videoadapters_.push_back(vinfo);
     
    //extract values from edid;
    for (unsigned i = 0; i < displays_.size(); i++){
        QByteArray barr(reinterpret_cast <char*>(displays_[i].EDID), int(displays_[i].EDIDsize));

        qDebug()<<"displays_[i].EDID - "<<barr.toHex();
        EDIDParser edid(displays_[i].EDID, displays_[i].EDIDsize, displays_[i].OSId);

        displays_[i].setSerial(edid.serial());
        displays_[i].setName(edid.name());
        displays_[i].setManufacture(edid.manufacturer());

        if (displays_[i].serial().isEmpty())
            displays_[i].setSerial(displays_[i].OSId);
        displays_[i].setDetectSerial(displays_[i].serial());
    }
}

void DisplayEnumerator::joinWideDisplays()
{
    qDebug() << "DisplayEnumerator::joinWideDisplays()";

    const int n = int(displays_.size());
    std::vector<bool> removed(uint(n), false);

    for (uint i = 0; int(i) < n; i++)
        if (isWideLeftPart(displays_[i])){
            auto &leftPart = displays_[i];
            const bool isHorizontal = leftPart.width() < leftPart.height();

            qDebug() << "left part" << i << "geometry:" << leftPart.left() << leftPart.top() << leftPart.width() << leftPart.height() << isHorizontal;

            for (uint j = 0; int(j) < n; j++)
                if (i != j && !removed[j] && isWideRightPart(displays_[j]))
                {
                    auto &rightPart = displays_[j];
                    qDebug() << "right part" << j << "geometry:" << rightPart.left() << rightPart.top() << rightPart.width() << rightPart.height();

                    if (leftPart.name() != rightPart.name())
                        continue;

                    const int gap = 20;

                    if (isHorizontal && (abs(leftPart.top() - rightPart.top()) <= gap &&
                                         abs(leftPart.left() + leftPart.width() - rightPart.left()) <= gap) )
                    {
                        leftPart.setWidth(leftPart.width() + rightPart.width());
                        leftPart.setResolutionWidth(leftPart.resolutionWidth() + rightPart.resolutionWidth());
                        removed[j] = true;
                        qDebug() << "join displays horizontally" << i << j;
                        break;
                    }

                    // clock-wise rotation
                    if (!isHorizontal && (abs(leftPart.left() - rightPart.left()) <= gap ||
                                          abs(leftPart.top() + leftPart.height() - rightPart.top()) <= gap) )
                    {
                        leftPart.setHeight(leftPart.height() + rightPart.height());
                        leftPart.setResolutionHeight(leftPart.resolutionHeight() + rightPart.resolutionHeight());
                        removed[j] = true;
                        qDebug() << "join displays clockwise rotation" << i << j;
                        break;
                    }

                    // counter-clockwise rotation
                    if (!isHorizontal && (abs(leftPart.left() - rightPart.left()) <= gap ||
                                          abs(rightPart.top() + rightPart.height() - leftPart.top()) <= gap) )
                    {
                        leftPart.setTop(rightPart.top());
                        leftPart.setHeight(leftPart.height() + rightPart.height());
                        leftPart.setResolutionHeight(leftPart.resolutionHeight() + rightPart.resolutionHeight());
                        removed[j] = true;
                        qDebug() << "join displays counter-clockwise rotation" << i << j;
                        break;
                    }

                    removed[j] = true;
                }
        }

    for (int i = n - 1; i >= 0; i--)
        if (removed[uint(i)])
            displays_.erase(displays_.begin() + i);
}

void DisplayEnumerator::joinEizoDisplays()
{
    qDebug() << "DisplayEnumerator::joinEizoDisplays()";

    const size_t n = displays_.size();
    std::vector<bool> removed(n, false);

    for (size_t i = 0; i < n; i++)
    {
        auto &leftPart = displays_[i];

        if (!isEizoPart(leftPart) )
            continue;

        qDebug() << "EIZO left display part" << i;

        for (size_t j = 0; j < n; j++)
        {
            if (i == j)
                continue;

            auto &rightPart = displays_[j];
            if (!isEizoPart(rightPart) ||
                leftPart.serial() != rightPart.serial() ||
                leftPart.name()   != rightPart.name())
                continue;

            if (leftPart.width()  != rightPart.width() ||
                leftPart.height() != rightPart.height())
                continue;

            bool isHorizontal = leftPart.width() < leftPart.height();
            const int gap = 20;

//            isHorizontal = !isHorizontal; // DELL testing

            qDebug() << "EIZO right display part" << j << "is horizontal" << isHorizontal;

            if (isHorizontal && (abs(leftPart.top() - rightPart.top()) <= gap &&
                                 abs(leftPart.left() + leftPart.width() - rightPart.left()) <= gap) )
            {
                leftPart.setWidth(leftPart.width() + rightPart.width());
                leftPart.setResolutionWidth(leftPart.resolutionWidth() + rightPart.resolutionWidth());
                leftPart.setWidth_mm(leftPart.width_mm() + rightPart.width_mm());
                removed[j] = true;
                qDebug() << "join EIZO displays horizontally" << i << j;
                break;
            }

            // clock-wise rotation
            if (!isHorizontal && (abs(leftPart.left() - rightPart.left()) <= gap ||
                                  abs(leftPart.top() + leftPart.height() - rightPart.top()) <= gap) )
            {
                leftPart.setHeight(leftPart.height() + rightPart.height());
                leftPart.setResolutionHeight(leftPart.resolutionHeight() + rightPart.resolutionHeight());
                leftPart.setHeight_mm(leftPart.height_mm() + rightPart.height_mm());
                removed[j] = true;
                qDebug() << "join EIZO displays clockwise rotation" << i << j;
                break;
            }
        }
    }

    for (int i = int(n - 1); i >= 0; i--)
        if (removed[uint(i)])
            displays_.erase(displays_.begin() + i);
}

bool DisplayEnumerator::isWideLeftPart(DisplayInfo display)
{
    if (display.manufacture() != "WIDE" && display.manufacture() != "WKC")
        return false;

    std::vector<QString> wideNames = {"CW60", "CW60N"};
    for (auto name : wideNames)
        if (display.name().startsWith(name) &&
            display.serial().startsWith(name) )
            return true;

    return false;
}

bool DisplayEnumerator::isWideRightPart(DisplayInfo display)
{
    if (display.manufacture() != "WIDE" && display.manufacture() != "WKC")
        return false;

    std::vector<QString> wideNames = {"CW60", "CW60N"};
    for (auto name : wideNames)
        if (display.name().startsWith(name) &&
            !display.serial().startsWith(name) )
            return true;

    return false;
}

bool DisplayEnumerator::isEizoPart(DisplayInfo display)
{
//    if (display.manufacturer == "DELL" || display.manufacturer == "DEL" || display.manufacturer == "Dell")
//        return true; // DELL testing

    if (display.manufacture() != "EIZO" && display.manufacture() != "ENC")
        return false;

    std::vector<QString> models = {"RX850", "RX650"};
    for (auto name : models)
        if (display.name().startsWith(name))
            return true;

    return false;
}
