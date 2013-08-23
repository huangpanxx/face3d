#include "cvextend.h"
#include "common.h"
#include <fstream>
#include <QDebug>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;

bool write_mat(const char* filename, const cv::Mat &mat) {
    string s(filename);
    if(s.length()>=4   && s.substr(s.length()-4) == ".bin") {
        return write_mat_to_bin(filename,mat);
    }else{
        return write_mat_to_text(filename,mat);
    }
}

cv::Mat read_mat(const char* filename) {
    string s(filename);
    if(s.length()>=4   && s.substr(s.length()-4) == ".bin") {
        return read_mat_from_bin(filename);
    }else{
        return read_mat_from_text(filename);
    }
}

bool write_mat_to_text(const char* file,const cv::Mat& mat) {
    std::ofstream fout(file);
    fout<<mat.rows<<"\t"<<mat.cols<<std::endl;
    for(int i = 0; i < mat.rows;++i) {
        for(int j=0; j < mat.cols;++j) {
            if(j!=0) fout<<"\t";
            fout<<mat.at<float>(i,j);
        }
        fout<<std::endl;
    }
    return fout;
}

bool write_mat_to_bin(const char* file,const cv::Mat & mat) {
    std::ofstream fout(file);
    fout.write((char*)(&mat.rows),sizeof(int));
    fout.write((char*)(&mat.cols),sizeof(int));
    for(int i=0;i<mat.rows;++i) {
        for(int j=0;j<mat.cols;++j) {
            float v = mat.at<float>(i,j);
            fout.write((char*)&v,sizeof(float));
        }
    }
    return fout;
}

cv::Mat read_mat_from_bin(const char *file){
    std::ifstream fin(file,std::ios::binary);
    if(!fin) return cv::Mat();
    float rows,columns;
    fin.read((char*)&rows,sizeof(int));
    fin.read((char*)&columns,sizeof(int));

    if(rows<=0 || columns<=0){
        return cv::Mat();
    }

    cv::Mat mat(rows,columns,CV_32F);
    for(int i = 0;i<rows;++i){
        for(int j=0;j<columns;++j){
            float value;
            fin.read((char*)(&value),sizeof(float));
            mat.at<float>(i,j) = value;
        }
    }
    return mat;
}

cv::Mat read_mat_from_text(const char *file) {
    std::ifstream fin(file);
    if(!fin) return cv::Mat();
    int rows,columns;
    fin>>rows>>columns;
    if(rows<=0 || columns<=0){
        return cv::Mat();
    }
    cv::Mat mat(rows,columns,CV_32F);
    for(int i = 0;i<rows;++i){
        for(int j=0;j<columns;++j){
            float value;
            fin>>value;
            mat.at<float>(i,j) = value;
        }
    }
    return mat;
}


std::vector<cv::Point3f> points3f_from_mat(const cv::Mat &src, int c1, int c2,int c3) {
    assert(src.cols>c1 && src.cols>c2 && src.cols>c3);
    std::vector<cv::Point3f> pts;
    pts.reserve(src.rows);
    for(int i=0;i<src.rows;++i){
        pts.push_back(cv::Point3f(src.at<float>(i,c1),
                                  src.at<float>(i,c2),
                                  src.at<float>(i,c3)));
    }
    return pts;
}


std::vector<cv::Point2f> points2f_from_mat(const cv::Mat &src, int c1, int c2) {
    assert(src.cols>c1 && src.cols>c2);
    std::vector<cv::Point2f> pts;
    pts.reserve(src.rows);
    for(int i=0;i<src.rows;++i){
        pts.push_back(cv::Point2f(src.at<float>(i,c1),
                                  src.at<float>(i,c2)));
    }
    return pts;
}

std::vector<cv::Point2f> translate(const VEC(cv::Point2f) &xy,float tx,float ty) {
    VEC(cv::Point2f) pts; pts.reserve(xy.size());
    FOR_EACH(it,xy) {
        pts.push_back(cv::Point2f(it->x+tx,it->y+ty));
    }
    return pts;
}

std::vector<cv::Point2f> xy_from_xyz(const VEC(cv::Point3f) &points3f) {
    std::vector<cv::Point2f> points2f;
    points2f.reserve(points3f.size());
    for(uint i=0;i<points3f.size();++i){
        const cv::Point3f &p = points3f.at(i);
        points2f.push_back(cv::Point2f(p.x,p.y));
    }
    return points2f;
}


std::vector<float> x_from_xyz(const VEC(cv::Point3f) &points3f) {
    std::vector<float> X; X.reserve(points3f.size());
    FOR_EACH(it,points3f) {
        X.push_back(it->x);
    }
    return X;
}

