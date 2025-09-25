#include "Graphics1DisplayDLL.h"
#include <QDebug>
#include <cstring>

// Global variables to store display information
static std::vector<QRect> g_geometry;
static std::vector<QSize> g_resolution;
static std::vector<QByteArray> g_edids;
static std::vector<QString> g_osIds;
static std::vector<QString> g_osDevNames;
static std::vector<QString> g_videoAdapterKey;
static std::vector<QString> g_videoAdapterName;

extern "C" {

GRAPHICS1DISPLAYDLL_EXPORT bool graphics1DisplayDLL_Load()
{
    qDebug() << "Graphics1 Display DLL: Loading...";
    
    // Clear previous data
    g_geometry.clear();
    g_resolution.clear();
    g_edids.clear();
    g_osIds.clear();
    g_osDevNames.clear();
    g_videoAdapterKey.clear();
    g_videoAdapterName.clear();
    
    return true;
}

GRAPHICS1DISPLAYDLL_EXPORT void graphics1DisplayDLL_Unload()
{
    qDebug() << "Graphics1 Display DLL: Unloading...";
    
    // Clear data
    g_geometry.clear();
    g_resolution.clear();
    g_edids.clear();
    g_osIds.clear();
    g_osDevNames.clear();
    g_videoAdapterKey.clear();
    g_videoAdapterName.clear();
}

GRAPHICS1DISPLAYDLL_EXPORT bool graphics1DisplayDLL_EnumerateDisplays()
{
    qDebug() << "Graphics1 Display DLL: Enumerating displays...";
    
    // Note: Graphics1 functionality is now handled by the compiled DLL
    // This is a placeholder implementation
    qDebug() << "Graphics1 Display DLL: Enumerating displays (placeholder)";
    return true;
}

GRAPHICS1DISPLAYDLL_EXPORT bool graphics1DisplayDLL_UpdateDisplayInfo(
    int* geometry_x, int* geometry_y, int* geometry_width, int* geometry_height,
    int* resolution_width, int* resolution_height,
    unsigned char* edid_data, int edid_size,
    char* os_id, int os_id_size,
    char* os_dev_name, int os_dev_name_size,
    char* video_adapter_key, int video_adapter_key_size,
    char* video_adapter_name, int video_adapter_name_size,
    int* display_count
)
{
    if (!display_count) return false;
    
    *display_count = g_geometry.size();
    
    if (g_geometry.empty()) return false;
    
    // Copy data to output arrays
    for (size_t i = 0; i < g_geometry.size(); ++i) {
        if (geometry_x) geometry_x[i] = g_geometry[i].x();
        if (geometry_y) geometry_y[i] = g_geometry[i].y();
        if (geometry_width) geometry_width[i] = g_geometry[i].width();
        if (geometry_height) geometry_height[i] = g_geometry[i].height();
        
        if (resolution_width) resolution_width[i] = g_resolution[i].width();
        if (resolution_height) resolution_height[i] = g_resolution[i].height();
        
        if (edid_data && edid_size > 0 && i < g_edids.size()) {
            int copy_size = qMin(edid_size, g_edids[i].size());
            memcpy(edid_data + i * edid_size, g_edids[i].data(), copy_size);
        }
        
        if (os_id && os_id_size > 0 && i < g_osIds.size()) {
            strncpy(os_id + i * os_id_size, g_osIds[i].toUtf8().data(), os_id_size - 1);
            os_id[i * os_id_size + os_id_size - 1] = '\0';
        }
        
        if (os_dev_name && os_dev_name_size > 0 && i < g_osDevNames.size()) {
            strncpy(os_dev_name + i * os_dev_name_size, g_osDevNames[i].toUtf8().data(), os_dev_name_size - 1);
            os_dev_name[i * os_dev_name_size + os_dev_name_size - 1] = '\0';
        }
        
        if (video_adapter_key && video_adapter_key_size > 0 && i < g_videoAdapterKey.size()) {
            strncpy(video_adapter_key + i * video_adapter_key_size, g_videoAdapterKey[i].toUtf8().data(), video_adapter_key_size - 1);
            video_adapter_key[i * video_adapter_key_size + video_adapter_key_size - 1] = '\0';
        }
        
        if (video_adapter_name && video_adapter_name_size > 0 && i < g_videoAdapterName.size()) {
            strncpy(video_adapter_name + i * video_adapter_name_size, g_videoAdapterName[i].toUtf8().data(), video_adapter_name_size - 1);
            video_adapter_name[i * video_adapter_name_size + video_adapter_name_size - 1] = '\0';
        }
    }
    
    return true;
}

} // extern "C"
