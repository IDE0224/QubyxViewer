/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef QMEDIAPLAYERWRAPPER_H
#define QMEDIAPLAYERWRAPPER_H

#include <QMediaPlayer>

class QMediaPlayerWrapper : public QMediaPlayer
{
    Q_OBJECT

public:
    QMediaPlayerWrapper(QObject* parent = 0);

    Q_INVOKABLE void setMediaFile(QString url);
    Q_INVOKABLE bool isPlaying();
};

#endif // QMEDIAPLAYERWRAPPER_H
