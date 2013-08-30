#include "../algorithm/gl/glviewer.h"
#include "../algorithm/gl/dotobject.h"
#include "../algorithm/common.h"
#include <QGuiApplication>
#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include "../algorithm/ui/scribblewidget.h"


cv::Mat relief(const QImage &image, float high = 4) {

    cv::Mat alpha = image_to_mat(image.alphaChannel(),false);
    alpha.convertTo(alpha,CV_32FC3);
    alpha = alpha/255.0;
    //cv::GaussianBlur(alpha,alpha,cv::Size(5,5),3);
    cv::cvtColor(alpha,alpha,CV_BGR2GRAY);


    cv::Mat detail = image_to_mat(image,false);
    detail.convertTo(detail,CV_32FC3);
    detail = detail/255.0;
    cv::cvtColor(detail,detail,CV_BGR2GRAY);
    detail =  detail.mul(alpha);

    // cv::Mat blured;
   // cv::GaussianBlur(detail,blured,cv::Size(3,3),1);

   // cv::sqrt(alpha,alpha);

    return alpha * high;//(alpha +  detail*1.2) * high;
}

int main(int argc,char **argv) {
    QApplication app(argc,argv);

//    ScribbleWidget *w = new ScribbleWidget();
//    w->show();
//    return app.exec();

    GLViewer *viewer = new GLViewer();

    QImage imageface("../relief_face.png");
    QImage imagehair("../relief_hair.png");
    QImage imagebody("../relief_body.png");

    cv::Mat mrelief = relief(imageface) + relief(imagehair) + relief(imagebody);

    viewer->addObject(new DotObject(mrelief));

    viewer->show();

    return app.exec();
}
