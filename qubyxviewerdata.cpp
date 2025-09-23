/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "qubyxviewerdata.h"
#include "libs/CurrentDisplays.h"
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <set>
#include "LutGenerator.h"
#include "MediaSourceConnector.h"

const QString DISPLAYS = "displays";
const QString DISPLAY_OSID = "display_osid";
const QString DISPLAY_ID = "display_id";
const QString VISIBLE = "visible";
const QString MAIN_DISPLAY_ID = "main_display_id";


QubyxViewerData::QubyxViewerData(QObject* parent) : QObject(parent)
{
    qRegisterMetaType<MediaSourceConnector*>("MediaSourceConnector*");

    CurrentDisplays::NonCachedRefresh();
    readDisplayIds();

    qDebug() << "displays:" << CurrentDisplays::DisplayCount();

    for (int i = 0;i < CurrentDisplays::DisplayCount();i++)
    {
        int id = CurrentDisplays::DisplayId(i);
        model_.append(new ViewerWindowInfo(CurrentDisplays::DisplayId(i), visible_[id], mainDisplayId_ == id));
    }

    lutGenerator_ = new LutGenerator();
    for (int i = 0;i < model_.length();i++)
    {
        ViewerWindowInfo* item = dynamic_cast<ViewerWindowInfo*>(model_[i]);
        lutGenerator_->setDisplayProfile(item->displayId(), item->displayProfile());
        qDebug() << item->displayId() << item->displayProfile();
    }

    mediaPlayer_.setVideoOutput(&proxyVideoSurface_);

    qml_.addImageProvider(QLatin1String("LUT"), lutGenerator_);

    qml_.rootContext()->setContextProperty("displayInfo", QVariant::fromValue(model_));
    qml_.rootContext()->setContextProperty("fileReader", &fileReader_);
    qml_.rootContext()->setContextProperty("pixelHandler", &pixelHandler_);

    qml_.rootContext()->setContextProperty("mediaPlayer", &mediaPlayer_);
    qml_.rootContext()->setContextProperty("proxyVideoSurface", &proxyVideoSurface_);

    qml_.load(QUrl("qrc:/qml/main.qml"));

    QObject* item = qml_.rootObjects()[0];
    connect(item, SIGNAL(signalOpenWorkingProfile(QString)), this, SLOT(slotOpenWorkingProfile(QString)));

    QString profilePath = fileReader_.lastProfilePath();
    qDebug() << "profilePath" << profilePath;
    if (!profilePath.isEmpty())
        slotOpenWorkingProfile(profilePath);
}

QubyxViewerData::~QubyxViewerData()
{
    writeDisplayIds();
}

void QubyxViewerData::readDisplayIds()
{
    mainDisplayId_ = settings_.value(MAIN_DISPLAY_ID).toInt();

    int cnt = settings_.beginReadArray(DISPLAYS);
    int maxId = 0;
    for (int i = 0; i < cnt; i++)
    {
        settings_.setArrayIndex(i);
        QString osid = settings_.value(DISPLAY_OSID).toString();
        int id = settings_.value(DISPLAY_ID).toInt();
        bool vis = settings_.value(VISIBLE).toBool();

        displayIds_[osid] = id;
        visible_[id] = vis;
        maxId = std::max(maxId, id);
    }
    settings_.endArray();

    int cntVisible = 0;
    for (int i = 0; i < CurrentDisplays::DisplayCount(); i++)
    {
        const DisplayInfo disp = CurrentDisplays::Display(i);
        if (!displayIds_.count(disp.OSId))
        {
            displayIds_[disp.OSId] = ++maxId;
            visible_[maxId] = true;
        }

        CurrentDisplays::SetDisplayId(i, displayIds_[disp.OSId]);
        qDebug() << i << displayIds_[disp.OSId] << disp.OSId;

        if (visible_[displayIds_[disp.OSId]])
            cntVisible++;
    }

    if (!cntVisible)
    {
        for (int i = 0; i < CurrentDisplays::DisplayCount(); i++)
            visible_[CurrentDisplays::DisplayId(i)] = true;
    }

    std::set<int> activeDisplays;
    for (int i = 0; i < CurrentDisplays::DisplayCount(); i++)
        if (visible_[CurrentDisplays::DisplayId(i)])
            activeDisplays.insert(CurrentDisplays::DisplayId(i));

    if (!activeDisplays.count(mainDisplayId_))
    {
        // try to find new main display
        for (int i = 0; i < CurrentDisplays::DisplayCount(); i++)
        {
            const DisplayInfo disp = CurrentDisplays::Display(i);
            if (!disp.top() && !disp.left()) // main display has (0, 0) coordinates
            {
                mainDisplayId_ = CurrentDisplays::DisplayId(i);
                break;
            }
        }

        if (!activeDisplays.count(mainDisplayId_))
            mainDisplayId_ = *activeDisplays.begin();
    }

}

void QubyxViewerData::writeDisplayIds()
{
    for (int i = 0; i < model_.size(); i++)
    {
        ViewerWindowInfo* info = dynamic_cast<ViewerWindowInfo*>(model_[i]);
        visible_[info->displayId()] = info->visible();
        if (info->isMainDisplay())
            mainDisplayId_ = info->displayId();
    }
    settings_.setValue(MAIN_DISPLAY_ID, mainDisplayId_);

    settings_.beginWriteArray(DISPLAYS, displayIds_.size());
    auto itr = displayIds_.begin();
    for (int i = 0; i < displayIds_.size(); i++, itr++)
    {
        settings_.setArrayIndex(i);
        settings_.setValue(DISPLAY_OSID, itr->first);
        settings_.setValue(DISPLAY_ID, itr->second);
        settings_.setValue(VISIBLE, visible_[itr->second]);
    }
    settings_.endArray();
}

void QubyxViewerData::slotQuit()
{
    QCoreApplication::quit();
}

void QubyxViewerData::slotOpenWorkingProfile(QString filePath)
{
    filePath = filePath.replace("file:///", "");
    filePath = QDir::toNativeSeparators(filePath);

    qDebug() << "QubyxViewerData::slotOpenWorkingProfile" << filePath;
    lutGenerator_->setWorkingProfile(filePath);

    QMetaObject::invokeMethod(qml_.rootObjects()[0], "reload3dLut");
}
