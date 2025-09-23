#include <qglobal.h>
#ifdef Q_OS_WIN

#include "amdcombineddisplaysplit.h"

#include "libs/AmdDisplayLibrary.h"

#include <QDebug>
#include <memory>

AMDCombinedDisplaySplit::AMDCombinedDisplaySplit()
{

}

bool AMDCombinedDisplaySplit::enumerateCombinedDisplaysInfo()
{
    if (!AmdDisplayLibrary::load())
        return false;

    if (!AmdDisplayLibrary::ADL_Display_DisplayMapConfig_Get_
       || !AmdDisplayLibrary::ADL_Display_Modes_Get_
       || !AmdDisplayLibrary::ADL_Display_SLSMapConfig_Get_
       || !AmdDisplayLibrary::ADL_Display_SLSMapIndex_Get_)
    {
        AmdDisplayLibrary::unload();
        return false;
    }

    displays_.clear();

    int iNumberAdapters = 0;
    if (ADL_OK != AmdDisplayLibrary::ADL_Adapter_NumberOfAdapters_Get_(&iNumberAdapters)
       || iNumberAdapters==0)
    {
        AmdDisplayLibrary::unload();
        return false;
    }

    std::unique_ptr<AdapterInfo[]> adapterInfos(new AdapterInfo[iNumberAdapters]);

    AmdDisplayLibrary::ADL_Adapter_AdapterInfo_Get_(adapterInfos.get(), int(sizeof(AdapterInfo) * uint(iNumberAdapters)));

    qDebug() << "iNumberAdapters" << iNumberAdapters;

    for (int adapter=0;adapter<iNumberAdapters;++adapter)
    {
        QString adapterName = QString(adapterInfos[adapter].strAdapterName);
        qDebug() << "adapter name:" << adapterName << adapter;
        if (adapterName.contains("NVidia", Qt::CaseInsensitive) ||
            adapterName.contains("Intel", Qt::CaseInsensitive) ||
            adapterName.contains("Matrox", Qt::CaseInsensitive))
        {
            qDebug() << "skipped";
            continue;
        }

        int iNumDisplayTarget = 0;
        ADLDisplayTarget *lpDisplayTarget = nullptr;
        int iNumDisplayMap = 0;
        ADLDisplayMap *lpDisplayMap = nullptr;


        qDebug() << "================================";
        qDebug() << "adapter #"<<adapter;

//        {
//            //debug displays

//            LPADLDisplayInfo  lpAdlDisplayInfo=nullptr;
//            int iNumDisplays;
//            if (ADL_OK == AmdDisplayLibrary::ADL_Display_DisplayInfo_Get_(adapterInfos[adapter].iAdapterIndex, &iNumDisplays, &lpAdlDisplayInfo, 0))
//            {
//               qDebug() << "---------------------";
//               for (int j = 0; j < iNumDisplays; j++ )
//               {
//                  qDebug()<<"display #"<<j;
//                  // For each display, check its status. Use the display only if it's connected AND mapped (iDisplayInfoValue: bit 0 and 1 )
//                  if (  (ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) !=
//                       ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED)& lpAdlDisplayInfo[j].iDisplayInfoValue)  )
//                       continue;   // Skip the not connected or not mapped displays

//                  qDebug() << "displayid.adapterid " << lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex;
//                  qDebug() << "displayid.displayid " << lpAdlDisplayInfo[j].displayID.iDisplayLogicalIndex;
//                  qDebug() << "displayid.physadapterid " << lpAdlDisplayInfo[j].displayID.iDisplayPhysicalAdapterIndex;
//                  qDebug() << "displayid.physdisplayid " << lpAdlDisplayInfo[j].displayID.iDisplayPhysicalIndex;

//                  qDebug() << "manufacturer" << lpAdlDisplayInfo[j].strDisplayManufacturerName;
//                  qDebug() << "name" << lpAdlDisplayInfo[j].strDisplayName;

//               }
//            }

//            AmdDisplayLibrary::memoryFree((void**)lpAdlDisplayInfo);
//        }


        if (ADL_OK != AmdDisplayLibrary::ADL_Display_DisplayMapConfig_Get_(adapterInfos[uint(adapter)].iAdapterIndex,
                                                                          &iNumDisplayMap, &lpDisplayMap,
                                                                          &iNumDisplayTarget, &lpDisplayTarget,
                                                                          ADL_DISPLAY_DISPLAYMAP_OPTION_GPUINFO ) )
        {
            qDebug() << "can't call ADL_Display_DisplayMapConfig_Get_ for " << adapter;

            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayMap));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayTarget));
            continue;
        }

