/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "ProxyVideoSurface.h"
#include "MediaSourceConnector.h"
#include <QDebug>

ProxyVideoSurface::ProxyVideoSurface(QObject *parent) : QAbstractVideoSurface(parent)
{

}

QList<QVideoFrame::PixelFormat> ProxyVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle)
    {
        return QList<QVideoFrame::PixelFormat>()
             << QVideoFrame::Format_YUV420P
             << QVideoFrame::Format_YV12
             << QVideoFrame::Format_NV12
             << QVideoFrame::Format_NV21
             << QVideoFrame::Format_RGB32
             << QVideoFrame::Format_ARGB32
             << QVideoFrame::Format_BGR32
             << QVideoFrame::Format_BGRA32
             << QVideoFrame::Format_RGB565;


//        return QList<QVideoFrame::PixelFormat>()
//             << QVideoFrame::Format_RGB32
//             << QVideoFrame::Format_ARGB32
//             << QVideoFrame::Format_ARGB32_Premultiplied
//             << QVideoFrame::Format_RGB565
//             << QVideoFrame::Format_RGB555
//             << QVideoFrame::Format_RGB24
//             << QVideoFrame::Format_BGRA32
//             << QVideoFrame::Format_BGRA32_Premultiplied
//             << QVideoFrame::Format_BGR32
//             << QVideoFrame::Format_BGR24
//             << QVideoFrame::Format_BGR565
//             << QVideoFrame::Format_BGR555;
    }
    else
    {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool ProxyVideoSurface::present(const QVideoFrame &frame)
{
    static bool firstCall = true;
    if (firstCall)
        qDebug() << "pixelFormat" << frame.pixelFormat();
    firstCall = false;

    for (auto connector : connectors_)
    {
        QAbstractVideoSurface *surface = connector.second->getVideoSurface();
        if (surface)
            surface->present(frame);
    }

    return true;
}

bool ProxyVideoSurface::start(const QVideoSurfaceFormat &format)
{
    qDebug() << "ProxyVideoSurface::start";
    for (auto connector : connectors_)
    {
        QAbstractVideoSurface *surface = connector.second->getVideoSurface();
        if (surface)
            surface->start(format);
    }

    return QAbstractVideoSurface::start(format);
}

void ProxyVideoSurface::stop()
{
    qDebug() << "ProxyVideoSurface::stop";
    for (auto connector : connectors_)
    {
        QAbstractVideoSurface *surface = connector.second->getVideoSurface();
        if (surface)
            surface->stop();
    }
    QAbstractVideoSurface::stop();
}

MediaSourceConnector *ProxyVideoSurface::getMediaSource(int id)
{
    qDebug() << "ProxyVideoSurface::getMediaSource" << id;

    if (!connectors_.count(id))
        connectors_[id] = new MediaSourceConnector(this);

    return connectors_[id];
}
