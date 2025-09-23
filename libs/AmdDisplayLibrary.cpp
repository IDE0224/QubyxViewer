#include "AmdDisplayLibrary.h"
//#include <QDebug>

#ifdef _WIN32

HINSTANCE AmdDisplayLibrary::hDLL_=nullptr;
std::mutex AmdDisplayLibrary::loadMutex_;
int AmdDisplayLibrary::loadCount_=0;

tADL_MAIN_CONTROL_CREATE          AmdDisplayLibrary::ADL_Main_Control_Create_=nullptr;
tADL_MAIN_CONTROL_DESTROY         AmdDisplayLibrary::ADL_Main_Control_Destroy_=nullptr;
tADL_ADAPTER_NUMBEROFADAPTERS_GET AmdDisplayLibrary::ADL_Adapter_NumberOfAdapters_Get_=nullptr;
tADL_ADAPTER_ADAPTERINFO_GET      AmdDisplayLibrary::ADL_Adapter_AdapterInfo_Get_=nullptr;
tADL_DISPLAY_DISPLAYINFO_GET      AmdDisplayLibrary::ADL_Display_DisplayInfo_Get_=nullptr;

tADL_Display_WriteAndReadI2CRev_Get AmdDisplayLibrary::ADL_Display_WriteAndReadI2CRev_Get_=nullptr;
tADL_Display_WriteAndReadI2C        AmdDisplayLibrary::ADL_Display_WriteAndReadI2C_=nullptr;
tADL_Display_DDCBlockAccess_Get     AmdDisplayLibrary::ADL_Display_DDCBlockAccess_Get_=nullptr;
tADL_Display_DDCInfo_Get            AmdDisplayLibrary::ADL_Display_DDCInfo_Get_=nullptr;
tADL_Display_EdidData_Get           AmdDisplayLibrary::ADL_Display_EdidData_Get_=nullptr;

tADL_Display_DisplayMapConfig_Get   AmdDisplayLibrary::ADL_Display_DisplayMapConfig_Get_=nullptr;
tADL_Display_Modes_Get              AmdDisplayLibrary::ADL_Display_Modes_Get_=nullptr;
tADL_Display_SLSMapConfig_Get       AmdDisplayLibrary::ADL_Display_SLSMapConfig_Get_=nullptr;
tADL_Display_SLSMapIndex_Get        AmdDisplayLibrary::ADL_Display_SLSMapIndex_Get_=nullptr;