std::vector<float> y_from_xyz(const VEC(cv::Point3f) &xyz) {
    std::vector<float> Y; Y.reserve(xyz.size());
    FOR_EACH(it,xyz) {
        Y.push_back(it->y);
    }
    return Y;
}

std::vector<float> z_from_xyz(const VEC(cv::Point3f) &xyz) {
   std::vector<float> Z; Z.reserve(xyz.size());
   FOR_EACH(it,xyz) {
       Z.push_back(it->z);
   }
   return Z;
}

std::vector<float> x_from_xy(const VEC(cv::Point2f) &xy) {
    std::vector<float> X; X.reserve(xy.size());
    FOR_EACH(it,xy) {
        X.push_back(it->x);
    }
    return X;
}

std::vector<float> y_from_xy(const VEC(cv::Point2f) &xy) {
    std::vector<float> Y; Y.reserve(xy.size());
    FOR_EACH(it,xy) {
        Y.push_back(it->y);
    }
    return Y;
}

cv::Mat normalize(cv::Mat _src,float maxv) {
    cv::Mat src; _src.convertTo(src,CV_32F);
    cv::Mat dst(src.size(),src.type());
    float mmax = -1e9, mmin = -mmax;
    for(int r=0;r<src.rows;++r) {
        for(int c=0;c<src.cols;++c) {
            float e = src.at<float>(r,c);
            mmax = fmax(e,mmax);
            mmin = fmin(e,mmin);
        }
    }
    for(int r=0;r<src.rows;++r) {
        for(int c=0;c<src.cols;++c){
            dst.at<float>(r,c) = (src.at<float>(r,c)-mmin)/(mmax-mmin+1e-8)*maxv;
        }
    }
    return dst;
}

float interpolate(const cv::Mat& src, float y, float x) {
    int xPos = (int) x, yPos = (int) y;
    float alpha = x-xPos, beta = y-yPos;

    const float
            A = src.at<float>(yPos,xPos),
            B = (xPos+1<src.cols)?src.at<float>(yPos,xPos+1) : 0,
            C = (yPos+1<src.rows)?src.at<float>(yPos+1,xPos):0,
            D = (xPos+1<src.cols && yPos+1<src.rows)?src.at<float>(yPos+1,xPos+1):0;

    return (float)
            (A * ((1-alpha) * (1-beta))
             + B * (   alpha  * (1-beta))
             + C * ((1-alpha) *    beta)
             + D *     alpha  *    beta);
}



cv::Mat image_to_mat(const QImage &image,bool contain_alpha) {
    cv::Mat mat(image.height(),image.width(),contain_alpha ? CV_8UC4 : CV_8UC3);
    for(int r = 0;r<image.height();++r) {
        for(int c = 0;c<image.width();++c) {
            QRgb rgb = image.pixel(c,r);
            if(contain_alpha) {
                mat.at<cv::Vec4b>(r,c) = cv::Vec4b(qBlue(rgb),qGreen(rgb),qRed(rgb),qAlpha(rgb));
            } else {
                mat.at<cv::Vec3b>(r,c) = cv::Vec3b(qBlue(rgb),qGreen(rgb),qRed(rgb));
            }
        }
    }
    return mat;
}



QImage mat_to_image(const cv::Mat &mat) {
    assert(mat.type() == CV_8UC4 || mat.type() ==CV_8UC3 ||
           mat.type() == CV_32FC4 || mat.type() == CV_32FC3);

    QImage image(mat.cols,mat.rows,QImage::Format_ARGB32);
    bool alpha = (mat.channels() == 4);
    bool is_float  = (mat.type() == CV_32FC4 || mat.type() == CV_32FC3);

    for(int r = 0;r<mat.rows;++r) {
        for(int c=0;c<mat.cols;++c) {
            QRgb rgb;

            if(alpha) {
                if(is_float)  {
                    rgb = vec4_to_rgba(mat.at<cv::Vec4f>(r,c)*255);
                }else{
                    rgb = vec4_to_rgba(mat.at<cv::Vec4b>(r,c));
                }
            }else{
                if(is_float) {
                    rgb = vec3_to_rgb(mat.at<cv::Vec3f>(r,c)*255);
                }else{
                    rgb = vec3_to_rgb(mat.at<cv::Vec3b>(r,c));
                }
            }
            image.setPixel(c,r,rgb);
        }
    }
    return image;
}

cv::Mat to_cv8u_image(const cv::Mat &src) {
    cv::Mat dst = src.clone();
    if(src.channels() == 3) {
        cv::cvtColor(src,dst,CV_BGR2GRAY);
    }
    if(dst.type() != CV_8U) {
        cv::Mat(dst*255.0).convertTo(dst,CV_8U);
    }
    return dst;
}