//        qDebug() << "---------------------";
//        for (int j = 0; j < iNumDisplayTarget; j++ )
//        {
//           qDebug()<<"target display #"<<j;

//           qDebug() << "displayid.adapterid " << lpDisplayTarget[j].displayID.iDisplayLogicalAdapterIndex;
//           qDebug() << "displayid.displayid " << lpDisplayTarget[j].displayID.iDisplayLogicalIndex;
//           qDebug() << "displayid.physadapterid " << lpDisplayTarget[j].displayID.iDisplayPhysicalAdapterIndex;
//           qDebug() << "displayid.physdisplayid " << lpDisplayTarget[j].displayID.iDisplayPhysicalIndex;

//           qDebug() << "map index" << lpDisplayTarget[j].iDisplayMapIndex;
//           qDebug() << "target mask" << lpDisplayTarget[j].iDisplayTargetMask;
//           qDebug() << "target value" << lpDisplayTarget[j].iDisplayTargetValue;

//        }

        if (iNumDisplayTarget<2)
        {
            qDebug() << "Only one display for adapter" << adapter;

            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayMap));
            qDebug() << "memoryFree 1";
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayTarget));
            qDebug() << "memoryFree 2";
            continue;
        }

        int iSLSMapIndex;
        if ( ADL_OK != AmdDisplayLibrary::ADL_Display_SLSMapIndex_Get_(adapterInfos[uint(adapter)].iAdapterIndex, iNumDisplayTarget, lpDisplayTarget, &iSLSMapIndex) )
        {
            qDebug() << "can't call ADL_Display_SLSMapIndex_Get_ for " << adapter;

            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayMap));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayTarget));
            continue;
        }

        //This is a temporary workaround to enable SLS.
        //Set this variable to any value.
        SetEnvironmentVariableA("ADL_4KWORKAROUND_CANCEL", "TRUE");


        int iNumSLSTarget = 0;
        ADLSLSTarget *lpSLSTarget = nullptr;
        int iNumNativeMode = 0;
        int iCurrentNativeMode = 0;
        ADLSLSMode *lpNativeMode = nullptr;
        int iNumBezelMode = 0;
        int iCurrentBezelMode = 0;
        ADLBezelTransientMode *lpBezelMode = nullptr;
        int iNumTransientMode = 0;
        ADLBezelTransientMode *lpTransientMode = nullptr;
        int iNumSLSOffset = 0;
        ADLSLSOffset *lpSLSOffset = nullptr;
        ADLSLSMap SLSMap = {0, 0, {0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        if ( ADL_OK != AmdDisplayLibrary::ADL_Display_SLSMapConfig_Get_(adapterInfos[uint(adapter)].iAdapterIndex, iSLSMapIndex,
                                                                        &SLSMap,
                                                                        &iNumSLSTarget, &lpSLSTarget,
                                                                        &iNumNativeMode, &lpNativeMode,
                                                                        &iNumBezelMode, &lpBezelMode,
                                                                        &iNumTransientMode, &lpTransientMode,
                                                                        &iNumSLSOffset, &lpSLSOffset,
                                                                        ADL_DISPLAY_SLSGRID_CAP_OPTION_RELATIVETO_CURRENTANGLE ) )
        {
            qDebug() << "can't call ADL_Display_SLSMapConfig_Get_ for " << adapter;

            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpSLSTarget));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpNativeMode));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpBezelMode));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpTransientMode));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpSLSOffset));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayMap));
            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayTarget));
            continue;
        }

