#ifndef FIBER_H
#define FIBER_H

#include <opencv2/core/core.hpp>
#include <vector>

namespace fiber {
class Kernel
{
public:
    Kernel(float sigma_x, float sigma_y, float lambda, int n, int kernel_size=-1);
    float theta(int i) const { return this->thetas[i]; }
    cv::Mat kernel(int i) const { return this->kernels[i]; }
    int size() const { return n; }
private:
    void buildKernels(int kernel_size,float sigma_x,float sigma_y,float lambda,int n);
    std::vector< cv::Mat > kernels;
    std::vector< float > thetas;
    float sigma_x,sigma_y;
    float lambda;
    int n;
};

class Point {
public:
    inline Point(float x,float y,float theta,float sup,float rsp,bool occlude) {
        this->x = x;this->y = y;
        this->theta = theta;this->occlude = occlude;
        this->sup = sup;this->rsp = rsp;
    }
    float x,y,theta,occlude,sup,rsp;
};


//filter
void filterForDense(const Kernel& kernel,const cv::Mat &src, cv::Mat& resp, cv::Mat &theta, float confLow = 0) ;
void filterForSparse(const Kernel& kernel,const cv::Mat &src,cv::Mat& resp,cv::Mat &theta) ;
void simpleFilter(const Kernel& kernel,const cv::Mat &src,cv::Mat& resp,cv::Mat &theta) ;

//trace
std::vector <std::vector<Point > >
traceFibers(const cv::Mat &response,
            const cv::Mat &supreession,
            const cv::Mat &theta,
            float hysteresisLow, float hysteresisHigh,
            float e, float thetaMax, float sampleStep=1,
            float removeRadius=2,int healthMax=5,float traceStep=0.75);

std::vector<Point >
traceFiber(float x, float y, cv::Mat &rsp,cv::Mat&sup, const cv::Mat &theta,
           float hysteresisLow, float thetaMax,float sampleStep, float removeRadius,
           int healthMax,float traceStep);

std::vector< Point >
traceFiber(float x, float y, cv::Mat &rsp, cv::Mat &sup, const cv::Mat &theta,
           float hysteresisLow, float thetaMax,bool orientation,float sampleStep,
           float removeRadius,int healthMax,float traceStep);

cv::Point2f
correctPoint(float x,float y,float theta,const cv::Mat &rsp,const float sampleStep);

//pre process
cv::Mat BGRToGray(const cv::Mat &src);
cv::Mat gaussBlur(const cv::Mat &src, int sz, float sigma);
cv::Mat localEnhence(const cv::Mat &src, int sz = 3, float offset=0.05, float low=0);
cv::Mat powEnhence(const cv::Mat &src,float coef);

//post process
cv::Mat
nonMaximaSuppression(const cv::Mat &src, const cv::Mat& thetas,
                     int ridgeOffsetStart = 2, int ridgeOffsetLen = 10,
                     float filterOffset = 1, float hysteresisLow = 0.001);

std::vector<std::vector<Point> >
smoothFibers(const std::vector<std::vector<Point> > &fibers,int iter,float dt);
std::vector<Point>
smoothFiber(const std::vector<Point> &fiber, int iter,float dt);

std::vector<std::vector<Point> >
uniquePoints(const std::vector<std::vector<Point> > &points,float dt);

std::vector<Point>
uniquePoints(const std::vector<Point> &points,float dt);



//helper
cv::Vec3f theta2Color(float theta,float light);
cv::Mat theta2Color(const cv::Mat &thetas,float lightRatio = 0.6f);

void combineMax(const cv::Mat &resp1,const cv::Mat &resp2,
                const cv::Mat &theta1,const cv::Mat &theta2,
                cv::Mat &resp,cv::Mat &theta);
float minAngle(float t1,float t2);
cv::Mat mixImage(const cv::Mat &bgrsrc, const cv::Mat &resp, const cv::Mat &theta,float lightRatio=1.0f);



cv::Mat splitImage(const cv::Mat &bgra_255, cv::Mat& alpha, cv::Mat& gray);
cv::Mat applyAlpha(const cv::Mat &src,const cv::Mat &alpha);

float angle(const Point &from, const Point &to);

//visual
cv::Mat buildOrientationMap(const cv::Mat &resp, const cv::Mat& thetas, const float lightRatio=1.0f);
cv::Mat buildColorStrip(int width=360, int height=50);
cv::Mat visualizeKernel(const cv::Mat &kernel);

}

#endif // FIBER_H
