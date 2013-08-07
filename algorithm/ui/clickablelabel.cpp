#include "clickablelabel.h"

ClickableLabel::ClickableLabel( QWidget * parent )
    : QLabel(parent) {
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mouseDoubleClickEvent ( QMouseEvent *  e)
{
    QLabel::mouseDoubleClickEvent(e);
    emit doubleClicked();
    emit doubleClicked(e);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event) {
    QLabel::mousePressEvent(event);
    emit mousePressed();
    emit mousePressed(event);
}

void ClickableLabel::mouseMoveEvent(QMouseEvent *event) {
    QLabel::mouseMoveEvent(event);
    emit mouseMoved(event);
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
    QLabel::mouseReleaseEvent(event);
    emit mouseReleased(event);
}
