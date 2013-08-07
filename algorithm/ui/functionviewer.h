#ifndef FUNCTIONVIEWER_H
#define FUNCTIONVIEWER_H

#include <QWidget>
#include <QMap>
#include <QPushButton>

class FunctionViewer : public QWidget
{
    Q_OBJECT
public:
    explicit FunctionViewer(QWidget *parent = 0);
    void regist(QString name, void(*fn)(void));
};


class Button : public QPushButton {
    Q_OBJECT

public:
    explicit Button(QString text,void(*fn)(void),QWidget *parent=0);

private:
    void(*_fn)(void);

private slots:
    void _on_clicked();
};

#endif // FUNCTIONVIEWER_H
