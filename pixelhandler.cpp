/* 
 * Author: QUBYX Software Technologies LTD HK
 * Copyright: QUBYX Software Technologies LTD HK
 */

#include "pixelhandler.h"
#include <QImage>
#include <QQuickItemGrabResult>
#include <QPainter>
#include <QDebug>

PixelHandler::PixelHandler(QObject *parent) : QObject(parent)
{
}

QColor PixelHandler::getColor(QQuickItemGrabResult *item, int x, int y)
{
//    QQuickPaintedItem* paintedItem = dynamic_cast<QQuickPaintedItem*>(item);
//    QImage image(paintedItem->width(), paintedItem->height(), QImage::Format_ARGB32);
//    QPainter painter(&image);
//    paintedItem->paint(&painter);

    QImage image = item->image();
    qDebug() << image.size();

    //qDebug() << "pixel" << x << y << image.pixelColor(x, y);
    return image.pixelColor(x, y);
}
