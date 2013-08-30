#ifndef ALIGN3D_H
#define ALIGN3D_H

#include <algorithm/common.h>

cv::Mat align3d(const VEC(cv::Point3f) &P,const VEC(cv::Point2f)& p,int iter=3);

#endif // ALIGN3D_H
