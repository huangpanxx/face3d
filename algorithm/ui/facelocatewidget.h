#ifndef FACELOCATEWIDGET_H
#define FACELOCATEWIDGET_H

#include <QWidget>
#include "../align/asm.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "../face/templateface.h"

namespace Ui {
class FaceLocateWidget;
}

class FaceLocateWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FaceLocateWidget(QWidget *parent , const StatModel::FaceLocator &locator,
                              const face::TemplateFace &face, int size = 600);
    ~FaceLocateWidget();

    void loadImage(const QImage& image);
    bool loadImage(const QString &filename);


    const std::vector<cv::Point2f> & featurePoints() const {
        return this->m_featurePoints;
    }

private:
    void locatePoints();
    void refreshPoints();
    QPoint convertToImage(const QPoint &point);
    QPoint convertFromImage(const QPoint &point);
    void updateTemplateFace();
    QColor indexToColor(int i);

private slots:
    void on_label_image_mousePressed(QMouseEvent *event);
    void on_label_image_mouseReleased(QMouseEvent *);
    void on_label_image_mouseMoved(QMouseEvent *event);

private:
    Ui::FaceLocateWidget *ui;
    int m_rectSize;
    face::TemplateFace m_face;
    StatModel::FaceLocator m_locator;
    std::vector<cv::Point2f> m_featurePoints;
    QImage m_image;
    QImage m_templateFace;
    float m_ox,m_oy;
    int m_selectedPointIndex;
    cv::Rect m_rect;
    int m_size;
    float m_scale;
    float m_blankRate;
};

#endif // FACELOCATEWIDGET_H
