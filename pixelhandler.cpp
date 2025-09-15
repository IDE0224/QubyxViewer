/*
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "pixelhandler.h"
#include <QImage>
#include <QQuickItemGrabResult>
#include <QPainter>
#include <QDebug>

PixelHandler::PixelHandler(QObject* parent) : QObject(parent)
{
}

QColor PixelHandler::getColor(QQuickItemGrabResult* item, int x, int y)
{
    QImage image = item->image();
    qDebug() << image.size();

    return image.pixelColor(x, y);
}
