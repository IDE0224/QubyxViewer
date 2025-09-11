/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef MEDIASOURCECONNECTOR_H
#define MEDIASOURCECONNECTOR_H

#include <QObject>

class QAbstractVideoSurface;
class MediaSourceConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ getVideoSurface WRITE setVideoSurface )
public:
    explicit MediaSourceConnector(QObject *parent = 0);

    Q_INVOKABLE void setVideoSurface(QAbstractVideoSurface* surface);
    Q_INVOKABLE QAbstractVideoSurface* getVideoSurface();

private:
    QAbstractVideoSurface *videoSurface_;
};

#endif // MEDIASOURCECONNECTOR_H
