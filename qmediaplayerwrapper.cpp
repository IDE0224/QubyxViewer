/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "qmediaplayerwrapper.h"

QMediaPlayerWrapper::QMediaPlayerWrapper(QObject* parent) : QMediaPlayer(parent)
{
}

void QMediaPlayerWrapper::setMediaFile(QString url)
{
    // Qt 6: Use setSource() instead of setMedia()
    setSource(QUrl::fromLocalFile(url));
}

bool QMediaPlayerWrapper::isPlaying()
{
    // Qt 6: Use playbackState() instead of state()
    return playbackState() == QMediaPlayer::PlayingState;
}
