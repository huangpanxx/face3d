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

void align3d(const VEC(cv::Point3f) &P,const VEC(cv::Point2f)& p,cv::Mat &A) {
    VEC(cv::Point2f) _p = centric_points(p);
    VEC(cv::Point3f) _P = centric_points(P);
    cv::Mat mp =  mat_from_points2f(_p).t();
    cv::Mat mP = mat_from_points3f(_P).t();

    cv::Mat a = mp*mP.t()*((mP*mP.t()).inv());
    A = a;
    cout<<A*mP<<endl;
    cout<<mp<<endl;
    return;

    float ptx = _p[0].x - p[0].x;
    float pty = _p[0].y - p[0].y;
    float Ptx = _P[0].x - P[0].x;
    float Pty = _P[0].y - P[0].y;
    float Ptz = _P[0].z - P[0].z;

    float tx = ptx - a.at<float>(0,0) * Ptx - a.at<float>(0,1) * Pty - a.at<float>(0,2) * Ptz;
    float ty = pty - a.at<float>(1,0) * Ptx - a.at<float>(1,1) * Pty - a.at<float>(1,2) * Ptz;

    A = cv::Mat(3,3,CV_32F);
    for(int i=0;i<2;++i){
        for(int j=0;j<3;++j) {
            A.at<float>(i,j) = a.at<float>(i,j);
        }
    }
    A.at<float>(2,0) = tx;
    A.at<float>(2,1) = ty;
    A.at<float>(2,2) = 0;
}

cv::Point3f apply_matrix(const cv::Mat &A,const cv::Point3f &p) {
    float x = A.at<float>(0,0)*p.x + A.at<float>(0,1)*p.y + A.at<float>(0,2)*p.z;
    float y = A.at<float>(1,0)*p.x + A.at<float>(1,1)*p.y + A.at<float>(1,2)*p.z;
    //float z = A.at<float>(2,0)*p.x + A.at<float>(2,1)*p.y + A.at<float>(2,2)*p.z;
    float a1 = A.at<float>(0,0) + A.at<float>(0,1) + A.at<float>(0,2);
    float a2 = A.at<float>(1,0) + A.at<float>(1,1) + A.at<float>(1,2);
    return cv::Point3f(x,y,p.z/distance(a1,a2));
}

cv::Point3f operator * (const cv::Mat &A,const cv::Point3f &p) {
    return apply_matrix(A,p);
}

VEC(cv::Point3f) operator * (const cv::Mat &A,const VEC(cv::Point3f) &pts) {
    VEC(cv::Point3f) new_pts; new_pts.reserve(pts.size());
    FOR_EACH(it,pts) {
        new_pts.push_back(A*(*it));
    }
    return new_pts;
}


int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    //初始化
    face::TemplateFace tface("./data/face/",0.0001,10);

    //载入图像
    cv::Mat image = cv::imread("/home/snail/桌面/right.png");
    float x,y;
    cv::Mat timage= tface.getFaceImage(100,x,y);

    //定位
    FaceLocator locator("./data/stasm/");
    VEC(cv::Point2f) pts = locator.fit(image),
            tpts = locator.fit(timage);

    //得到特征点
    tpts = translate(tpts,x,y);
    VEC(cv::Point3f) tpts3d = tface.interpolate(tpts);

    //拟合
    cv::Mat A;
    align3d(tpts3d,pts,A);


    cv::Mat im = cv::Mat::zeros(400,400,CV_32FC3);

    FOR_EACH(it,tpts3d) {
        cout<<*it<<endl;
        cv::circle(im,cv::Point(it->x+200,it->y+200),1,cv::Scalar( 0,0,1));
    }

    tpts3d = A*centric_points(tpts3d);
    pts = centric_points(pts);


    FOR_EACH(it,tpts3d) {
        cout<<*it<<endl;
        cv::circle(im,cv::Point(it->x+100,it->y+100),1,cv::Scalar(1,0,0));
    }
    cout<<"other"<<endl;
    FOR_EACH(it,pts) {
        cout<<*it<<endl;
        cv::circle(im,cv::Point(it->x+300,it->y+300),1,cv::Scalar(0,1,0));
    }
    cv::imshow("win",im);




    GLViewer viewer;
    viewer.addObject(new DotObject(A*centric_points(tface.xyzs(true)),true,true));
    viewer.addObject(new DotObject(tface.xyzs(true),true,true));
    viewer.addObject(new DotObject(tface.deform(tpts,0,0,pts,0.01),true,true));
    viewer.show();

    return app.exec();


}