bool AmdDisplayLibrary::load()
{
    std::lock_guard<std::mutex> locker(loadMutex_);

    if (loadCount_>0)
    {
        if (hDLL_)
        {
            ++loadCount_;
            return true;
        }

        loadCount_ = 0;
    }

    hDLL_ = LoadLibraryA("atiadlxx.dll");

    if (!hDLL_)
    {
//        qDebug()<<"AmdDisplayLibrary: A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.";
        hDLL_ = LoadLibraryA("atiadlxy.dll");
    }

    //try barco
    if (!hDLL_)
    {
//        qDebug()<<"AmdDisplayLibrary: Try Barco version.";
        hDLL_ = LoadLibraryA("b7atiadlxx.dll");
    }

    if (!hDLL_)
    {
//        qDebug()<<"AmdDisplayLibrary: Try 32 bit Barco version.";
        hDLL_ = LoadLibraryA("b7atiadlxy.dll");
    }

    if (!hDLL_)
    {
//        qDebug()<<"AmdDisplayLibrary: ADL library not found!\n";
        return false;
    }


    ++loadCount_;



    ADL_Main_Control_Create_ = (tADL_MAIN_CONTROL_CREATE) GetProcAddress(hDLL_,"ADL_Main_Control_Create");
    ADL_Main_Control_Destroy_ = (tADL_MAIN_CONTROL_DESTROY) GetProcAddress(hDLL_,"ADL_Main_Control_Destroy");
    ADL_Adapter_NumberOfAdapters_Get_ = (tADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(hDLL_,"ADL_Adapter_NumberOfAdapters_Get");
    ADL_Adapter_AdapterInfo_Get_ = (tADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(hDLL_,"ADL_Adapter_AdapterInfo_Get");
    ADL_Display_DisplayInfo_Get_ = (tADL_DISPLAY_DISPLAYINFO_GET) GetProcAddress(hDLL_,"ADL_Display_DisplayInfo_Get");

    ADL_Display_WriteAndReadI2CRev_Get_ = (tADL_Display_WriteAndReadI2CRev_Get)GetProcAddress(hDLL_,"ADL_Display_WriteAndReadI2CRev_Get");
    ADL_Display_WriteAndReadI2C_        = (tADL_Display_WriteAndReadI2C)GetProcAddress(hDLL_,"ADL_Display_WriteAndReadI2C");
    ADL_Display_DDCBlockAccess_Get_     = (tADL_Display_DDCBlockAccess_Get)GetProcAddress(hDLL_,"ADL_Display_DDCBlockAccess_Get");
    ADL_Display_DDCInfo_Get_            = (tADL_Display_DDCInfo_Get)GetProcAddress(hDLL_,"ADL_Display_DDCInfo_Get");
    ADL_Display_EdidData_Get_           = (tADL_Display_EdidData_Get)GetProcAddress(hDLL_,"ADL_Display_EdidData_Get");

    ADL_Display_DisplayMapConfig_Get_   = (tADL_Display_DisplayMapConfig_Get)GetProcAddress(hDLL_, "ADL_Display_DisplayMapConfig_Get");
    ADL_Display_Modes_Get_              = (tADL_Display_Modes_Get)GetProcAddress(hDLL_, "ADL_Display_Modes_Get");
    ADL_Display_SLSMapConfig_Get_       = (tADL_Display_SLSMapConfig_Get)GetProcAddress(hDLL_, "ADL_Display_SLSMapConfig_Get");
    ADL_Display_SLSMapIndex_Get_        = (tADL_Display_SLSMapIndex_Get)GetProcAddress(hDLL_, "ADL_Display_SLSMapIndex_Get");

    if (ADL_Main_Control_Create_)
    {
        bool ok = init();
        if (!ok)
            return false;
    }

    if ( !ADL_Main_Control_Create_ ||
         !ADL_Main_Control_Destroy_ ||
         !ADL_Adapter_NumberOfAdapters_Get_ ||
         !ADL_Adapter_AdapterInfo_Get_ ||
         !ADL_Display_DisplayInfo_Get_ ||
         !ADL_Display_WriteAndReadI2CRev_Get_ ||
         !ADL_Display_WriteAndReadI2C_ ||
         !ADL_Display_DDCBlockAccess_Get_ ||
         !ADL_Display_DDCInfo_Get_ ||
         !ADL_Display_EdidData_Get_)
        {
//           qDebug()<<"AmdDisplayLibrary: ADL's API is missing!\n";
           unload();
           return false;
        }

    return true;
}

bool AmdDisplayLibrary::unload()
{
    std::lock_guard<std::mutex> locker(loadMutex_);

    if (!hDLL_)
    {
        --loadCount_;
        return true;
    }

    --loadCount_;
    if (loadCount_<0)
        loadCount_ = 0;

    if (loadCount_>0)
        return true;

    if (ADL_Main_Control_Create_ && ADL_Main_Control_Destroy_)
        deinit();

    bool res = FreeLibrary(hDLL_);

    ADL_Main_Control_Create_ = nullptr;
    ADL_Main_Control_Destroy_ = nullptr;
    ADL_Adapter_NumberOfAdapters_Get_ = nullptr;
    ADL_Adapter_AdapterInfo_Get_ = nullptr;
    ADL_Display_DisplayInfo_Get_ = nullptr;

    ADL_Display_WriteAndReadI2CRev_Get_ = nullptr;
    ADL_Display_WriteAndReadI2C_        = nullptr;
    ADL_Display_DDCBlockAccess_Get_     = nullptr;
    ADL_Display_DDCInfo_Get_            = nullptr;
    ADL_Display_EdidData_Get_           = nullptr;

    ADL_Display_DisplayMapConfig_Get_   = nullptr;
    ADL_Display_Modes_Get_              = nullptr;
    ADL_Display_SLSMapConfig_Get_       = nullptr;
    ADL_Display_SLSMapIndex_Get_        = nullptr;

    return res;
}

bool AmdDisplayLibrary::init()
{
//    qDebug() << "AmdDisplayLibrary::init()";
    // Initialize ADL. The second parameter is 1, which means:
    // retrieve adapter information only for adapters that are physically present and enabled in the system
    if ( ADL_OK != ADL_Main_Control_Create_(memoryAlloc, 1) )
    {
//        qDebug()<<"AmdDisplayLibrary:ADL Initialization Error!";
        return false;
    }

    return true;
}

bool AmdDisplayLibrary::deinit()
{
    return ADL_Main_Control_Destroy_();
}

void *AmdDisplayLibrary::memoryAlloc(int size)
{
    return malloc(size);
}

void AmdDisplayLibrary::memoryFree(void **buffer)
{
    if (*buffer)
    {
        free(*buffer);
        *buffer = nullptr;
    }
}

AmdDisplayLibrary::AmdDisplayLibrary()
{

}

#endif
