#include "templateface.h"

#include <vector>
#include <opencv2/core/core.hpp>
#include <fstream>
#include <QImage>
#include "common.h"
#include "fiber.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
namespace face{

template <class T>
std::vector<T> select_index(const std::vector<T> &elems,const std::vector<int> &indexs) {
    std::vector<T> new_elems; new_elems.reserve(indexs.size());
    for(uint i=0;i<indexs.size();++i) {
        new_elems.push_back(elems[indexs[i]]);
    }
    return new_elems;
}


TemplateFace::TemplateFace(const char*shapefile , const char *colorfile, const char*indexfile) {
    cv::Mat mshape = read_mat(shapefile);
    cv::Mat mcolor = read_mat(colorfile);
    cv::Mat mindex = read_mat(indexfile);

    m_xys = points2f_from_mat(mshape,0,1);
    m_z = vector_from_mat<float>(mshape,2);

    m_r = vector_from_mat<float>(mcolor,0);
    m_g = vector_from_mat<float>(mcolor,1);
    m_b = vector_from_mat<float>(mcolor,2);
    this->m_indexs = vector_from_mat<int>(mindex,0);
    std::sort(this->m_indexs.begin(),this->m_indexs.end());
}

cv::Mat TemplateFace::getFaceImage(float blank,float &x,float &y) const {


    VEC(cv::Point2f) xy = select_index(this->m_xys,this->m_indexs);
    VEC(float) r = select_index(this->m_r,this->m_indexs);
    VEC(float) g = select_index(this->m_g,this->m_indexs);
    VEC(float) b = select_index(this->m_b,this->m_indexs);


    cv::Rect_<float> rect = rectf_from_points(xy);
    rect.x -= blank; rect.y -= blank; rect.width += blank*2; rect.height += blank*2;
    x = rect.x; y = rect.y;


    cv::Mat grid_r = griddata(xy,r,rect.x,rect.y,rect.width,rect.height,0)/255;
    cv::Mat grid_g = griddata(xy,g,rect.x,rect.y,rect.width,rect.height,0)/255;
    cv::Mat grid_b = griddata(xy,b,rect.x,rect.y,rect.width,rect.height,0)/255;

    cv::Mat bgr[] = {grid_b,grid_g,grid_r};
    cv::Mat dst;
    cv::merge(bgr,3,dst);

    return dst;
}

std::vector<cv::Point3f> TemplateFace::deform(const std::vector<cv::Point2f> &feature_points) const {
    const uint n = feature_points.size();
    std::vector<cv::Point2f> src(this->m_xys.begin(),this->m_xys.begin() + n);
    std::vector<float> dst_x; dst_x.reserve(this->m_xys.size());
    std::vector<float> dst_y; dst_y.reserve(this->m_xys.size());

    FOR_EACH(pt,feature_points) {
        dst_x.push_back(pt->x);
        dst_y.push_back(pt->y);
    }

    Tps tps_x(src,dst_x), tps_y(src,dst_y);

    std::vector<cv::Point3f> deformed_face;
    deformed_face.reserve(this->m_xys.size());

    for(uint i=0;i<this->m_xys.size();++i) {
        const cv::Point2f &xy = this->m_xys[i];
        float new_x = tps_x.interpolate(xy.x,xy.y);
        float new_y = tps_y.interpolate(xy.x,xy.y);
        deformed_face.push_back(
                    cv::Point3f(new_x,new_y,this->m_z[i])
                    );
    }
    return deformed_face;
}

}