//        qDebug() << "---------------------";
//        qDebug() << "SLSMap";
//        qDebug() << "iSLSGridColumn " << SLSMap.grid.iSLSGridColumn;
//        qDebug() << "iSLSGridRow " << SLSMap.grid.iSLSGridRow;
//        qDebug() << "iSLSGridValue " << SLSMap.grid.iSLSGridValue;

//        qDebug() << "---------------------";
//        for (int j = 0; j < iNumSLSTarget; j++ )
//        {
//           qDebug()<<"sls traget #"<<j;

//           qDebug() << "iSLSGridPositionX " << lpSLSTarget[j].iSLSGridPositionX;
//           qDebug() << "iSLSGridPositionY " << lpSLSTarget[j].iSLSGridPositionY;
//           qDebug() << "viewSize.iXPos " << lpSLSTarget[j].viewSize.iXPos;
//           qDebug() << "viewSize.iYPos " << lpSLSTarget[j].viewSize.iYPos;
//           qDebug() << "viewSize.iXRes " << lpSLSTarget[j].viewSize.iXRes;
//           qDebug() << "viewSize.iYRes " << lpSLSTarget[j].viewSize.iYRes;

//           qDebug() << "iSLSMapIndex " << lpSLSTarget[j].iSLSMapIndex;

//           qDebug() << "displayTarget.displayID.iDisplayLogicalAdapterIndex " << lpSLSTarget[j].displayTarget.displayID.iDisplayLogicalAdapterIndex;
//           qDebug() << "displayTarget.displayID.iDisplayLogicalIndex " << lpSLSTarget[j].displayTarget.displayID.iDisplayLogicalIndex;
//        }

//        qDebug() << "---------------------";
//        for (int j = 0; j < iNumSLSOffset; j++ )
//        {
//           qDebug()<<"sls offset #"<<j;

//           qDebug() << "iSLSMapIndex " << lpSLSOffset[j].iSLSMapIndex;
//           qDebug() << "iDisplayWidth " << lpSLSOffset[j].iDisplayWidth;
//           qDebug() << "iDisplayHeight " << lpSLSOffset[j].iDisplayHeight;
//           qDebug() << "iDisplayLogicalAdapterIndex " << lpSLSOffset[j].displayID.iDisplayLogicalAdapterIndex;
//           qDebug() << "iDisplayLogicalIndex " << lpSLSOffset[j].displayID.iDisplayLogicalIndex;

//        }

//        qDebug() << "---------------------";
//        for (int j = 0; j < iNumNativeMode; j++ )
//        {
//           qDebug()<<"native mode #"<<j;

//           qDebug() << "displayid.adapterid " << lpNativeMode[j].displayMode.displayID.iDisplayLogicalAdapterIndex;
//           qDebug() << "displayid.displayid " << lpNativeMode[j].displayMode.displayID.iDisplayLogicalIndex;
//           qDebug() << "displayid.physadapterid " << lpNativeMode[j].displayMode.displayID.iDisplayPhysicalAdapterIndex;
//           qDebug() << "displayid.physdisplayid " << lpNativeMode[j].displayMode.displayID.iDisplayPhysicalIndex;

//           qDebug() << "pos x" << lpNativeMode[j].displayMode.iXPos;
//           qDebug() << "pos y" << lpNativeMode[j].displayMode.iYPos;
//           qDebug() << "res x" << lpNativeMode[j].displayMode.iXRes;
//           qDebug() << "res y" << lpNativeMode[j].displayMode.iYRes;

//           qDebug() << "sls map index" << lpNativeMode[j].iSLSMapIndex;
//           qDebug() << "sls mode index" << lpNativeMode[j].iSLSModeIndex;
//           qDebug() << "sls native mode mask" << lpNativeMode[j].iSLSNativeModeMask;
//           qDebug() << "sls native mode value" << lpNativeMode[j].iSLSNativeModeValue;
//        }
//        qDebug() << "---------------------";
//        for (int j = 0; j < iNumBezelMode; j++ )
//        {
//           qDebug()<<"bezel  mode #"<<j;

