#ifndef CVEXTEND_H
#define CVEXTEND_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <QImage>
#include "../common.h"
#include <ostream>
#include <QDebug>

cv::Mat read_mat_from_bin(const char *file);
cv::Mat read_mat_from_text(const char *file);

bool write_mat_to_text(const char* file,const cv::Mat& mat);
bool write_mat_to_bin(const char* file,const cv::Mat & mat);

bool write_mat(const char* filename, const cv::Mat &mat);

cv::Mat read_mat(const char* filename);

std::vector<cv::Point2f> points2f_from_mat(const cv::Mat &src, int c1=0, int c2=1);

std::vector<cv::Point3f> points3f_from_mat(const cv::Mat &src, int c1=0, int c2=1,int c3=2);

std::vector<cv::Point2f> xy_from_xyz(const VEC(cv::Point3f) &points3f);

std::vector<float> x_from_xyz(const VEC(cv::Point3f) &xyz);
std::vector<float> y_from_xyz(const VEC(cv::Point3f) &xyz);
std::vector<float> z_from_xyz(const VEC(cv::Point3f) &xyz);
std::vector<float> x_from_xy(const VEC(cv::Point2f) &xy);
std::vector<float> y_from_xy(const VEC(cv::Point2f) &xy);

template <class T>
std::vector<T> vector_from_mat(const cv::Mat &src,int col = 0) {
    assert(src.type()==CV_32F && src.cols > col);
    std::vector<T> v; v.reserve(src.rows);
    for(int i=0;i<src.rows;++i)
        v.push_back(src.at<float>(i,col));
    return v;
}

std::vector<cv::Point2f> translate(const VEC(cv::Point2f) &xy,float tx,float ty);


template<class T>
cv::Mat mat_from_points3f(const std::vector<T> points) {
    cv::Mat mat(points.size(),3, CV_32F);
    for(uint i=0;i<points.size();++i) {
        const T &point = points[i];
        mat.at<float>(i,0) = point.x;
        mat.at<float>(i,1) = point.y;
        mat.at<float>(i,2) = point.z;
    }
    return mat;
}

template<class T>
cv::Mat mat_from_points2f(const std::vector<T> points) {
    cv::Mat mat(points.size(),2, CV_32F);
    for(uint i=0;i<points.size();++i) {
        const T &point = points[i];
        mat.at<float>(i,0) = point.x;
        mat.at<float>(i,1) = point.y;

    }
    return mat;
}

template<class T>
cv::Mat mat_from_vecs(const std::vector<T> vec,int channel) {
    cv::Mat mat(vec.size(),channel, CV_32F);
    for(uint i=0;i<vec.size();++i) {
        const T &point = vec[i];
        for(int j=0;j<channel;++j) {
            mat.at<float>(i,j) = point(j);
        }
    }
    return mat;
}

template <class T>
cv::Mat mat_from_vector(const std::vector<T> vec) {
    cv::Mat mat(vec.size(),vec.size(),CV_32F);
    for(uint i=0;i<vec.size();++i) {
        mat.at<float>(i,0) = vec[i];
    }
    return mat;
}

cv::Mat to_cv8u_image(const cv::Mat &src);

cv::Mat normalize(cv::Mat src,float maxv = 1);

float interpolate(const cv::Mat &src, float y, float x);

inline cv::Mat to_float_mat(const cv::Mat &src) {
    assert(src.channels() == 1);
    cv::Mat dst; src.convertTo(dst,CV_32F);
    return dst;
}

template <class T>
QRgb vec4_to_rgba(T vec4) {
    return qRgba (vec4(2),vec4(1),vec4(0),vec4(3));
}
template <class T>
QRgb vec3_to_rgb(T vec3) {
    return qRgb(vec3(2),vec3(1),vec3(0));
}

cv::Mat image_to_mat(const QImage &image, bool contain_alpha = false);
QImage mat_to_image(const cv::Mat &mat);




#endif // CVEXTEND_H
