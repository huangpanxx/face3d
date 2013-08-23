#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <opencv2/core/core.hpp>
#include <vector>
#include "../common.h"

cv::Mat griddata(const std::vector<cv::Point2f> &xy,const std::vector<float> &v,
                 int left,int top,int width,int height,float defaultValue = NaN);

class Tps
{
public:
    Tps();
    Tps(const std::vector<cv::Point2f> &xy,const std::vector<float> &v,float regularization=0.0);
    Tps(const std::vector<cv::Point3f> &xyz,float regularization=0.0);
    bool initialize(const std::vector<cv::Point2f> &xy, const std::vector<float> &v, float regularization=0.0);
    float interpolate(float x,float y) const;
    float interpolate(const cv::Point2f &pt) const;
private:
    std::vector<cv::Point2f> m_xy;
    cv::Mat m_X;
    int m_n;
};

class Tps2d
{
public:
    Tps2d();
    Tps2d(const VEC(cv::Point2f) &src, const VEC(cv::Point2f) &dst,float regularization=0.0);
    void initialize(const VEC(cv::Point2f) &src, const VEC(cv::Point2f) &dst,float regularization=0.0);
    cv::Point2f interpolate(float x,float y) const ;
    cv::Point2f interpolate(const cv::Point2f &pt) const;
private:
    Tps m_tps_x,m_tps_y;
};

#endif
