#ifndef DISPLAYINFO_H
#define	DISPLAYINFO_H

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QString>

struct DisplayInfo
{
    unsigned char EDID[1024];
    unsigned int EDIDsize;

    QString OSId;
    QString OSDevName;
    QString VideoAdapterKey;
    QString VideoAdapterName;
    int calPatchSize_; //if -1 full screen or percent value 1..100;

    QString vendor3Symbols;
    QString name0xFC;
    QString name0xFE;
    unsigned char productCode[2];

    QString serialFromUser;

//    int left, top, width, height;
//    int resolutionWidth, resolutionHeight;
//    int widthMm, heightMm;

    int left()             const { return m_rect.left();         }
    int top()              const { return m_rect.top();          }
    int width()            const { return m_rect.width();        }
    int height()           const { return m_rect.height();       }
    int width_mm()         const { return m_size_mm.width();     }
    int height_mm()        const { return  m_size_mm.height();   }
    int resolutionWidth()  const { return m_resolution.width();  }
    int resolutionHeight() const { return m_resolution.height(); }
    QRect rect()           const { return m_rect;                }
    QSize resolution()     const { return m_resolution;          }
    QString resolutionStr()const { return QString("%1x%2").arg(m_resolution.width(), m_resolution.height()); }
    QSize size_mm()        const { return m_size_mm;             }
    QPoint topLeft()       const { return m_rect.topLeft();      }

    void setLeft(const int left);
    void setTop(const int top);
    void setLeftTop(QPoint leftTop);
    void setLeftTop(int left, int top);
    void setRect(QRect rect);
    void setWidth(int w);
    void setHeight(int h);
    void setResolution(QSize resolution);
    void setResolution(int w, int h);
    void setResolutionWidth(int w);
    void setResolutionHeight(int h);
    void setWidth_mm(int w);
    void setHeight_mm(int h);
    void setSize_mm(QSize size);
    void setSize_mm(int w, int h);
    void swapWidthHeight();

    QString name() const;
    void setName(QString newName);

    QString serial() const;
    void setSerial(QString newSerial);

    QString detectSerial() const;
    void setDetectSerial(QString newDSerial);

    QString manufacture() const;
    void setManufacture(QString newManufacture);

    enum ISensorEnum
    {
        ISUnknown,
        ISExists,
        ISNotExists
    } hasInternalSensor_, hasAmbientSensor_;
    bool specialCorrelation_ = false;
    
    bool specialRegistration_;
    
    DisplayInfo();

protected:
    QRect m_rect;
    QSize m_resolution;
    QSize m_size_mm;
    QString m_name;
    QString m_serial;
    QString m_detectSerial;
    QString m_manufacture;
};

#endif	/* DISPLAYINFO_H */

