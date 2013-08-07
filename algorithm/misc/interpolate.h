#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <opencv2/core/core.hpp>
#include <vector>
#include "common.h"

cv::Mat griddata(const std::vector<cv::Point2f> &xy,const std::vector<float> &v,
                 int left,int top,int width,int height,float defaultValue = NaN);

class Tps
{
public:
    Tps(const std::vector<cv::Point2f> &xy,const std::vector<float> &v);
    float interpolate(float x,float y);
private:
    std::vector<cv::Point2f> m_xy;
    cv::Mat m_X;
    int m_n;
};

#endif
