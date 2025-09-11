/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "qmediaplayerwrapper.h"

QMediaPlayerWrapper::QMediaPlayerWrapper(QObject *parent) : QMediaPlayer(parent)
{
}

void QMediaPlayerWrapper::setMediaFile(QString url)
{
    QMediaPlayer::setMedia(QUrl::fromLocalFile(url));
}

bool QMediaPlayerWrapper::isPlaying()
{
    return state() == QMediaPlayer::PlayingState;
}
