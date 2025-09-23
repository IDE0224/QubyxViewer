#include "DisplayInfo.h"
#include <QDebug>

DisplayInfo::DisplayInfo()
    : m_name(""), m_serial(""), m_detectSerial(""), m_manufacture("")
{
    m_rect = QRect();
    m_resolution = QSize();
    m_size_mm = QSize();
//    left = top = width = height = 0;
//    resolutionWidth = resolutionHeight = 0;
//    widthMm = heightMm = 0;
    calPatchSize_ = -1;

    memset(EDID, 0, 1024*sizeof(char));
    EDIDsize = 0;

    productCode[0] = productCode[1] = 0;

    hasAmbientSensor_ = hasInternalSensor_ = ISUnknown;
    specialRegistration_ = false;
}

void DisplayInfo::setLeft(const int left)
{
    if (m_rect.left() == left)
        return;

    m_rect.setLeft(left);
}

void DisplayInfo::setTop(const int top)
{
    if (m_rect.top() == top)
        return;

    m_rect.setTop(top);
}

void DisplayInfo::setLeftTop(QPoint leftTop)
{
    if (m_rect.topLeft() == leftTop)
        return;

    m_rect.setTopLeft(leftTop);
}

void DisplayInfo::setLeftTop(int left, int top)
{
    setLeftTop(QPoint(left, top));
}

void DisplayInfo::setRect(QRect rect)
{
    if (m_rect == rect)
        return;

    qDebug() << "change rect for display" << m_name << m_serial << "from" << m_rect << "to" << rect;
    m_rect = rect;
}

void DisplayInfo::setWidth(int w)
{
    if (m_rect.width() == w)
        return;

    m_rect.setWidth(w);
}

void DisplayInfo::setHeight(int h)
{
    if (m_rect.height() == h)
        return;

    m_rect.setHeight(h);
}

void DisplayInfo::setResolution(QSize resolution)
{
    if (m_resolution == resolution)
        return;

    m_resolution = resolution;
}

void DisplayInfo::setResolution(int w, int h)
{
    setResolution(QSize(w, h));
}

void DisplayInfo::setResolutionWidth(int w)
{
    if (m_resolution.width() == w)
        return;

    m_resolution.setWidth(w);
}

void DisplayInfo::setResolutionHeight(int h)
{
    if (m_resolution.height() == h)
        return;

    m_resolution.setHeight(h);
}

void DisplayInfo::setWidth_mm(int w)
{
    if (m_size_mm.width() == w)
        return;

    m_size_mm.setWidth(w);
}

void DisplayInfo::setHeight_mm(int h)
{
    if (m_size_mm.height() == h)
        return;

    m_size_mm.setHeight(h);
}

void DisplayInfo::setSize_mm(QSize size)
{
    setSize_mm(size.width(), size.height());
}

void DisplayInfo::setSize_mm(int w, int h)
{
    setHeight_mm(h);
    setWidth_mm(w);
}

void DisplayInfo::swapWidthHeight()
{
    auto tmp = width();
    setWidth(height());
    setHeight(tmp);

    tmp = resolutionWidth();
    setResolutionWidth(resolutionHeight());
    setResolutionHeight(tmp);
}

QString DisplayInfo::name() const
{
    return m_name;
}

void DisplayInfo::setName(QString newName)
{
    if (newName.isEmpty() || newName == m_name)
        return;

    qDebug() << "change name display from " << m_name << " to " << newName;
    m_name = newName;
}

QString DisplayInfo::serial() const
{
    return m_serial;
}

void DisplayInfo::setSerial(QString newSerial)
{
    if (newSerial.isEmpty() || newSerial == m_serial)
        return;

    qDebug() << "change display serial from " << m_serial << " to " << newSerial;
    m_serial = newSerial;
}

QString DisplayInfo::detectSerial() const
{
    return m_detectSerial;
}

void DisplayInfo::setDetectSerial(QString newDSerial)
{
    if (newDSerial.isEmpty())
        return;

    qDebug() << "change display detect serial from " << m_detectSerial << " to " << newDSerial;
    m_detectSerial = newDSerial;
}

QString DisplayInfo::manufacture() const
{
    return m_manufacture;
}

void DisplayInfo::setManufacture(QString newManufacture)
{
    if (newManufacture.isEmpty() || newManufacture == m_manufacture)
        return;

    qDebug() << "change display manufactor from " << m_manufacture << " to " << newManufacture;
    m_manufacture = newManufacture;
}
