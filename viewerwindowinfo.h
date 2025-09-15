/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef VIEWERWINDOWINFO_H
#define VIEWERWINDOWINFO_H

#include <QObject>
#include <QRect>

class ViewerWindowInfo : public QObject
{
    Q_OBJECT
    int m_displayId;
    QRect m_geometry;
    bool m_visible;
    bool m_lutEnabled;
    QString m_name;
    bool m_mainDisplay;
    QString m_displayProfile;

    Q_PROPERTY(int displayId READ displayId WRITE setDisplayId NOTIFY displayIdChanged)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool lutEnabled READ lutEnabled WRITE setLutEnabled NOTIFY lutEnabledChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool isMainDisplay READ isMainDisplay WRITE setIsMainDisplay NOTIFY mainDisplayChanged)
    Q_PROPERTY(QString displayProfile READ displayProfile WRITE setDisplayProfile NOTIFY displayProfileChanged)

public:
    explicit ViewerWindowInfo(int dispId, bool isVisible, bool isMain);
    virtual ~ViewerWindowInfo();

    int displayId() const
    {
        return m_displayId;
    }

    QRect geometry() const
    {
        return m_geometry;
    }

    bool visible() const
    {
        return m_visible;
    }

    QString name() const
    {
        return m_name;
    }

    bool isMainDisplay() const
    {
        return m_mainDisplay;
    }

    bool lutEnabled() const
    {
        return m_lutEnabled;
    }

    QString displayProfile() const
    {
        return m_displayProfile;
    }

signals:
    void displayIdChanged(int newDisplayId);
    void geometryChanged(QRect newGeometry);
    void visibleChanged(bool newVisible);
    void lutEnabledChanged(bool newLutEnabled);
    void nameChanged(QString name);
    void mainDisplayChanged(bool isMainDisplay);
    void displayProfileChanged(QString displayProfile);

public slots:
    void setDisplayId(int displayId)
    {
        if (m_displayId == displayId)
            return;

        m_displayId = displayId;
        emit displayIdChanged(displayId);
    }

    void setGeometry(QRect geometry)
    {
        if (m_geometry == geometry)
            return;

        m_geometry = geometry;
        emit geometryChanged(geometry);
    }

    void setVisible(bool visible)
    {
        if (m_visible == visible)
            return;

        m_visible = visible;
        emit visibleChanged(visible);
    }

    void setLutEnabled(bool lutEnabled)
    {
        if (m_lutEnabled == lutEnabled)
            return;

        m_lutEnabled = lutEnabled;
        emit lutEnabledChanged(lutEnabled);
    }

    void setName(QString name)
    {
        if (m_name == name)
            return;

        m_name = name;
        emit nameChanged(name);
    }

    void setIsMainDisplay(bool mainDisplay)
    {
        if (m_mainDisplay == mainDisplay)
            return;

        m_mainDisplay = mainDisplay;
        emit mainDisplayChanged(mainDisplay);
    }
    
    void setDisplayProfile(QString displayProfile)
    {
        if (m_displayProfile == displayProfile)
            return;

        m_displayProfile = displayProfile;
        emit displayProfileChanged(displayProfile);
    }
};

#endif // VIEWERWINDOWINFO_H
