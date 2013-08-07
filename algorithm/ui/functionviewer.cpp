#include "functionviewer.h"
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>

Button::Button(QString text, void (*fn)(), QWidget *parent):
    QPushButton(text,parent) {
    _fn  = fn;
    this->connect(this,SIGNAL(clicked()),this,SLOT(_on_clicked()));
}

void Button::_on_clicked() {
    this->_fn();
}

FunctionViewer::FunctionViewer(QWidget *parent) :
    QWidget(parent)
{
    this->setLayout(new QVBoxLayout());
}

void FunctionViewer::regist(QString name, void(*fn)(void)) {
    Button *button = new Button(name,fn,this);
    this->layout()->addWidget(button);
}
