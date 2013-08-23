
//#include <QApplication>

//#include "common.h"

//#include "gl/glviewer.h"
//#include "gl/mathead.h"
//#include "gl/usfhead.h"

//#include "fibertest.h"

//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>


//#include <fstream>

//#include "align/asm.h"
//#include "ui/facelocatewidget.h"

//#include "fiber.h"
//#include "face/templateface.h"
//#include "gl/dotobject.h"
//#include "matting.h"
//#include "ui/scribblewidget.h"

//#include <fstream>
//#include <stdlib.h>

//using namespace std;

//void gl_test() {
//    GLViewer *v = new GLViewer();
//    v->addObject(new USFHead("/Users/snail/desktop/T0000.off"));
//    v->addObject(new MatHead("/Users/snail/desktop/shape2.txt",
//                             "/Users/snail/desktop/color2.txt"));
//    v->updateGL();
//    v->show();
//}

//void griddata_test() {

//    cv::Mat shape = read_mat("/users/snail/desktop/shape2.txt");
//    cv::Mat color = read_mat("/users/snail/desktop/color2.txt");

//    std::vector<cv::Point2f> xy = points2f_from_mat(shape,0,1);
//    std::vector<float> z = vector_from_mat<float>(color,2);
//    cv::Rect_<float> rect = rectf_from_points(xy);

//    cv::Mat grid = griddata(xy,z,rect.x,rect.y,rect.width,rect.height,0);
//    cv::imshow("12",normalize(grid));
//}

//void align_test() {
//    cv::Mat mat = cv::imread("/users/snail/desktop/image.png");
//    StatModel::FaceLocator locator("/users/snail/desktop/face3d/data/muct76.model",
//                                   "/users/snail/desktop/face3d/data/haarcascade_frontalface_alt.xml");
//    vector<StatModel::ASMFitResult> results = locator.fitAll(mat);
//    StatModel::ASMFitResult result = results[0];
//    std::vector<cv::Point> points;
//    result.toPointList(points);

//    for(uint i=0;i<points.size();++i) {
//        cv::Point p = points[i];
//        cv::circle(mat,p,2,cv::Scalar(1,0,0));
//    }
//    cv::imshow("image",mat);
//}





//int rd() {
//    return random() % 100;
//}


//string data_dir = string(getenv("HOME")) + "/desktop/face3d/data/";
//string desktop_dir = string(getenv("HOME")) + "/desktop/";

//string data_file(const string& filename) {
//    return data_dir + filename;
//}
//string desktop_file(const string& filename) {
//    return desktop_dir + filename;
//}

//VEC(cv::Point3f) centric_points(const VEC(cv::Point3f) &points,bool flipY = false) {
//    float sx=0,sy=0,sz=0;
//    FOR_EACH(it,points) {
//        sx += it->x;
//        sy += it->y;
//        sz += it->z;
//    }
//    sx /= points.size();
//    sy /= points.size();
//    sz /= points.size();

//    VEC(cv::Point3f) rts;
//    rts.reserve(points.size());
//    FOR_EACH(it,points) {
//        float x = it->x - sx;
//        float y = flipY ? (sy - it->y) : (it->y - sy);
//        float z = it->z - sz;
//        rts.push_back(cv::Point3f(x,y,z));
//    }
//    return rts;
//}

//int main(int argc, char *argv[])
//{
//    QApplication app(argc,argv);


//    (new ScribbleWidget())->show();
//    return app.exec();


//    //qDebug()<<"as"<<endl;

//    face::TemplateFace tface(data_file("shape.txt").c_str(),
//                             data_file("color.txt").c_str(),
//                             data_file("index.txt").c_str());

////    float x,y;
////    cv::Mat mat = tface.getFaceImage(0,x,y);
////    cv::imshow("",mat);
////    cv::waitKey();
////    return 0;

//    StatModel::FaceLocator locator(data_file("muct76.model").c_str(),
//                                   data_file("haarcascade_frontalface_alt.xml").c_str());

//    FaceLocateWidget w(0,locator,tface);
//    w.loadImage(desktop_file("image.png").c_str());
//    w.show();
//    app.exec();
//    const std::vector<cv::Point2f> &points  = w.featurePoints();

//    VEC(cv::Point3f) dface = tface.filter(tface.deform(points));
//    VEC(cv::Point3f) cdface = centric_points(dface,true);

//    GLViewer *v = new GLViewer();

//    v->addObject(new DotObject(cdface));
//    v->addObject(new DotObject(centric_points(tface.filter(tface.xyzs()),true)));
//    v->updateGL();

//    v->show();
//    app.exec();
//    return 0;



////    cv::Mat faceFeature = mat_from_points2f(points);
////    write_mat_to_text("/users/snail/desktop/features.txt",faceFeature);


//    //    float x,y;
//    //    cv::imshow("",face.getFaceImage(0.1,x,y));
//    //    cv::waitKey();
//    //    return 0;






////        return 0;

//    //    std::vector<cv::Point2f> pts;
//    //    std::vector<float> vs;
//    //    for(int i=0;i<10;++i) {
//    //        pts.push_back(cv::Point2f(rd(),rd()));
//    //        vs.push_back(rd());
//    //    }

//    //    Tps tps(pts,vs);

//    //    qDebug()<<tps.interpolate(pts[0].x,pts[0].y)<<vs[0];

//    //    return 0;

////    cv::Mat fm = read_mat("/users/snail/desktop/features.txt");
////    cv::Mat fs = read_mat("/users/snail/desktop/shape.txt");

////    VEC(cv::Point2f) features = points2f_from_mat(fm);
////    VEC(cv::Point3f) dface = tface.deform(features);
////    VEC(cv::Point3f) oface = points3f_from_mat(fs);


//////    std::ofstream fout("/users/snail/desktop/face.off");
//////    write_off_file(dface,
//////                   fout);
//////    return 0;


////    VEC(cv::Point2f) dxy;
////    FOR_EACH(it,dface) {
////        dxy.push_back(cv::Point2f(it->x,it->y));
////    }

////    cv::Rect_<float> rect = rectf_from_points(dxy);
////    cv::imshow("red",griddata(dxy,tface.r(),rect.x,rect.y,rect.width,rect.height,0)/255.0);

////    GLViewer *v = new GLViewer();

////    v->addObject(new DotObject(dface));
////    v->addObject(new DotObject(oface));
////    v->addObject(new USFHead("/users/snail/desktop/T0000.off"));

////    v->updateGL();
////    v->show();;
////    return  app.exec();
//}