//           qDebug() << "displayid.adapterid " << lpBezelMode[j].displayMode.displayID.iDisplayLogicalAdapterIndex;
//           qDebug() << "displayid.displayid " << lpBezelMode[j].displayMode.displayID.iDisplayLogicalIndex;
//           qDebug() << "displayid.physadapterid " << lpBezelMode[j].displayMode.displayID.iDisplayPhysicalAdapterIndex;
//           qDebug() << "displayid.physdisplayid " << lpBezelMode[j].displayMode.displayID.iDisplayPhysicalIndex;

//           qDebug() << "pos x" << lpBezelMode[j].displayMode.iXPos;
//           qDebug() << "pos y" << lpBezelMode[j].displayMode.iYPos;
//           qDebug() << "res x" << lpBezelMode[j].displayMode.iXRes;
//           qDebug() << "res y" << lpBezelMode[j].displayMode.iYRes;

//           qDebug() << "sls map index" << lpBezelMode[j].iSLSMapIndex;
//           qDebug() << "sls mode index" << lpBezelMode[j].iSLSModeIndex;
//        }


        for (int display=0;display<iNumDisplayTarget;++display)
        {
            int iNumModes = 0;
            ADLMode *lpModes = nullptr;

            if ( ADL_OK != AmdDisplayLibrary::ADL_Display_Modes_Get_(adapterInfos[uint(adapter)].iAdapterIndex,
                                                                  lpDisplayTarget[display].displayID.iDisplayLogicalIndex,
                                                                  &iNumModes, &lpModes ) )
            {
                qDebug() << "can't call ADL_Display_Modes_Get for " << adapter << display;
                AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpModes));
                continue;
            }

            //check is display really in SLS (Single Large Surface)
            bool found = false;
            std::map<int, bool> mapindexes;
            for ( iCurrentNativeMode=0; iCurrentNativeMode<iNumNativeMode; iCurrentNativeMode++)
//                if ( lpModes->displayID.iDisplayLogicalIndex == lpNativeMode[iCurrentNativeMode].displayMode.displayID.iDisplayLogicalIndex)
                if ( lpModes->iXRes == lpNativeMode[iCurrentNativeMode].displayMode.iXRes
                    && lpModes->iYRes == lpNativeMode[iCurrentNativeMode].displayMode.iYRes)
                {
                    qDebug() << "!!! found match for display-"<<display<<"and native mode" << iCurrentNativeMode;
                    found = true;
                    mapindexes[lpNativeMode[iCurrentNativeMode].iSLSMapIndex] = true;
                }

            for ( iCurrentBezelMode=0; iCurrentBezelMode<iNumBezelMode; iCurrentBezelMode++)
