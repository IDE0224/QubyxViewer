/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef QUBYXVIEWERDATA_H
#define QUBYXVIEWERDATA_H

#include <QObject>
#include <QList>
#include <QQmlApplicationEngine>
#include <map>
#include <QSettings>
#include "QMediaPlayerWrapper.h"
#include "viewerwindowinfo.h"
#include "filereader.h"
#include "pixelhandler.h"
#include "ProxyVideoSurface.h"
class MediaSourceConnector;
Q_DECLARE_OPAQUE_POINTER(MediaSourceConnector*)

class LutGenerator;
class QubyxViewerData : public QObject
{
    Q_OBJECT
public:
    explicit QubyxViewerData(QObject* parent = 0);
    virtual ~QubyxViewerData();

private:
    QList<QObject*> model_;
    QQmlApplicationEngine qml_;
    LutGenerator* lutGenerator_;

    FileReader fileReader_;
    PixelHandler pixelHandler_;
    std::map<QString, int> displayIds_;
    std::map<int, bool> visible_;
    int mainDisplayId_;
    QSettings settings_;

    QMediaPlayerWrapper mediaPlayer_;
    ProxyVideoSurface proxyVideoSurface_;

    void readDisplayIds();
    void writeDisplayIds();

signals:

public slots:
    void slotQuit();
    void slotOpenWorkingProfile(QString filePath);
};

#endif // QUBYXVIEWERDATA_H
