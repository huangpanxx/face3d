#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H


#include <QLabel>
#include <QImage>
#include <QList>
#include <QSize>


class CanvasWidget: public QLabel
{
    Q_OBJECT

public:
    const static QRgb BLANK = 0;
    explicit CanvasWidget(QWidget *parent = 0);
    ~CanvasWidget();
    bool loadImage(QString path);
    void loadImage(const QImage &image);
    QImage scribble() const { return this->_scribble; }
    QImage image() const { return this->_image; }
    QImage canvas() const { return this->_canvas; }
    void clear() { this->loadImage(this->image()); }
    void setColor(QColor color) { this->_color = color; }
    QColor color() const { return this->_color; }
    bool empty() const { return this->image().isNull(); }
    bool setStepBackLength(int length) {
        bool valid = length>=0;
        if(valid) this->_stepBackLength = length;
        return valid;
    }
    int stepBackLength() const { return this->_stepBackLength; }
    bool loadScribble(const QImage &scribble);
    bool loadScribble(const QString& filePath);
    QSize imageSize() const { return this->_image.size(); }
    int penWidth() const { return this->_penWidth; }
    bool setPenWidth(int n) {
        bool suc = n>0;
        if(suc) {
            this->_penWidth = n;
        }
        return suc;
    }

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    void _drawLine(QImage *image, const QPoint &from, const QPoint &to, QColor  color);
    void _saveState();
    void _popState();
    QImage _scribble;
    QImage _image;
    QImage _canvas;
    QColor _color;
    QPoint _lastPoint;
    Qt::MouseButton _pressedButton;
    QList<QImage> _canvasHistory;
    QList<QImage> _scribbleHistory;
    int _stepBackLength;
    int _penWidth;
};

#endif // CANVASWIDGET_H
