#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{

    Q_OBJECT

public:
    explicit ClickableLabel( QWidget * parent = 0 );
    ~ClickableLabel();

signals:
    void mousePressed();
    void doubleClicked();
    void mousePressed(QMouseEvent *event);
    void doubleClicked(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // CLICKABLELABEL_H