//                if ( lpModes->displayID.iDisplayLogicalIndex == lpBezelMode[iCurrentNativeMode].displayMode.displayID.iDisplayLogicalIndex)
                if ( lpModes->iXRes == lpBezelMode[iCurrentBezelMode].displayMode.iXRes
                    && lpModes->iYRes == lpBezelMode[iCurrentBezelMode].displayMode.iYRes)
                {
                    qDebug() << "!!! found match for display-"<<display<<"and bezel mode" << iCurrentNativeMode;
                    found = true;
                    mapindexes[lpBezelMode[iCurrentBezelMode].iSLSMapIndex] = true;
                }

            if (found)
            {
                for (int did=0;did<iNumSLSTarget;++did)
                {
                    if (mapindexes.find(lpSLSTarget[did].iSLSMapIndex)!=mapindexes.end())
                    {
                        DisplayDetails d;
                        d.adapterIndex = lpSLSTarget[did].displayTarget.displayID.iDisplayLogicalAdapterIndex;
                        d.displayIndex = lpSLSTarget[did].displayTarget.displayID.iDisplayLogicalIndex;
                        d.x = lpModes->iXPos + lpModes->iXRes*lpSLSTarget[did].iSLSGridPositionX/SLSMap.grid.iSLSGridColumn;
                        d.y = lpModes->iYPos + lpModes->iYRes*lpSLSTarget[did].iSLSGridPositionY/SLSMap.grid.iSLSGridRow;
                        d.width = lpModes->iXRes/SLSMap.grid.iSLSGridColumn;
                        d.height = lpModes->iYRes/SLSMap.grid.iSLSGridRow;

                        displays_[adapterInfos[uint(adapter)].strDisplayName].push_back(d);
                    }
                }
            }

            AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpModes));
        }


        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpSLSTarget));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpNativeMode));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpBezelMode));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpTransientMode));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpSLSOffset));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayMap));
        AmdDisplayLibrary::memoryFree(reinterpret_cast <void**>(&lpDisplayTarget));
    }

    qDebug() << "enumeration end";

    updateEDIDS();

    AmdDisplayLibrary::unload();

    qDebug() << "AMDCombinedDisplaySplit::enumerateCombinedDisplaysInfo() end";

    return !displays_.empty();
}

bool AMDCombinedDisplaySplit::updateDisplaysInfo(std::vector<QRect> &geometry, std::vector<QSize> &resolution, std::vector<QByteArray> &edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey, std::vector<QString> &videoadaptername)
{
    for (int i = int(osIds.size()) - 1; i >= 0; --i){
        auto itr = displays_.find(osIds[uint(i)]);
        if (itr==displays_.end())
            continue;

        for (auto d : itr->second)
        {
            geometry.push_back(QRect(d.x, d.y, d.width, d.height));
            resolution.push_back(QSize(d.width, d.height));
            edids.push_back(QByteArray(reinterpret_cast <const char*>(d.EDID), 128));
            osIds.push_back(osIds[uint(i)]);
            osDevNames.push_back(osDevNames[uint(i)]);
            videoadapterkey.push_back(videoadapterkey[uint(i)]);
            videoadaptername.push_back(videoadaptername[uint(i)]);
        }

        geometry.erase(geometry.begin()+i);
        resolution.erase(resolution.begin()+i);
        edids.erase(edids.begin()+i);
        osIds.erase(osIds.begin()+i);
        osDevNames.erase(osDevNames.begin()+i);
        videoadapterkey.erase(videoadapterkey.begin()+i);
        videoadaptername.erase(videoadaptername.begin()+i);
    }


    return true;
}

bool AMDCombinedDisplaySplit::readRealEDID(int adapterIndex, int displayIndex, unsigned char *edid, int edidSize)
{
    qDebug()<<"AMDCombinedDisplaySplit: read EDID iAdapterIndex_ - "<<adapterIndex<<" iDisplayIndex_ - "<<displayIndex;
    ADLDisplayEDIDData lpEDIDData;
    lpEDIDData.iSize = sizeof(ADLDisplayEDIDData);
    lpEDIDData.iFlag = 0;
    lpEDIDData.iBlockIndex = 0;

    if (!AmdDisplayLibrary::ADL_Display_EdidData_Get_)
        return 0;

    qDebug()<<"ATIDDCIControl: read edid - "<<AmdDisplayLibrary::ADL_Display_EdidData_Get_(adapterIndex, displayIndex, &lpEDIDData);
    int sz = std::min<int>(edidSize, lpEDIDData.iSize);
    memcpy(edid, lpEDIDData.cEDIDData, uint(sz));

    qDebug() << QByteArray::fromRawData(lpEDIDData.cEDIDData, sz).toHex();

    return sz;
}

void AMDCombinedDisplaySplit::updateEDIDS()
{
    qDebug() << "AMDCombinedDisplaySplit::updateEDIDS()";
    for (auto& v : displays_)
        for (auto& d : v.second)
            readRealEDID(d.adapterIndex, d.displayIndex, d.EDID, 128);
}

#endif //Q_OS_WIN
