#include "align3d.h"
#include <algorithm/common.h>
#include <exception>
using namespace std;


class align3d_yz_equations : public equations {
public:
    align3d_yz_equations(const cv::Mat &value) : m_value(value), m_num(6) {
    }

    double operator () (const VEC(double) &x,int i) const {
        double  a = x[0] , b = x[1],
                m = x[2] , n = x[3];
        switch(i) {
        case 0: return (a*m - value_at(0,0));
        case 1: return (b - value_at(0,1));
        case 2: return (a*n - value_at(0,2));
        case 3: return (-b*m - value_at(1,0));
        case 4: return (a - value_at(1,1));
        case 5: return (-b*n - value_at(1,2));
        };
        assert(false);
    }
    int number() const {
        return this->m_num;
    }
    void set_number(int n) {
        this->m_num = n;
    }

private:
    double value_at(int i,int j) const { return m_value.at<float>(i,j); }
    const cv::Mat m_value;
    int m_num;
};


cv::Mat expand(const cv::Mat& m) {
    cv::Mat r = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<3;++i) {
        for(int j=0;j<3;++j) {
            r.at<float>(i,j) = m.at<float>(i,j);
        }
    }
    r.at<float>(3,3) = 1;
    return r;
}

cv::Mat align3d_yz(const VEC(cv::Point3f) &pt3d,const VEC(cv::Point2f)& pt2d) {

    VEC(cv::Point2f) _p = centric_points(pt2d);
    VEC(cv::Point3f) _P = centric_points(pt3d);
    //trans for 2f
    float tx2d = _p[0].x - pt2d[0].x;
    float ty2d = _p[0].y - pt2d[0].y;
    //trans for 3d
    float tx3d = _P[0].x - pt3d[0].x;
    float ty3d = _P[0].y - pt3d[0].y;


    cv::Mat mp =  mat_from_points2f(_p).t();
    cv::Mat mP = mat_from_points3f(_P).t();

    cv::Mat ap = mp*mP.t()*((mP*mP.t()).inv());


    std::vector<double> x;
    align3d_yz_equations eqs(ap);


    eqs.set_number(4);
    iter_solve(eqs,x,1e4,1e-4,1e-4);

    double  a = x[0], b = x[1],
            m = x[2], n = x[3];

    cv::Mat t1 = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<4;++i) t1.at<float>(i,i) = 1;
    t1.at<float>(0,3) = tx3d; t1.at<float>(1,3) = ty3d;

    cv::Mat t2 = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<4;++i) t2.at<float>(i,i) = 1;
    t2.at<float>(0,3) = -tx2d; t2.at<float>(1,3) = -ty2d;

    cv::Mat r1 = cv::Mat::zeros(3,3,CV_32F);
    float s1 = distance(a,b);   r1.at<float>(2,2) = s1;
    r1.at<float>(0,0) = a;      r1.at<float>(0,1) = b;
    r1.at<float>(1,0) = -b;     r1.at<float>(1,1) = a;

    cv::Mat r2 = cv::Mat::zeros(3,3,CV_32F);
    float s2 = distance(m,n);   r2.at<float>(1,1) = s2;
    r2.at<float>(0,0) = m;      r2.at<float>(0,2) = n;
    r2.at<float>(2,0) = -n;     r2.at<float>(2,2) = m;

    cv::Mat r = t2*expand(r2*r1)*t1;
    return r;
}

cv::Mat align3d_x(const VEC(cv::Point3f) &pt3d,const VEC(cv::Point2f)& pt2d) {
    VEC(cv::Point2f) _p = centric_points(pt2d);
    VEC(cv::Point3f) _P = centric_points(pt3d);
    //trans for 2f
    float tx2d = _p[0].x - pt2d[0].x;
    float ty2d = _p[0].y - pt2d[0].y;
    //trans for 3d
    float tx3d = _P[0].x - pt3d[0].x;
    float ty3d = _P[0].y - pt3d[0].y;
    //matrix
    cv::Mat mp =  mat_from_points2f(_p).t();
    cv::Mat mP = mat_from_points3f(_P).t();
    cv::Mat ap = mp*mP.t()*((mP*mP.t()).inv());
    //build
    cv::Mat t1 = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<4;++i) t1.at<float>(i,i) = 1;
    t1.at<float>(0,3) = tx3d; t1.at<float>(1,3) = ty3d;

    cv::Mat t2 = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<4;++i) t2.at<float>(i,i) = 1;
    t2.at<float>(0,3) = -tx2d; t2.at<float>(1,3) = -ty2d;

    float p = ap.at<float>(1,1) , q = ap.at<float>(1,2);
    float s = distance(p,q);

    cv::Mat r = cv::Mat::zeros(3,3,CV_32F);
    r.at<float>(0,0) = s;
    r.at<float>(1,1) = p; r.at<float>(1,2) = q;
    r.at<float>(2,1) = -q;r.at<float>(2,2) = p;
    return t2*expand(r)*t1;

}

cv::Mat align3d(const VEC(cv::Point3f) &pt3d,const VEC(cv::Point2f)& pt2d,int iter) {
    VEC(cv::Point3f) pts = pt3d;
    cv::Mat m = cv::Mat::zeros(4,4,CV_32F);
    for(int i=0;i<4;++i)
        m.at<float>(i,i) = 1;
    for(int i=0;i<iter;++i) {
        cv::Mat t1 = align3d_yz(pts,pt2d);
        pts = t1*pts;
        m = t1*m;
        cv::Mat t2 = align3d_x(pts,pt2d);
        pts = t2*pts;
        m = t2*m;
    }
    return m;
}
