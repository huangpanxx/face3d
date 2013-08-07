#include "qtextend.h"

#include <QPainter>
#include "common.h"

void draw_rect(QImage &image,const QPoint &center,float radius,QColor color,int width) {
    QPainter *painter = new QPainter();
    QPen pen(color);
    pen.setWidth(width);
    painter->begin(&image);
    painter->setPen(pen);
    painter->drawRect(QRect(center.x()-radius/2,center.y()-radius/2,radius,radius));
    painter->end();
    delete painter;
}
