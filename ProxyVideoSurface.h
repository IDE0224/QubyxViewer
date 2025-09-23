/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef PROXYVIDEOSURFACE_H
#define PROXYVIDEOSURFACE_H

#include <QVideoSink>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QList>
#include <map>

class MediaSourceConnector;
class ProxyVideoSurface : public QVideoSink
{
    Q_OBJECT
public:
    ProxyVideoSurface(QObject* parent = nullptr);

    QList<QVideoFrameFormat::PixelFormat> supportedPixelFormats(QVideoFrame::HandleType handleType = QVideoFrame::NoHandle) const;
    bool present(const QVideoFrame& frame);

    virtual bool start(const QVideoFrameFormat& format);
    virtual void stop();

    Q_INVOKABLE MediaSourceConnector* getMediaSource(int id);

private:
    std::map<int, MediaSourceConnector*> connectors_;
};

#endif // PROXYVIDEOSURFACE_H
