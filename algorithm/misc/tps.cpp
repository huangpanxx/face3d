#include "tps.h"


inline float base_func(float r){
    if(is_zero(r))	return r;
    else return r * r * log(r);
}


Tps::Tps(const std::vector<cv::Point2f> &xy,const std::vector<float> &v)
{
    assert(xy.size() == v.size());

    this->m_xy = xy;
    this->m_n = (int)xy.size();

    cv::Mat A(m_n+3,m_n+3,CV_32F),B(m_n+3,1,CV_32F);
    //left up corner
    for(int i=0;i<m_n;++i){
        for(int j=i;j<m_n;++j){
            const cv::Point2f &p1 = xy[i], p2 = xy[j];
            A.at<float>(i,j) = A.at<float>(j,i) = base_func(abs(p1-p2));
        }
    }
    //right up and left down corner
    for(int i = 0; i < m_n; ++i){
        const cv::Point2f &p = xy[i];
        A.at<float>(i,m_n+0) = A.at<float>(m_n+0,i) = 1;
        A.at<float>(i,m_n+1) = A.at<float>(m_n+1,i) = p.x;
        A.at<float>(i,m_n+2) = A.at<float>(m_n+2,i) = p.y;
    }
    //right down corner
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            A.at<float>(m_n+i,m_n+j) = 0;
        }
    }
    //up of B
    for(int i=0;i<m_n;++i){
        B.at<float>(i,0) = v[i];
    }
    //down of B
    for(int i=m_n;i<m_n+3;++i) {
        B.at<float>(i,0) = 0;
    }
    cv::solve(A,B,m_X);
}


float Tps::interpolate(float x,float y){
    float a1 = this->m_X.at<float>(this->m_n+0,0);
    float a2 = this->m_X.at<float>(this->m_n+1,0);
    float a3 = this->m_X.at<float>(this->m_n+2,0);
    float h = a1 + a2*x + a3*y;
    for(int i=0;i<this->m_n;++i) {
        float w = this->m_X.at<float>(i,0);
        const cv::Point2f &p = this->m_xy[i];
        h += w * base_func(distance(x-p.x,y-p.y));
    }
    return h;
}
