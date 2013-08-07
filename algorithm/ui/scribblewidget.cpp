#include "scribblewidget.h"
#include "ui_scribblewidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <matting.h>
#include <QHBoxLayout>
#include <qmath.h>
#include "share.h"

using namespace matting;

ScribbleWidget::ScribbleWidget(QWidget *parent) :
    QWidget(parent) ,
    ui(new Ui::ScribbleWidget)
{
    ui->setupUi(this);
    this->ui->canvas->setColor(Qt::red);
}

ScribbleWidget::~ScribbleWidget()
{
    delete ui;
}

QLabel* ScribbleWidget::_getLabel(QString component) {
    if(component == tr("Face"))  return this->ui->label_faceMatting;
    if(component == tr("Hair")) return this->ui->label_hairMatting;
    if(component == tr("Body")) return this->ui->label_bodyMatting;
    if(component == tr("Background")) return this->ui->label_backgroundMatting;
    throw "invalid component";
}

QLabel* ScribbleWidget::_getInfoLabel(QString component) {
    if(component == tr("Face"))  return this->ui->label_faceMattingLabel;
    if(component == tr("Hair")) return this->ui->label_hairMattingLabel;
    if(component == tr("Body")) return this->ui->label_bodyMattingLabel;
    if(component == tr("Background")) return this->ui->label_backgroundMattingLabel;
    throw "invalid component";
}

bool ScribbleWidget::loadImage(QString path, bool tryLoadall) {
    QImage image;
    if(!image.load(path)) return false;
    this->loadImage(image);
    if(tryLoadall) {
        int i = path.lastIndexOf(".");
        QString left = path.left(i);
        QString right = path.mid(i);
        this->loadScribble(left  + "_scribble" + right);
        this->_loadComponent(tr("Hair"),left  + "_hair" + right);
        this->_loadComponent(tr("Body"),left  + "_body" + right);
        this->_loadComponent(tr("Face"),left  + "_face" + right);
        this->_loadComponent(tr("Background"),left  + "_background" + right);
    }
    return true;
}

bool ScribbleWidget::_loadComponent(QString component,QString path) {
    QImage image;
    bool ok = image.load(path);
    if(ok) {
        this->_setMatting(component,image);
    }
    return ok;
}

void  ScribbleWidget::loadHair(const QImage &image) {
    this->_setMatting(tr("Hair"),image);
}

void  ScribbleWidget::loadBody(const QImage &image) {
    this->_setMatting(tr("Body"),image);
}

void  ScribbleWidget::loadFace(const QImage &image) {
    this->_setMatting(tr("Face"),image);
}

void  ScribbleWidget::loadBackground(const QImage &image) {
    this->_setMatting(tr("Background"),image);
}

void  ScribbleWidget::loadImage(const QImage &image) {

    this->ui->canvas->loadImage(image);
    QSize sz = this->ui->canvas->imageSize();
    this->ui->label_imageSize->setText(QObject::tr("Size  ") + QString("%1*%2").
                                       arg(sz.width()).arg(sz.height()));
    this->ui->horizontalSlider_previewSize
            ->setMaximum(sz.width()*sz.height());
    emit this->ui->horizontalSlider_previewSize->
            valueChanged(this->ui->horizontalSlider_previewSize->value());
    this->_enableWidgets();
    //fix
    this->adjustSize();
    this->setFixedSize(this->size());

    //clear
    this->_clearMatting(tr("Face"));
    this->_clearMatting(tr("Background"));
    this->_clearMatting(tr("Hair"));
    this->_clearMatting(tr("Body"));

    //disable finish button
    this->ui->pushButton_finish->setEnabled(false);

}

void ScribbleWidget::_clearMatting(QString component) {
    this->_getLabel(component)->clear();
    this->_getInfoLabel(component)->setText(component);
}

bool  ScribbleWidget::loadScribble(const QImage &scribble) {
    return this->ui->canvas->loadScribble(scribble);
}

