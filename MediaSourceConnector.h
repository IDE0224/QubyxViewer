/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef MEDIASOURCECONNECTOR_H
#define MEDIASOURCECONNECTOR_H

#include <QObject>

class QVideoSink;
Q_DECLARE_OPAQUE_POINTER(QVideoSink*)
class MediaSourceConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSurface READ getVideoSurface WRITE setVideoSurface)
    
public:
    explicit MediaSourceConnector(QObject* parent = 0);

    Q_INVOKABLE void setVideoSurface(QVideoSink* surface);
    Q_INVOKABLE QVideoSink* getVideoSurface();

private:
    QVideoSink* videoSurface_;
};

#endif // MEDIASOURCECONNECTOR_H
