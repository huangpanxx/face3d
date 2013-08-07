#include "facelocatewidget.h"
#include "ui_facelocatewidget.h"
#include <QMessageBox>
#include "common.h"
#include "fiber.h"




FaceLocateWidget::FaceLocateWidget(QWidget *parent ,const StatModel::FaceLocator &locator,
                                   const face::TemplateFace &face,int size):
    QWidget(parent),
    ui(new Ui::FaceLocateWidget),
    m_face(face),
    m_locator(locator) {
    m_rectSize = 6;
    m_selectedPointIndex = -1;
    m_size = size;
    m_blankRate = 0.1;
    cv::Mat mface =  this->m_face.getFaceImage(20,m_ox,m_oy);
    m_templateFace = mat_to_image(mface);
    ui->setupUi(this);

    this->updateTemplateFace();
}

FaceLocateWidget::~FaceLocateWidget() {
    delete ui;
}

void FaceLocateWidget::updateTemplateFace() {
    QImage image(this->m_templateFace);

    float rate = this->m_size/fmax(image.width(),image.height());
    image = image.scaled(image.size()*rate);

    for(uint i=0;i<this->m_featurePoints.size();++i) {
        cv::Point2f point = this->m_face.xy(i);
        QColor color =  this->indexToColor(i);
        int width = 3;
        if(i==(uint)this->m_selectedPointIndex) {
            width = 5;
        }
        draw_rect(image,QPoint((point.x-m_ox)*rate,(point.y-m_oy)*rate),width*2,color,width);
    }


    image.scaled(this->m_image.size());
    this->ui->label->setPixmap(QPixmap::fromImage(image));
    this->ui->label->setFixedSize(this->m_image.size());
}

void FaceLocateWidget::locatePoints() {
    cv::Mat image = image_to_mat(this->m_image);
    std::vector<StatModel::ASMFitResult> fitResults = this->m_locator.fitAll(image);
    if(fitResults.size()==0) {
        QMessageBox::warning(this,tr("warning"),tr("No face was detected!"));
    }
    StatModel::ASMFitResult &result = fitResults.at(0);

    std::vector<cv::Point> points;
    result.toPointList(points);

    this->m_featurePoints.clear();
    this->m_featurePoints.reserve(points.size());
    for(uint i=0;i<points.size();++i) {
        const cv::Point &p = points[i];
        this->m_featurePoints.push_back(cv::Point2f(p.x,p.y));
    }

    this->m_rect  =  rectf_from_points(m_featurePoints);
    int blank = fmax(this->m_rect.width,this->m_rect.height) * m_blankRate;
    this->m_rect.x -= blank; this->m_rect.y -= blank;
    this->m_rect.width += blank*2; this->m_rect.height += blank*2;

    this->m_image = mat_to_image(image(this->m_rect));
    this->m_scale = std::min(m_size/this->m_image.width(),m_size/this->m_image.height());
    this->m_image = this->m_image.scaled(this->m_image.size()*m_scale);


    this->refreshPoints();
}

QColor FaceLocateWidget::indexToColor(int i) {
    if(this->m_featurePoints.size() == 0) return Qt::black;
    cv::Vec3f bgr =  fiber::theta2Color(float(i)/this->m_featurePoints.size()*CV_PI,1)*255;
    return QColor(bgr(2),bgr(1),bgr(0));
}

void FaceLocateWidget::refreshPoints() {
    if(this->m_featurePoints.size() == 0) { return; }

    QImage canvas(this->m_image);
    for(uint i=0;i<this->m_featurePoints.size();++i) {
        cv::Point point = m_featurePoints.at(i);
        QPoint src = QPoint(point.x,point.y);
        QPoint dst = convertToImage(src);
        QColor color = indexToColor(i);
        if(i == (uint)this->m_selectedPointIndex) {
            draw_rect(canvas,dst,m_rectSize,color,2);
        }else{
            draw_rect(canvas,dst,m_rectSize*1.6,color,4);
        }
    }
    this->ui->label_image->setPixmap(QPixmap::fromImage(canvas));
    this->ui->label_image->setFixedSize(this->m_image.size());

}

void FaceLocateWidget::loadImage(const QImage& image) {
    this->m_image = image;
    this->ui->label_image->setPixmap(QPixmap::fromImage(image));
    this->ui->label_image->setFixedSize(image.size());
    this->locatePoints();
    this->updateTemplateFace();
}

bool FaceLocateWidget::loadImage(const QString &filename) {
    QImage image;
    if(!image.load(filename))
        return false;
    this->loadImage(image);
    return true;
}


void FaceLocateWidget::on_label_image_mousePressed(QMouseEvent *event) {
    QPoint pos = this->convertFromImage(event->pos());
    for(uint i=0;i<this->m_featurePoints.size();++i) {
        cv::Point2f &p = this->m_featurePoints.at(i);
        if(distance(p.x-pos.x(),p.y-pos.y()) < m_rectSize/this->m_scale) {
            this->m_selectedPointIndex = i;
            this->refreshPoints();
            this->updateTemplateFace();
            break;
        }
    }
}

void FaceLocateWidget::on_label_image_mouseReleased(QMouseEvent *) {
    if(this->m_selectedPointIndex!=-1) {
        this->m_selectedPointIndex = -1;
        this->refreshPoints();
        this->updateTemplateFace();
    }
}

void FaceLocateWidget::on_label_image_mouseMoved(QMouseEvent *event) {
    if(this->m_featurePoints.size() == 0) return;
    if(this->m_selectedPointIndex >= 0 &&
            this->m_selectedPointIndex < (int)this->m_featurePoints.size()) {
         cv::Point2f &p = this->m_featurePoints.at(this->m_selectedPointIndex);
         QPoint pos = this->convertFromImage(event->pos());
         p.x = pos.x();
         p.y = pos.y();
         this->refreshPoints();
    }
}

QPoint FaceLocateWidget::convertToImage(const QPoint &point) {
    float x =(point.x() - this->m_rect.x)*this->m_scale;
    float y = (point.y() - this->m_rect.y)*this->m_scale;
    return QPoint(x,y);
}

QPoint FaceLocateWidget::convertFromImage(const QPoint &point) {
    float x = point.x()/this->m_scale+ this->m_rect.x;
    float y = point.y()/this->m_scale+this->m_rect.y;
    return QPoint(x,y);
}
