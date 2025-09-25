#ifndef GRAPHICS1DISPLAYSTATICWRAPPER_H
#define GRAPHICS1DISPLAYSTATICWRAPPER_H

#include <QRect>
#include <QSize>
#include <QByteArray>
#include <QString>
#include <vector>

#ifdef Q_OS_WIN
class Graphics1DisplayStaticWrapper
{
public:
    Graphics1DisplayStaticWrapper();
    ~Graphics1DisplayStaticWrapper();
    
    bool load();
    void unload();
    bool enumerateDisplays();
    bool updateDisplayInfo(std::vector<QRect> &geometry, 
                          std::vector<QSize> &resolution,
                          std::vector<QByteArray> &edids, 
                          std::vector<QString> &osIds, 
                          std::vector<QString> &osDevNames,
                          std::vector<QString> &videoAdapterKey, 
                          std::vector<QString> &videoAdapterName);

private:
    bool m_loaded;
};

#else
// Empty implementation for non-Windows platforms
class Graphics1DisplayStaticWrapper
{
public:
    Graphics1DisplayStaticWrapper() {}
    ~Graphics1DisplayStaticWrapper() {}
    
    bool load() { return false; }
    void unload() {}
    bool enumerateDisplays() { return false; }
    bool updateDisplayInfo(std::vector<QRect> &, 
                          std::vector<QSize> &,
                          std::vector<QByteArray> &, 
                          std::vector<QString> &, 
                          std::vector<QString> &,
                          std::vector<QString> &, 
                          std::vector<QString> &) { return false; }
};
#endif

#endif // GRAPHICS1DISPLAYSTATICWRAPPER_H
