#include "canvaswidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

CanvasWidget::CanvasWidget(QWidget *parent) :
    QLabel(parent) , _penWidth(10)
{
    this->setText(tr("load an image first..."));
    this->_stepBackLength = 10;
}

CanvasWidget::~CanvasWidget()
{
}

bool CanvasWidget::loadImage(QString path) {
    QImage image;
    bool suc = image.load(path);
    if(suc) {
        this->loadImage(image);
    }
    return suc;
}

void CanvasWidget::loadImage(const QImage& image) {
    this->_canvas = this->_image =
            image.convertToFormat(QImage::Format_ARGB32); //ensure alpha channel

    this->_scribble = QImage(image.size(),QImage::Format_ARGB32);
    this->_scribble.fill(CanvasWidget::BLANK);
    this->setPixmap(QPixmap::fromImage(this->_canvas));
    this->setFixedSize(image.size());
    this->_canvasHistory.clear();
    this->_scribbleHistory.clear();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if(!this->empty() &&  _pressedButton == Qt::LeftButton) {
        QPoint pos = event->pos();
        if(!this->_lastPoint.isNull()) {
            this->_drawLine(&this->_canvas, this->_lastPoint, pos, this->color());
            this->_drawLine(&this->_scribble, this->_lastPoint, pos, this->color());
            this->setPixmap(QPixmap::fromImage(this->_canvas));
        }
        this->_lastPoint = pos;
    }
}

void CanvasWidget::_drawLine(QImage *image, const QPoint &from, const QPoint &to,  QColor  color)
{
    QPen pen(color);
    pen.setWidth(this->penWidth());

    QPainter *painter = new QPainter();
    painter->begin(image);
    painter->setPen(pen);
    painter->drawLine(from,to);
    painter->end();

    delete painter;
}

void CanvasWidget::_saveState() {
    //save to history
    if(this->stepBackLength() > 0) {
        this->_canvasHistory.push_back(this->_canvas);
        this->_scribbleHistory.push_back(this->_scribble);
        if(this->_canvasHistory.size() > this->stepBackLength()) {
            this->_canvasHistory.pop_front();
            this->_scribbleHistory.pop_front();
        }
    }
}

void CanvasWidget::_popState() {
    //recover
    if(this->_canvasHistory.size() > 0) {
        this->_canvas = this->_canvasHistory.last();
        this->_scribble = this->_scribbleHistory.last();
        this->setPixmap(QPixmap::fromImage(this->_canvas));
        this->_canvasHistory.pop_back();
        this->_scribbleHistory.pop_back();
     }
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if(!this->empty()) {
        if(event->type() == QEvent::MouseButtonPress) { //mouse down
            this->_lastPoint = QPoint();
        }

        this->_pressedButton = event->button();

        if(event->button() == Qt::RightButton && event->type() == QEvent::MouseButtonPress) { //right click
           this->_popState();
        }

        if(event->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress) { //left click
            this->_saveState();
        }
    }
}

bool CanvasWidget::loadScribble(const QImage &scribble) {
    if(scribble.size() != this->_image.size())
        return false;

    this->clear();
    this->_saveState();

    for(int x=0;x<scribble.width();++x) {
        for(int y=0;y<scribble.height();++y) {
            QRgb color = scribble.pixel(x,y);
            if(color != CanvasWidget::BLANK) {
                this->_canvas.setPixel(x,y,color);
            }
        }
    }
    this->_scribble = scribble.convertToFormat(QImage::Format_ARGB32);
    this->setPixmap(QPixmap::fromImage(this->_canvas));
    return true;
}

bool CanvasWidget::loadScribble(const QString& filePath) {
    QImage image;
    if(!image.load(filePath))
        return false;
    return this->loadScribble(image);
}
