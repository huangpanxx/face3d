#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include <opencv2/core/core.hpp>

std::vector<cv::Vec3i> trianglate(const std::vector<cv::Point2f> &points);

#endif // TRIANGLE_H
