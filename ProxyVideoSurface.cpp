/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "ProxyVideoSurface.h"
#include "MediaSourceConnector.h"
#include <QDebug>

ProxyVideoSurface::ProxyVideoSurface(QObject* parent) : QVideoSink(parent)
{

}

QList<QVideoFrameFormat::PixelFormat> ProxyVideoSurface::supportedPixelFormats(QVideoFrame::HandleType handleType) const
{
    if (handleType == QVideoFrame::NoHandle)
    {
        return QList<QVideoFrameFormat::PixelFormat>()
            << QVideoFrameFormat::Format_YUV420P
            << QVideoFrameFormat::Format_YV12
            << QVideoFrameFormat::Format_NV12
            << QVideoFrameFormat::Format_NV21
            << QVideoFrameFormat::Format_RGBX8888
            << QVideoFrameFormat::Format_ARGB8888
            << QVideoFrameFormat::Format_BGRX8888
            << QVideoFrameFormat::Format_BGRA8888;
    }
    else
    {
        return QList<QVideoFrameFormat::PixelFormat>();
    }
}

bool ProxyVideoSurface::present(const QVideoFrame& frame)
{
    static bool firstCall = true;
    if (firstCall)
        qDebug() << "pixelFormat" << frame.pixelFormat();

    firstCall = false;

    for (auto connector : connectors_)
    {
        QVideoSink* surface = connector.second->getVideoSurface();
        if (surface)
            surface->setVideoFrame(frame);
    }

    return true;
}

bool ProxyVideoSurface::start(const QVideoFrameFormat& format)
{
    qDebug() << "ProxyVideoSurface::start";

    for (auto connector : connectors_)
    {
        QVideoSink* surface = connector.second->getVideoSurface();
        
        if (surface)
        {
            // In Qt 6, QVideoSink doesn't have a start method
            // The video sink is ready to receive frames via setVideoFrame()
            qDebug() << "Video sink ready for format:" << format.pixelFormat();
        }
    }

    return true;
}

void ProxyVideoSurface::stop()
{
    qDebug() << "ProxyVideoSurface::stop";
    
    for (auto connector : connectors_)
    {
        QVideoSink* surface = connector.second->getVideoSurface();

        if (surface)
        {
            // In Qt 6, QVideoSink doesn't have a stop method
            // Stop by setting an invalid frame
            surface->setVideoFrame(QVideoFrame());
        }
    }
}

MediaSourceConnector* ProxyVideoSurface::getMediaSource(int id)
{
    qDebug() << "ProxyVideoSurface::getMediaSource" << id;

    if (!connectors_.count(id))
        connectors_[id] = new MediaSourceConnector(this);

    return connectors_[id];
}
