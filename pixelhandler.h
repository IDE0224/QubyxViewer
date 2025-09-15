/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#ifndef PIXELHANDLER_H
#define PIXELHANDLER_H

#include <QObject>
#include <QColor>

class QQuickItemGrabResult;
class PixelHandler : public QObject
{
    Q_OBJECT
public:
    explicit PixelHandler(QObject* parent = 0);

    Q_INVOKABLE QColor getColor(QQuickItemGrabResult* item, int x, int y);
};

#endif // PIXELHANDLER_H
