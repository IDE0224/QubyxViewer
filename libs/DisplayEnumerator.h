#ifndef DISPLAYENUMERATOR_H
#define	DISPLAYENUMERATOR_H

#include <vector>
#include <QRect>
#include <QSize>

#include "DisplayInfo.h"
#include "VideoCardInfo.h"
#include "SerialPort.h"

class DisplayEnumerator
{
    std::vector<DisplayInfo> displays_;
    std::vector<VideoCardInfo> videoadapters_;

    DisplayInfo empty_;

public:
    DisplayEnumerator();
    virtual ~DisplayEnumerator();

    void EnumerateDisplays();
    
    unsigned int Count();
    const DisplayInfo& GetDisplay(unsigned int pos);
    DisplayInfo& operator[](unsigned int pos);
    void ClearInformation();
    bool RemoveDisplay(unsigned int pos);
    
    QString GetVideocardNameByKey(const QString &key);
    
    void AddDisplay(const DisplayInfo& info);
    
private:
     void FillInfo(std::vector<QRect> &geometry, std::vector<QSize> &resolution,
                      std::vector<QByteArray>& edids, std::vector<QString> &osIds, std::vector<QString> &osDevNames, std::vector<QString> &videoadapterkey,  std::vector<QString> &videoadaptername);
     
     void FillInfo(const DisplayInfo &info);

     void joinWideDisplays();
     void joinEizoDisplays();

     bool isWideLeftPart(DisplayInfo display);
     bool isWideRightPart(DisplayInfo display);

     bool isEizoPart(DisplayInfo display);
};

#endif	/* DISPLAYENUMERATOR_H */

