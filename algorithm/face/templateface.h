#ifndef TEMPLATEFACE_H
#define TEMPLATEFACE_H

#include <vector>
#include <opencv2/core/core.hpp>
#include "common.h"

namespace face{

class TemplateFace {
public:
    TemplateFace(const char*shapefile , const char *colorfile,const char*indexfile);
    cv::Mat getFaceImage(float blank,float &x,float &y) const;
    const std::vector<cv::Point2f> &xys() {
        return this->m_xys;
    }
    const cv::Point2f& xy(int i) const {
        return this->m_xys[i];
    }
    const std::vector<int>& indexs() const {
        return this->m_indexs;
    }

    std::vector<cv::Point3f> deform(const std::vector<cv::Point2f> &feature_points) const;
    const std::vector<float>& r() const {
        return this->m_r;
    }
    const std::vector<float>& g() const {
        return this->m_g;
    }
    const std::vector<float>& b() const {
        return this->m_b;
    }

    template <class T>
    std::vector<T> filter(const std::vector<T> & src) {
        VEC(T) dst; dst.reserve(this->m_indexs.size());
        FOR_EACH(it,this->m_indexs) {
            dst.push_back(src[*it]);
        }
        return dst;
    }

private:
    std::vector<cv::Point2f> m_xys;
    std::vector<float> m_r,m_g,m_b;
    std::vector<float> m_z;
    std::vector<int> m_indexs;
};

}

#endif // TEMPLATEFACE_H
