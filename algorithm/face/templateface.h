#ifndef TEMPLATEFACE_H
#define TEMPLATEFACE_H

#include <vector>
#include <opencv2/core/core.hpp>
#include "../common.h"

namespace face{

class TemplateFace {
public:
    TemplateFace(const char*shapefile , const char *colorfile,const char*indexfile,float regularization,float dis_th);
    TemplateFace(const char*datadir, float regularization, float dis_th);
    cv::Mat getFaceImage(float blank,float &x,float &y) const;
    cv::Mat getFaceImage(float blank) const {
        float x,y;
        return this->getFaceImage(blank,x,y);
    }

    const std::vector<cv::Point2f> &xys() const {
        return this->m_xys;
    }
    const cv::Point2f& xy(int i) const {
        return this->m_xys[i];
    }
    const std::vector<int>& indexs() const {
        return this->m_indexs;
    }

    VEC(cv::Point3f) interpolate(const VEC(cv::Point2f) &pts) const;

    std::vector<cv::Point3f>
    deform(const std::vector<cv::Point2f> &temp_face_feature_points,
           float ox,float oy,
           const std::vector<cv::Point2f> &obj_face_feature_points,float regularization = 0.0,bool filte = true) const;

    const std::vector<float>& r() const {
        return this->m_r;
    }
    const std::vector<float>& g() const {
        return this->m_g;
    }
    const std::vector<float>& b() const {
        return this->m_b;
    }

    const std::vector<cv::Point3f> xyzs(bool filte = false) const {
        return filte ? this->filter(this->m_xyzs) : this->m_xyzs;
    }

    template <class T>
    std::vector<T> filter(const std::vector<T> & src) const {
        VEC(T) dst; dst.reserve(this->m_indexs.size());
        for(auto &it :this->m_indexs) {
            dst.push_back(src[it]);
        }
        return dst;
    }

protected:
     static cv::Mat align2d(const VEC(cv::Point2f) &src,const VEC(cv::Point2f) &dst);

private:
    void initialize(const std::string &shapefile , const std::string &colorfile, const std::string &indexfile, float regularization, float dis_th);
    Tps m_tps;
    std::vector<cv::Point2f> m_xys;
    std::vector<cv::Point3f> m_xyzs;
    std::vector<float> m_r,m_g,m_b;
    std::vector<float> m_z;
    std::vector<int> m_indexs;
};

}

#endif // TEMPLATEFACE_H
