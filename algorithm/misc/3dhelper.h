#ifndef IO_H
#define IO_H


#include "../common.h"

#include <iostream>

void write_off_file(const std::vector<cv::Point3f> points,
                    std::ostream &fout);

void write_off_file(const std::vector<cv::Point3f> points,
                    const std::vector<cv::Vec3i> vexs,
                    std::ostream &fout);

VEC(cv::Point3f) centric_points(const VEC(cv::Point3f) &points,bool flipY = false);
VEC(cv::Point2f) centric_points(const VEC(cv::Point2f) &points,bool flipY = false);

#endif // IO_H
