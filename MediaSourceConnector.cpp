/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "MediaSourceConnector.h"
#include <QVideoSink>
#include <QDebug>

MediaSourceConnector::MediaSourceConnector(QObject* parent) : QObject(parent), videoSurface_(nullptr)
{
}

void MediaSourceConnector::setVideoSurface(QVideoSink* surface)
{
    videoSurface_ = surface;
    qDebug() << "MediaSourceConnector::setVideoSurface" << this << surface;

    if (surface)
        qDebug() << "Video surface set successfully";
}

QVideoSink* MediaSourceConnector::getVideoSurface()
{
    return videoSurface_;
}