bool ScribbleWidget::loadScribble(QString path) {
    return this->ui->canvas->loadScribble(path);
}


void ScribbleWidget::_enableWidgets()  {
    this->ui->groupBox_components->setEnabled(true);
    this->ui->pushButton_clear->setEnabled(true);
    this->ui->groupBox_preview->setEnabled(true);
    this->ui->pushButton_save->setEnabled(true);
    this->ui->groupBox_image->setEnabled(true);
}

void ScribbleWidget::on_pushButton_load_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                this, tr("Select a Picture"),"~",tr("Images (*.png *.jpg *.bmp)"));
    if(!filePath.isEmpty()) {
        if(!this->loadImage(filePath,true)) {
            QMessageBox::warning(this,QObject::tr("error"),
                                 QObject::tr("Load image failed..."));
        }
    }
}

void ScribbleWidget::on_radioButton_face_toggled(bool checked)
{
    if(checked) this->ui->canvas->setColor(Qt::red);
}

void ScribbleWidget::on_radioButton_blue_toggled(bool checked)
{
    if(checked) this->ui->canvas->setColor(Qt::blue);
}

void ScribbleWidget::on_radioButton_body_toggled(bool checked)
{
    if(checked) this->ui->canvas->setColor(Qt::green);
}

void ScribbleWidget::on_radioButton_background_toggled(bool checked)
{
    if(checked) this->ui->canvas->setColor(Qt::black);
}

void ScribbleWidget::on_pushButton_clear_clicked()
{
    this->ui->canvas->clear();
}

void ScribbleWidget::on_pushButton_save_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
                this, tr("Save"),"~",tr("Images (*.png *.jpg *.bmp)"));
    if(!filePath.isEmpty()) {
        int i = filePath.lastIndexOf(".");
        QString left = filePath.left(i);
        QString right = filePath.mid(i);
        this->ui->canvas->image().save(filePath);
        this->ui->canvas->scribble().save(left + "_scribble" + right);
        this->_saveComponent(tr("Hair"),left+"_hair" +right);
        this->_saveComponent(tr("Body"),left+"_body"+right);
        this->_saveComponent(tr("Background"),left+"_background"+right);
        this->_saveComponent(tr("Face"),left+"_face"+right);
    }
}

bool ScribbleWidget::_saveComponent(QString component,QString path) {
    QLabel *label = this->_getLabel(component);
    bool ok = label->pixmap() && !label->pixmap()->isNull();
    if(ok){
        label->pixmap()->save(path);
    }
    return ok;
}

void ScribbleWidget::on_pushButton_faceMatting_clicked()
{
    this->_showPreview(Qt::red,tr("Face"));
}

void ScribbleWidget::on_pushButton_hairMatting_clicked()
{
    this->_showPreview(Qt::blue,tr("Hair"));
}

void ScribbleWidget::on_pushButton_bodyMatting_clicked()
{
    this->_showPreview(Qt::green,tr("Body"));
}

void ScribbleWidget::on_pushButton_backgroundMatting_clicked()
{
    this->_showPreview(Qt::black,tr("Background"));
}

QSize ScribbleWidget::_scaledSize() const {
    QSize size = this->ui->canvas->imageSize();
    float rate = sqrt((float)this->ui->horizontalSlider_previewSize->value() /
                      (size.width()*size.height()));
    int w = size.width()*rate, h = size.height()*rate;
    return QSize(w,h);
}

