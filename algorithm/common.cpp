#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include "common.h"



void hsl_to_rgb(double h, double s, double L,int &rr, int &gg,int &bb) {
    double v,m,sv, vs0, c1, c2,xiaoshu,r,g,b;
    int ih;
    r = L; g = L;b = L;
    if (L <= 0.5) v = L * (1.0 + s); else v = L + s - L * s;
    if (v > 0) {
        m = L + L - v; sv = (v - m ) / v;
        h = h * 6.0; ih = (int)h;
        xiaoshu = h - ih;
        vs0 = v * sv * xiaoshu; c1 = m + vs0; c2 = v - vs0;
        switch (ih) {
        case 0: r = v; g = c1; b = m; break;
        case 1: r = c2; g = v; b = m; break;
        case 2: r = m;  g = v; b = c1; break;
        case 3: r = m; g = c2; b = v; break;
        case 4: r = c1; g = m; b = v; break;
        case 5: r = v;  g = m; b = c2; break;
        default: break;
        }
    }
    rr =  (int) (r * 0xff); gg =  (int) (g * 0xff); bb =  (int) (b * 0xff);
}

void zeros(double *p,int sz) {
    memset(p,0,sz*sizeof(double));
}


double* malloc_zeroed_buffer(int sz) {
    double *p = new double[sz];
    zeros(p,sz);
    return p;
}



void draw_poly(cv::Mat &mat, float x,float y,
               const std::vector<cv::Point2f> &vex) {
    for(uint i=0;i<vex.size();++i) {
        cv::Point2f p1 = vex[i], p2 = vex[(i+1)%vex.size()];
        cv::line(mat,cv::Point2f(p1.x-x,p1.y-y),
                 cv::Point2f(p2.x-x,p2.y-y),cv::Scalar(1));
    }
}

void draw_tri(cv::Mat &mat,float x,float y,const std::vector<cv::Point2f> &pts,
               const cv::Vec3i &tri) {
    std::vector<cv::Point2f> vex;
    for(int i=0;i<3;++i) {
        vex.push_back(pts[tri(i)]);
    }
    draw_poly(mat,x,y,vex);
}

void draw_tris(cv::Mat &mat,float x,float y,const std::vector<cv::Point2f> &pts,
               const std::vector<cv::Vec3i> &tris) {

    for(uint i=0;i<tris.size();++i) {
        draw_tri(mat,x,y,pts,tris[i]);
    }
}

cv::Point3f apply_matrix(const cv::Mat &A,const cv::Point3f &p) {
    const int r = A.rows;
    float tx = 0,ty = 0,tz = 0;
    if(4 == r) {
        tx = A.at<float>(0,3);
        ty = A.at<float>(1,3);
        tz = A.at<float>(2,3);
    }
    float x = A.at<float>(0,0)*p.x + A.at<float>(0,1)*p.y + A.at<float>(0,2)*p.z + tx;
    float y = A.at<float>(1,0)*p.x + A.at<float>(1,1)*p.y + A.at<float>(1,2)*p.z + ty;
    float z = A.at<float>(2,0)*p.x + A.at<float>(2,1)*p.y + A.at<float>(2,2)*p.z + tz;
    return cv::Point3f(x,y,z);
}

void init_resources() {
    Q_INIT_RESOURCE(resource);
}
