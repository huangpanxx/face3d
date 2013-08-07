#ifndef TPS_H
#define TPS_H

#include <opencv2/core/core.hpp>
#include "common.h"
#include <vector>

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

#endif // TPS_H
