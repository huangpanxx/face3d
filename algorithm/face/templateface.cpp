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

TemplateFace::TemplateFace(const char*datadir, float regularization,float dis_th) {
    std::string dir = std::string(datadir) + "/";
    this->initialize(dir + "shape.txt", dir + "color.txt", dir + "index.txt",regularization,dis_th);
}

TemplateFace::TemplateFace(const char*shapefile , const char *colorfile, const char*indexfile,float regularization,float dis_th) {
    initialize(shapefile,colorfile,indexfile,regularization,dis_th);
}

void TemplateFace::initialize(const std::string &shapefile ,
                         const std::string &colorfile,
                         const std::string &indexfile,
                         float regularization,float dis_th) {
    cv::Mat mshape = read_mat(shapefile.c_str());
    cv::Mat mcolor = read_mat(colorfile.c_str());
    cv::Mat mindex = read_mat(indexfile.c_str());

    m_xyzs = points3f_from_mat(mshape,0,1,2);
    m_xys = points2f_from_mat(mshape,0,1);
    m_z = vector_from_mat<float>(mshape,2);

    m_r = vector_from_mat<float>(mcolor,0);
    m_g = vector_from_mat<float>(mcolor,1);
    m_b = vector_from_mat<float>(mcolor,2);
    this->m_indexs = vector_from_mat<int>(mindex,0);
    std::sort(this->m_indexs.begin(),this->m_indexs.end());

    VEC(cv::Point3f) pts3d = remove_near_points2d(this->filter(m_xyzs),dis_th);
    this->m_tps.initialize(xy_from_xyz(pts3d),z_from_xyz(pts3d),regularization);

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

cv::Mat TemplateFace::align2d(const VEC(cv::Point2f) &src,const VEC(cv::Point2f) &dst)  {
    cv::Mat msrc = cv::Mat::zeros(4,4,CV_32F),
            mdst = cv::Mat::zeros(4,1,CV_32F),
            mtran;

    for(uint i=0;i<src.size();++i) {
        const cv::Point2f &xy_src = src[i];
        const cv::Point2f &xy_dst = dst[i];
        float x = xy_src.x,   y = xy_src.y;
        float _x = xy_dst.x, _y = xy_dst.y;
        float r = xy_src.dot(xy_src);
        msrc.at<float>(0,0) += x;  msrc.at<float>(0,1) += y; msrc.at<float>(0,2) += 1; mdst.at<float>(0,0) += _x;
        msrc.at<float>(1,0) += y; msrc.at<float>(1,1) += -x; msrc.at<float>(1,3) += 1; mdst.at<float>(1,0) += _y;
        msrc.at<float>(2,0) += r; msrc.at<float>(2,2) += x; msrc.at<float>(2,3) += y;  mdst.at<float>(2,0) += x*_x+y*_y;
        msrc.at<float>(3,1) += r; msrc.at<float>(3,2) += y; msrc.at<float>(3,3) += -x; mdst.at<float>(3,0) += y*_x-x*_y;
    }
    cv::solve(msrc,mdst,mtran);
    return mtran;
}

VEC(cv::Point3f) TemplateFace::interpolate(const VEC(cv::Point2f) &pts) const {
    VEC(cv::Point3f) dst; dst.reserve(pts.size());
    for(auto &it :pts) {
        float z= this->m_tps.interpolate(it);
        dst.push_back(cv::Point3f(it.x,it.y,z));
    }
    return dst;
}



std::vector<cv::Point3f>
TemplateFace::deform(const std::vector<cv::Point2f> &_src, float ox, float oy,
       const std::vector<cv::Point2f> &dst, float regularization, bool filte) const {
    assert(_src.size() == dst.size());

    const uint n = _src.size();
    VEC(cv::Point2f) src; src.reserve(n);
    for(auto &it :_src) {
        src.push_back(cv::Point2f(it.x+ox,it.y+oy));
    }


    //least square to estimate scale
    cv::Mat mtran = align2d(src,dst);
    float a = mtran.at<float>(0,0);
    float b = mtran.at<float>(0,1);
    float scale = distance(a,b);

    //interpolate
    Tps2d tps(src,dst,regularization);
    VEC(cv::Point3f) new_face;
    new_face.reserve(this->m_xys.size());
    for(auto &it :this->m_xyzs) {
        cv::Point2f pt = tps.interpolate(it.x,it.y);
        new_face.push_back(cv::Point3f(pt.x,pt.y,it.z*scale));
    }

    if(filte) {
        new_face = filter(new_face);
    }
    return new_face;
}


}

