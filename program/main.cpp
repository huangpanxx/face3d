#include <QApplication>
#include <algorithm/ui/scribblewidget.h>
#include <algorithm/common.h>
#include <stasm/stasm_lib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <exception>
#include <algorithm/face/templateface.h>
#include <algorithm/gl/glviewer.h>
#include <algorithm/gl/dotobject.h>
#include <algorithm/ui/facelocatewidget.h>
#include "align3d.h"
#include <QString>

class FaceLocator {
public:
    FaceLocator(const std::string &datadir);
    FaceLocator(const FaceLocator& locator);
    std::vector<cv::Point2f> fit(const cv::Mat &image,const char* image_path = "unknown");
private:
    std::string m_datadir;
};
FaceLocator::FaceLocator(const std::string &datadir) {
    this->m_datadir = datadir;
}

FaceLocator::FaceLocator(const FaceLocator &locator) {
    this->m_datadir = locator.m_datadir;
}
std::vector<cv::Point2f> FaceLocator::fit(const cv::Mat &image, const char* image_path) {
    cv::Mat src = image.clone();
    src = to_cv8u_image(src);

    int foundface;
    float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
    if (!stasm_search_single(&foundface, landmarks,
                                (const char*)src.data, src.cols, src.rows, image_path, this->m_datadir.c_str()))
    {
        std::string error = std::string("Error in stasm_search_single: ") + stasm_lasterr();
        qDebug()<<QString::fromStdString(error);
        throw error;
    }
    std::vector<cv::Point2f> points;
    if(foundface) {
        points.reserve(stasm_NLANDMARKS);
        stasm_force_points_into_image(landmarks, src.cols, src.rows);
        for (int i = 0; i < stasm_NLANDMARKS; i++) {
            points.push_back(cv::Point2f(cvRound(landmarks[i*2+0]), cvRound(landmarks[i*2+1])));
        }
    } else {
        qDebug()<<"face not found!(FaceLocator::fit)";
    }
    return points;
}


int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    QWidget *w =  new ScribbleWidget();
    w->show();
    app.exec();

    //初始化
    face::TemplateFace tface("./data/face/",0.0001,10);


    //载入图像
    cv::Mat image = cv::imread("../test.png");
    float x,y;
    cv::Mat timage= tface.getFaceImage(100,x,y);

    //定位
    FaceLocator locator("./data/stasm/");
    VEC(cv::Point2f) pts = locator.fit(image),
            tpts = locator.fit(timage);

    pts = VEC(cv::Point2f) (pts.begin()  + 16,pts.end());
    tpts = VEC(cv::Point2f)(tpts.begin() + 16,tpts.end());

    //得到特征点
    tpts = translate(tpts,x,y);
    VEC(cv::Point3f) tpts3d = tface.interpolate(tpts);


    //拟合
    cv::Mat A = align3d(tpts3d,pts);

    cv::Mat im = image;

    tpts3d = A*tpts3d;

    for(auto &it : tpts3d) {
        cv::circle(im,cv::Point(it.x,it.y),1,cv::Scalar(255,0,0));
    }
    for(auto &it : pts) {
        cv::circle(im,cv::Point(it.x,it.y),1,cv::Scalar(0,255,0));
    }

    cv::imshow("win",im);




    GLViewer viewer;
    viewer.addObject(new DotObject(A*centric_points(tface.xyzs(true)),true,true));
   // viewer.addObject(new DotObject(tface.xyzs(true),true,true));
  //  viewer.addObject(new DotObject(tface.deform(tpts,0,0,pts,0.01),true,true));
    viewer.show();

    return app.exec();
}
