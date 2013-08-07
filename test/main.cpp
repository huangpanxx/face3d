#include <QApplication>

#include "common.h"

#include "gl/glviewer.h"
#include "gl/mathead.h"
#include "gl/usfhead.h"

#include "fibertest.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <fstream>

#include "align/asm.h"
#include "ui/facelocatewidget.h"

#include "fiber.h"
#include "face/templateface.h"
#include "gl/dotobject.h"



void gl_test() {
    GLViewer *v = new GLViewer();
    v->addObject(new USFHead("/Users/snail/desktop/T0000.off"));
    v->addObject(new MatHead("/Users/snail/desktop/shape2.txt",
                             "/Users/snail/desktop/color2.txt"));
    v->updateGL();
    v->show();
}

void griddata_test() {

    cv::Mat shape = read_mat("/users/snail/desktop/shape2.txt");
    cv::Mat color = read_mat("/users/snail/desktop/color2.txt");

    std::vector<cv::Point2f> xy = points2f_from_mat(shape,0,1);
    std::vector<float> z = vector_from_mat<float>(color,2);
    cv::Rect_<float> rect = rectf_from_points(xy);

    cv::Mat grid = griddata(xy,z,rect.x,rect.y,rect.width,rect.height,0);
    cv::imshow("12",normalize(grid));
}

void align_test() {
    cv::Mat mat = cv::imread("/users/snail/desktop/image.png");
    StatModel::FaceLocator locator("/users/snail/desktop/face3d/data/muct76.model",
                                   "/users/snail/desktop/face3d/data/haarcascade_frontalface_alt.xml");
    vector<StatModel::ASMFitResult> results = locator.fitAll(mat);
    StatModel::ASMFitResult result = results[0];
    std::vector<cv::Point> points;
    result.toPointList(points);

    for(uint i=0;i<points.size();++i) {
        cv::Point p = points[i];
        cv::circle(mat,p,2,cv::Scalar(1,0,0));
    }
    cv::imshow("image",mat);
}



void init_resources() {
    Q_INIT_RESOURCE(resource);
}

int rd() {
    return random() % 100;
}

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    init_resources();

    face::TemplateFace tface("/users/snail/desktop/shape.txt",
                             "/users/snail/desktop/color.txt",
                             "/users/snail/desktop/index.txt");



    //    float x,y;
    //    cv::imshow("",face.getFaceImage(0.1,x,y));
    //    cv::waitKey();
    //    return 0;



    //    StatModel::FaceLocator locator("/users/snail/desktop/face3d/data/muct76.model",
    //                                   "/users/snail/desktop/face3d/data/haarcascade_frontalface_alt.xml");

    //    FaceLocateWidget w(0,locator,tface);
    //    w.loadImage("/users/snail/desktop/image.png");
    //    w.show();
    //    app.exec();
    //    const std::vector<cv::Point2f> &points  = w.featurePoints();
    //    cv::Mat faceFeature = mat_from_points2f(points);
    //    write_mat_to_text("/users/snail/desktop/features.txt",faceFeature);

    //    return 0;

    //    std::vector<cv::Point2f> pts;
    //    std::vector<float> vs;
    //    for(int i=0;i<10;++i) {
    //        pts.push_back(cv::Point2f(rd(),rd()));
    //        vs.push_back(rd());
    //    }

    //    Tps tps(pts,vs);

    //    qDebug()<<tps.interpolate(pts[0].x,pts[0].y)<<vs[0];

    //    return 0;

    cv::Mat fm = read_mat("/users/snail/desktop/features.txt");
    cv::Mat fs = read_mat("/users/snail/desktop/shape.txt");

    VEC(cv::Point2f) features = points2f_from_mat(fm);
    VEC(cv::Point3f) dface = tface.deform(features);
    VEC(cv::Point3f) oface = points3f_from_mat(fs);

    VEC(cv::Point2f) dxy;
    FOR_EACH(it,dface) {
        dxy.push_back(cv::Point2f(it->x,it->y));
    }

    cv::Rect_<float> rect = rectf_from_points(dxy);
    cv::imshow("red",griddata(dxy,tface.r(),rect.x,rect.y,rect.width,rect.height,0)/255.0);

    GLViewer *v = new GLViewer();

    v->addObject(new DotObject(dface));
    v->addObject(new DotObject(oface));

    v->updateGL();
    v->show();;
    return  app.exec();
}