void ScribbleWidget::_showPreview(QColor foreground, QString component)  {

    //prepare trimap
    QImage scribble = this->_getScribble(foreground);
    QImage image = this->ui->canvas->image();

    //scale
    QSize sz = this->_scaledSize();
    scribble = scribble.scaled(sz.width(),sz.height(),  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    image = image.scaled(sz.width(),sz.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    //matting
    Matting matting = getMatting(image,scribble,qRgb(255,0,0),qRgb(0,255,0));

    //set to matting label
    this->_setMatting(component,matting.forground);

    //pop up dialog
    this->_showImageDialog(matting.forground,component);

}

void ScribbleWidget::_setMatting(QString component, const QImage& matting) {

    QLabel* label = this->_getLabel(component);
    QLabel* infoLabel = this->_getInfoLabel(component);
    label->setPixmap(QPixmap::fromImage(matting));
    infoLabel->setText(QString("%1 (%2*%3)")
                       .arg(component)
                       .arg(matting.width())
                       .arg(matting.height()));

    bool ok = true;
    QString ccs[4] = {tr("Face"),tr("Hair"),tr("Background"),tr("Body")};
    for(int i=0;i<4 && ok;++i) {
        QLabel *lb = this->_getLabel(ccs[i]);
        if(lb->pixmap() && !lb->pixmap()->isNull())
            continue;
        else
            ok = false;
    }
    if(ok)
        this->ui->pushButton_finish->setEnabled(true);
}

QImage ScribbleWidget::_getScribble(QColor foreground) const {
    QImage scribble = this->ui->canvas->scribble();
    for(int x=0;x<scribble.width();++x) {
        for(int y=0;y<scribble.height();++y) {
            QRgb color = scribble.pixel(x,y);
            if(color == CanvasWidget::BLANK) continue;
            if(color == foreground.rgba()) scribble.setPixel(x,y,qRgb(255,0,0));
            else scribble.setPixel(x,y,qRgb(0,255,0));
        }
    }
    return scribble;
}

void ScribbleWidget::on_horizontalSlider_previewSize_valueChanged(int )
{
    const QSize size = this->_scaledSize();
    QString msg = QString("%1*%2").arg(size.width()).arg(size.height());
    if(size.width()*size.height() > 40000) {
        msg += tr("(slow)");
    }
    this->ui->label_previewSize->setText(msg);
}

void ScribbleWidget::on_label_faceMatting_doubleClicked() {
    const QPixmap *pixmap = this->ui->label_faceMatting->pixmap();
    if(pixmap && !pixmap->isNull()) {
        _showImageDialog(pixmap->toImage(),tr("Face"));
    }
}

void ScribbleWidget::on_label_bodyMatting_doubleClicked() {
    const QPixmap *pixmap = this->ui->label_bodyMatting->pixmap();
    if(pixmap && !pixmap->isNull()) {
        _showImageDialog(pixmap->toImage(),tr("Body"));
    }
}

void ScribbleWidget::on_label_backgroundMatting_doubleClicked() {
    const QPixmap *pixmap = this->ui->label_backgroundMatting->pixmap();
    if(pixmap && !pixmap->isNull()) {
        _showImageDialog(pixmap->toImage(),tr("Background"));
    }
}

void ScribbleWidget::on_label_hairMatting_doubleClicked() {
    const QPixmap *pixmap = this->ui->label_hairMatting->pixmap();
    if(pixmap && !pixmap->isNull()) {
        _showImageDialog(pixmap->toImage(),tr("Hair"));
    }
}

void ScribbleWidget::_showImageDialog(QImage image,const QString& component) {
    QLabel *label_matting = new QLabel(this);
    QLabel *label_alpha = new QLabel(this);

    label_matting->setPixmap(QPixmap::fromImage(image));
    label_alpha->setPixmap(QPixmap::fromImage(image.alphaChannel()));

    QDialog *dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::Tool); //for osx
    dialog->setLayout(new QHBoxLayout());


    dialog->layout()->addWidget(label_matting);
    dialog->layout()->addWidget(label_alpha);

    dialog->adjustSize();
    dialog->setFixedSize(dialog->size());
    QString title = QString("%1 %2*%3").arg(component).arg(image.width()).arg(image.height());
    dialog->setWindowTitle(title);
    dialog->exec();
}

bool ScribbleWidget::setPenWidth(int n) {
    return this->ui->canvas->setPenWidth(n);
}

int ScribbleWidget::penWidth() const {
    return this->ui->canvas->penWidth();
}
