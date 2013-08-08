#ifndef IO_H
#define IO_H


#include "common.h"

#include <iostream>

void write_off_file(const std::vector<cv::Point3f> points,
                    const std::ostream &fout);

void write_off_file(const std::vector<cv::Point3f> points,
                    const std::vector<cv::Point3i> vexs,
                    const std::ostream& fout);

#endif // IO_H
