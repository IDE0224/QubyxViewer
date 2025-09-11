/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "MediaSourceConnector.h"
#include <QAbstractVideoSurface>
#include <QDebug>

MediaSourceConnector::MediaSourceConnector(QObject *parent) : QObject(parent), videoSurface_(nullptr)
{
}

void MediaSourceConnector::setVideoSurface(QAbstractVideoSurface *surface)
{
    videoSurface_ = surface;
    qDebug() << "MediaSourceConnector::setVideoSurface" << this << surface;
    if (surface)
        qDebug() << surface->supportedPixelFormats();
}

QAbstractVideoSurface *MediaSourceConnector::getVideoSurface()
{
    return videoSurface_;
}
