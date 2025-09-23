#ifndef AMDDISPLAYLIBRARY_H
#define AMDDISPLAYLIBRARY_H

#ifdef _WIN32

#include <mutex>
#include <windows.h>

#include "AMD/adl_sdk.h"

typedef int ( *tADL_MAIN_CONTROL_CREATE )(ADL_MAIN_MALLOC_CALLBACK, int );
typedef int ( *tADL_MAIN_CONTROL_DESTROY )();
typedef int ( *tADL_ADAPTER_NUMBEROFADAPTERS_GET ) ( int* );
typedef int ( *tADL_ADAPTER_ADAPTERINFO_GET ) ( LPAdapterInfo, int );
typedef int ( *tADL_DISPLAY_DISPLAYINFO_GET ) ( int, int *, ADLDisplayInfo **, int );



/*Function to retrieve the I2C API revision. (int iAdapterIndex, int *lpMajor, int *lpMinor)*/
typedef int (*tADL_Display_WriteAndReadI2CRev_Get) (int, int *, int *);

/*Function to write and read I2C. ((int iAdapterIndex, ADLI2C *plI2C))*/
typedef int (*tADL_Display_WriteAndReadI2C) (int , ADLI2C *);

/*Function to get Display DDC block access. (int iAdapterIndex, int iDisplayIndex, int iOption, int iCommandIndex, int iSendMsgLen, char *lpucSendMsgBuf, int *lpulRecvMsgLen, char *lpucRecvMsgBuf)*/
typedef int (*tADL_Display_DDCBlockAccess_Get) (int , int , int , int , int , char *, int *, char *);

/*Function to get the DDC info. (int iAdapterIndex, int iDisplayIndex, ADLDDCInfo *lpInfo)*/
typedef int (*tADL_Display_DDCInfo_Get) (int , int , ADLDDCInfo *);

/*Function to get the DDC info. (int iAdapterIndex, int iDisplayIndex, ADLDDCInfo2 *lpInfo)*/
typedef int (*tADL_Display_DDCInfo2_Get) (int , int , ADLDDCInfo2 *);

/*Function to get the EDID data. (int iAdapterIndex, int iDisplayIndex, ADLDisplayEDIDData *lpEDIDData)*/
typedef int (*tADL_Display_EdidData_Get) (int , int , ADLDisplayEDIDData *);

/*Function to retrieve current display map configurations.*/
typedef int (*tADL_Display_DisplayMapConfig_Get) (int ,int* ,ADLDisplayMap** ,int* ,ADLDisplayTarget** ,int);

/*Function to retrieve the display mode information.*/
typedef int (*tADL_Display_Modes_Get) (int, int, int*, ADLMode **);

/*Function to retrieve an SLS configuration.*/
typedef int (*tADL_Display_SLSMapConfig_Get) (int, int, ADLSLSMap *, int *, ADLSLSTarget **, int *, ADLSLSMode **, int *, ADLBezelTransientMode **, int *, ADLBezelTransientMode **, int *, ADLSLSOffset **, int);

/*Function to get the SLS map index for a given adapter and a given display device.*/
typedef int (*tADL_Display_SLSMapIndex_Get) (int, int, ADLDisplayTarget *, int *);

class AmdDisplayLibrary
{
public:
    static bool load();
    static bool unload();

    static void * __stdcall memoryAlloc(int size);
    static void __stdcall memoryFree(void** buffer);

    static tADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create_;
    static tADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy_;
    static tADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get_;
    static tADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get_;
    static tADL_DISPLAY_DISPLAYINFO_GET      ADL_Display_DisplayInfo_Get_;

    static tADL_Display_WriteAndReadI2CRev_Get ADL_Display_WriteAndReadI2CRev_Get_;
    static tADL_Display_WriteAndReadI2C        ADL_Display_WriteAndReadI2C_;
    static tADL_Display_DDCBlockAccess_Get     ADL_Display_DDCBlockAccess_Get_;
    static tADL_Display_DDCInfo_Get           ADL_Display_DDCInfo_Get_;
    static tADL_Display_EdidData_Get           ADL_Display_EdidData_Get_;

    static tADL_Display_DisplayMapConfig_Get ADL_Display_DisplayMapConfig_Get_;
    static tADL_Display_Modes_Get ADL_Display_Modes_Get_;
    static tADL_Display_SLSMapConfig_Get ADL_Display_SLSMapConfig_Get_;
    static tADL_Display_SLSMapIndex_Get ADL_Display_SLSMapIndex_Get_;

private:
    AmdDisplayLibrary();

    static HINSTANCE hDLL_;
    static std::mutex loadMutex_;
    static int loadCount_;

    static bool init();
    static bool deinit();

};

#endif

#endif // AMDDISPLAYLIBRARY_H
