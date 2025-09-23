#ifndef AMDCOMBINEDDISPLAYSPLIT_H
#define AMDCOMBINEDDISPLAYSPLIT_H

#include <qglobal.h>
#ifdef Q_OS_WIN

#include <QRect>
#include <QByteArray>
#include <QString>

#include <vector>
#include <map>

class AMDCombinedDisplaySplit
{
public:
    AMDCombinedDisplaySplit();    
    bool enumerateCombinedDisplaysInfo();

    bool updateDisplaysInfo(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
                            std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames,
                            std::vector<QString> &videoadapterkey, std::vector<QString> &videoadaptername);

private:
    struct DisplayDetails
    {
        int adapterIndex;
        int displayIndex;
        int x, y, width, height;
        unsigned char EDID[128];
    };

    std::map<QString, std::vector<DisplayDetails>> displays_;

    bool readRealEDID(int adapterIndex, int displayIndex, unsigned char *edid, int edidSize);
    void updateEDIDS();
};

#endif //Q_OS_WIN

#endif // AMDCOMBINEDDISPLAYSPLIT_H
