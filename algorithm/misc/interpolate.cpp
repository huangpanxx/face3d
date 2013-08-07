#include "interpolate.h"
#include <QDebug>
#include "common.h"

void fill_tri(cv::Mat &mat,const cv::Vec3i &tri, const std::vector<cv::Point2f> &xy,
              const std::vector<float> &v,int _l,int _t) {
    float x1 = xy[tri(0)].x, x2 = xy[tri(1)].x, x3 = xy[tri(2)].x;
    float y1 = xy[tri(0)].y, y2 = xy[tri(1)].y, y3 = xy[tri(2)].y;

    float left = min(x1,x2,x3), right = max(x1,x2,x3);
    float top = min(y1,y2,y3),  bottom = max(y1,y2,y3);

    std::vector<cv::Point2f> pts; pts.reserve(3);
    pts.push_back(xy[tri(0)]);
    pts.push_back(xy[tri(1)]);
    pts.push_back(xy[tri(2)]);

    cv::Mat A(3,3,CV_32F);
    cv::Mat B(3,1,CV_32F);
    for(int i=0;i<3;++i) {
        int j = tri(i);
        A.at<float>(i,0) = xy[j].x;
        A.at<float>(i,1) = xy[j].y;
        A.at<float>(i,2) = 1;
        B.at<float>(i,0) = v[j];
    }
    cv::invert(A,A);
    cv::Mat T = A*B;
    float ca = T.at<float>(0,0), cb = T.at<float>(1,0), cc = T.at<float>(2,0);

    for(int x = floor(left); x <= ceil(right); ++ x) {
        for(int y=floor(top); y<=ceil(bottom) ; ++y) {
            int r = y-_t,c = x-_l;
            if(r>=0 && r<mat.rows && c>=0 && c<mat.cols) {
                PolyResult re = is_in_poly(cv::Point2f(x,y),pts);
                if(re != out) {
                    mat.at<float>(r,c) = ca*x+cb*y+cc;
                }
            }
        }
    }
}

cv::Mat griddata(const std::vector<cv::Point2f> &xy,const std::vector<float> &v,
                 int left,int top,int width,int height,float defaultValue) {
    assert(width>0 && height>0);
    BEGIN_TIME;
    cv::Mat grid = cv::Mat::ones(height,width,CV_32F)*defaultValue;
    std::vector<cv::Vec3i> tris = trianglate(xy);
    for(uint i=0;i<tris.size();++i) {
        const cv::Vec3i &tri = tris[i];
        fill_tri(grid,tri,xy,v,left,top);
    }
    PRINT_TIME("griddata");
    return grid;
}
