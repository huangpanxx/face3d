#ifndef SCRIBBLEWIDGET_H
#define SCRIBBLEWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class ScribbleWidget;
}

class ScribbleWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ScribbleWidget(QWidget *parent = 0);
    ~ScribbleWidget();

public:
    bool loadImage(QString path,bool tryLoadall = false);
    void loadImage(const QImage &image);
    bool loadScribble(const QImage &scribble);
    bool loadScribble(QString path);
    void loadHair(const QImage &image);
    void loadBody(const QImage &image);
    void loadFace(const QImage &image);
    void loadBackground(const QImage &image);
    bool setPenWidth(int n);
    int penWidth() const;
    
private slots:
    void on_pushButton_load_clicked();
    void on_radioButton_face_toggled(bool checked);
    void on_radioButton_blue_toggled(bool checked);
    void on_radioButton_body_toggled(bool checked);
    void on_radioButton_background_toggled(bool checked);
    void on_pushButton_clear_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_faceMatting_clicked();
    void on_pushButton_hairMatting_clicked();
    void on_pushButton_bodyMatting_clicked();
    void on_pushButton_backgroundMatting_clicked();
    void on_horizontalSlider_previewSize_valueChanged(int);

    void on_label_faceMatting_doubleClicked();
    void on_label_bodyMatting_doubleClicked();
    void on_label_backgroundMatting_doubleClicked();
    void on_label_hairMatting_doubleClicked();

private:
    void _enableWidgets();
    QImage _getMatting(QColor foreground) const;
    void _showPreview(QColor foreground, QString component);
    QImage _getScribble(QColor foreground) const;
    QSize _scaledSize() const;
    void _setMatting(QString component,const QImage& matting);
    void _clearMatting(QString component);
    void _showImageDialog(QImage image,const QString& component);
    QLabel* _getLabel(QString component);
    QLabel* _getInfoLabel(QString component);
    bool _loadComponent(QString component,QString path);
    bool _saveComponent(QString component,QString path);

    Ui::ScribbleWidget *ui;
};

#endif // SCRIBBLEWIDGET_H
