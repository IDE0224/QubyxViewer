/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef PROXYVIDEOSURFACE_H
#define PROXYVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QList>
#include <map>

class MediaSourceConnector;
class ProxyVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    ProxyVideoSurface(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                 QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool present(const QVideoFrame &frame);
    virtual bool start(const QVideoSurfaceFormat &format);
    virtual void stop();

    Q_INVOKABLE MediaSourceConnector* getMediaSource(int id);

private:
    std::map<int, MediaSourceConnector*> connectors_;
};

#endif // PROXYVIDEOSURFACE_H
