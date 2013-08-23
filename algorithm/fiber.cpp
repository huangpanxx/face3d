#include "fiber.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "common.h"
#include <QDebug>


namespace fiber {

Kernel::Kernel(float sigma_x, float sigma_y, float lambda, int n ,int kernel_size)
{
    this->sigma_x = sigma_x;
    this->sigma_y = sigma_y;
    this->n = n;
    this->lambda = lambda;
    this->buildKernels(kernel_size,sigma_x,sigma_y,lambda,n);
}

void Kernel::buildKernels(int kernel_size,float sigma_x, float sigma_y, float lambda, int n) {
    float sigma = sigma_x;
    float gamma = sigma_x/sigma_y;
    this->thetas.reserve(n);
    for(int i=0;i<n;++i) {
        float theta = CV_PI*i/n;
        cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size,kernel_size),
                                            sigma,theta,lambda,gamma,0);
        this->thetas.push_back(theta);
        this->kernels.push_back(kernel);
    }
}

void filterForDense(const Kernel& kernel, const cv::Mat &input,cv::Mat& resp,cv::Mat &theta,float confLow)  {
    cv::Mat src = normalize(input);
    cv::Mat _resp = cv::Mat(src.rows,src.cols,CV_32F);
    theta = cv::Mat(src.rows,src.cols,CV_32F);
    std::vector<cv::Mat> vec; vec.reserve(kernel.size());
    for(int i=0;i<kernel.size();++i) {
        cv::Mat dst;
        cv::Mat k = kernel.kernel(i);
        cv::filter2D(src,dst,-1,k);
        vec.push_back(dst);
    }

    for(int r=0;r<src.rows;++r) {
        for(int c=0;c<src.cols;++c) {
            float maxresp = -1e20; int respidx=-1;
            for(int i=0;i<kernel.size();++i) {
                float val = vec[i].at<float>(r,c);
                if(val>maxresp) {
                    maxresp =val; respidx = i;
                }
            }
            if(respidx==-1)
                qDebug()<<vec[0].at<float>(r,c)<<endl;
            assert(respidx != -1);
            _resp.at<float>(r,c) = maxresp;
            theta.at<float>(r,c) = kernel.theta(respidx);
        }
    }

    if(confLow != 0) {
        for(int r=0;r<src.rows;++r) {
            for(int c=0;c<src.cols;++c) {
                float maxresp = _resp.at<float>(r,c);
                float t = theta.at<float>(r,c);
                float conf = 0;
                for(int i=0;i<kernel.size();++i) {
                    float val = vec[i].at<float>(r,c);
                    float d = minAngle(kernel.theta(i),t);
                    conf += sqrt(d)*(maxresp - val);
                }
                if(conf<=confLow) {
                    _resp.at<float>(r,c)  = 0;
                }
            }
        }
    }
    resp = normalize(_resp);
}

void simpleFilter(const Kernel& kernel,const cv::Mat &src,
                  cv::Mat& resp,cv::Mat &theta)  {
    cv::Mat _resp = cv::Mat(src.rows,src.cols,CV_32F);
    theta = cv::Mat(src.rows,src.cols,CV_32F);
    std::vector<cv::Mat> vec; vec.reserve(kernel.size());
    for(int i=0;i<kernel.size();++i) {
        cv::Mat dst;
        cv::Mat k = kernel.kernel(i);
        cv::filter2D(src,dst,-1,k);
        vec.push_back(dst);
    }
    for(int r=0;r<src.rows;++r) {
        for(int c=0;c<src.cols;++c) {
            float maxresp = -1e9; int respidx=-1;
            for(int i=0;i<kernel.size();++i) {
                float val = vec[i].at<float>(r,c);
                if(val>maxresp) {
                    maxresp =val; respidx = i;
                }
            }
            assert(respidx != -1);
            _resp.at<float>(r,c) = maxresp;
            theta.at<float>(r,c) = kernel.theta(respidx);
        }
    }
    resp = _resp;
}

void combineMax(const cv::Mat &resp1, const cv::Mat& resp2,
                const cv::Mat &theta1, const cv::Mat &theta2,
                cv::Mat &resp, cv::Mat &theta) {
    const int rows = resp1.rows;
    const int cols = resp1.cols;
    theta = cv::Mat(rows,cols,CV_32F);
    resp = cv::Mat(rows,cols,CV_32F);

    for(int r=0;r<rows;++r) {
        for(int c=0;c<cols;++c) {
            float v1 = resp1.at<float>(r,c);
            float v2 = resp2.at<float>(r,c);
            float v,t;
            if(v1>v2) {  v = v1; t = theta1.at<float>(r,c);  }
            else      {  v = v2; t = theta2.at<float>(r,c); }
            resp.at<float>(r,c) = v;
            theta.at<float>(r,c) = t;
        }
    }
}

void filterForSparse(const Kernel& kernel,
                     const cv::Mat &src,cv::Mat& resp,
                     cv::Mat &theta) {
    cv::Mat s = normalize(src,1);
    cv::Mat r1,r2,t1,t2;
    simpleFilter(kernel,0.5-s,r1,t1);
    simpleFilter(kernel,s-0.5,r2,t2);
    cv::Mat _resp;
    combineMax(r1,r2,t1,t2,_resp,theta);
    resp = _resp;
}

cv::Vec3f theta2Color(float theta, float light=0.6) {
    theta = theta - floor(theta/CV_PI)*CV_PI;
    if(theta<0) theta += CV_PI;
    int rr,gg,bb;
    hsl_to_rgb(theta/CV_PI,0.7,0.5,rr,gg,bb);
    return cv::Vec3f(range(0,bb*light,255),
                     range(0,gg*light,255),
                     range(0,rr*light,255))/255;
}

cv::Mat nonMaximaSuppression(const cv::Mat &input,const cv::Mat& thetas,
                             int ridgeOffsetStart,int ridgeOffsetLen,
                             float filterOffset,float hysteresisLow
                             ) {
    assert(ridgeOffsetLen>0);
    assert(ridgeOffsetStart >= 1);

    cv::Mat src = normalize(input,1);
    cv::Mat dst(src.rows,src.cols,CV_32F);  dst.setTo(cv::Scalar());

    const int ridgeOffsetEnd = ridgeOffsetStart+ridgeOffsetLen;

    for(int x=ridgeOffsetEnd;x<src.cols-ridgeOffsetEnd;++x) {
        for(int y=ridgeOffsetEnd;y<src.rows-ridgeOffsetEnd;++y) {
            float theta = thetas.at<float>(y,x);

            float ox = cos(theta), oy = sin(theta);

            float v1=-1e9,v2=-1e9;
            for(int i=ridgeOffsetStart;i<=ridgeOffsetEnd;++i) {
                float oxx = i*ox, oyy = i*oy;
                float vv1 = interpolate(src,y+oyy,x+oxx);
                if(v1<vv1) {  v1 = vv1; }
                float vv2 = interpolate(src,y-oyy,x-oxx);
                if(v2<vv2) {  v2 = vv2; }
            }
            float v = src.at<float>(y,x);
            bool hasMax = (v>=v1) && (v>=v2);
            float surroundMax = fmax(v1, v2);
            float relVal = (v-surroundMax) /
                    (surroundMax+filterOffset);

            if (!hasMax || relVal <= hysteresisLow) {
                relVal = 0;
            }
            dst.at<float>(y,x) = relVal;

        }
    }
    return normalize(dst,1);
}

cv::Mat theta2Color(const cv::Mat &thetas,float lightRatio) {
    cv::Mat om(thetas.rows,thetas.cols,CV_32FC3);
    for(int x=0;x<thetas.cols;++x) {
        for(int y=0;y<thetas.rows;++y) {
            float theta = thetas.at<float>(y,x);
            om.at<cv::Vec3f>(y,x) = theta2Color(theta,range(lightRatio,0,1));
        }
    }
    return om;
}

cv::Mat buildOrientationMap(const cv::Mat &resp, const cv::Mat &thetas,const float lightRatio) {
    cv::Mat om(resp.rows,resp.cols,CV_32FC3);
    cv::Mat nr = normalize(resp,1);
    for(int x=0;x<resp.cols;++x) {
        for(int y=0;y<resp.rows;++y) {
            float theta = thetas.at<float>(y,x);
            float l = nr.at<float>(y,x);
            om.at<cv::Vec3f>(y,x) = theta2Color(theta,(float)range(lightRatio*l,0,1));
        }
    }
    return om;
}

cv::Mat buildColorStrip(int width,int height) {
    assert(width>0 && height>0);
    cv::Mat m(height,width,CV_32FC3);
    for(int i=0;i<width;++i) {
        float theta = (CV_PI*i)/width;
        cv::Vec3f color = theta2Color(theta,0.5f);
        for(int j=0;j<height;++j){
            m.at<cv::Vec3f>(j,i) = color;
        }
    }
    return m;
}

cv::Mat visualizeKernel(const cv::Mat &kernel) {
    float rate = fmin(300.0/kernel.cols,300.0/kernel.rows);
    cv::Mat v;
    int r = rate*kernel.rows, c = rate*kernel.cols;
    cv::resize(kernel,v,cv::Size(r,c));
    return normalize(v,1);
}


cv::Mat BGRToGray(const cv::Mat &src) {
    cv::Mat udst,fdst;
    cv::cvtColor(src,udst,CV_BGR2GRAY);
    udst.convertTo(fdst,CV_32F);
    return fdst;
}

cv::Mat gaussBlur(const cv::Mat &src,int sz,float sigma) {
    cv::Mat dst;
    cv::GaussianBlur(src,dst,cv::Size(sz,sz),sigma);
    return dst;
}

float minAngle(float t1,float t2) {
    const float P = CV_PI;
    t1 = t1 - floor(t1/(P))*P;
    if(t1<0) t1+=P;
    t2 = t2 - floor(t2/(P))*P;
    if(t2<0) t2+=P;
    float dt = fabs(t1-t2);
    return fmin(dt,fabs(CV_PI-dt));
}

cv::Mat powEnhence(const cv::Mat &input,float coef) {
    cv::Mat src = input.clone();
    for(int c=0;c<src.cols;++c) {
        for(int r=0;r<src.rows;++r) {
            src.at<float>(r,c) = pow(src.at<float>(r,c),coef);
      }
    }
    return normalize(src);
}

cv::Mat localEnhence(const cv::Mat &input,int sz,float offset,float low) {
    assert(sz>=3 && sz%2==1);
    cv::Mat dst = cv::Mat::zeros(input.rows,input.cols,CV_32F);
    cv::Mat src = normalize(input,1);

    for(int c=sz;c<src.cols-sz;++c) {
        for(int r=sz;r<src.rows-sz;++r) {
            float mmax = -1e20, mmin=1e20,s=0;
            for(int cc=c-sz;cc<=c+sz;++cc) {
                for(int rr=r-sz;rr<=r+sz;++rr) {
                    float v = src.at<float>(rr,cc);
                    mmax = fmax(mmax,v);
                    mmin = fmin(mmin,v);
                    s+=v;
                }
            }
            float v = src.at<float>(r,c);
            if(is_equal(mmax,mmin) || v<low) {
                v = 0;
            }else {
                v = (v-mmin)/(mmax-mmin+offset);
            }
            dst.at<float>(r,c) = v;
        }
    }
    return normalize(dst);
}


cv::Mat mixImage(const cv::Mat &bgrsrc, const cv::Mat &resp,const cv::Mat &theta,float lightRatio) {
    assert(bgrsrc.type()==CV_32FC3);
    assert(resp.type()==CV_32F);

    cv::Mat dst(bgrsrc.rows,bgrsrc.cols,CV_32FC3);
    cv::Mat nr = normalize(resp);

    for(int c=0;c<bgrsrc.cols;++c) {
        for(int r=0;r<bgrsrc.rows;++r) {
            float v = nr.at<float>(r,c);
            cv::Vec3f c1 = bgrsrc.at<cv::Vec3f>(r,c);
            cv::Vec3f c2 = theta2Color(theta.at<float>(r,c),1);
            v = range(v*lightRatio,0,1);
            cv::Vec3f color = c1*(1-v) + v*c2;
            dst.at<cv::Vec3f>(r,c) = color;
        }
    }
    return dst;
}

cv::Mat splitImage(const cv::Mat &bgra_255, cv::Mat& alpha, cv::Mat& gray) {
    assert(bgra_255.channels() == 4);

    std::vector<cv::Mat> channels;
    cv::split(bgra_255,channels);

    for(uint i=0;i<channels.size();++i) {
        channels[i] = to_float_mat(channels[i])/255.0f;
    }

    alpha = channels[3];

    cv::Mat dst;
    cv::merge(std::vector<cv::Mat>(channels.begin(),channels.begin() + 3),dst);
    cv::cvtColor(dst,gray,CV_BGR2GRAY);

    return dst;
}


cv::Mat applyAlpha(const cv::Mat &src,const cv::Mat &alpha) {
    assert(alpha.type() == CV_32F);
    assert(src.type() == CV_32F || src.type() == CV_32FC3);

    if(src.channels()==1) return src.mul(alpha);

    std::vector<cv::Mat> channels;
    cv::split(src,channels);
    for(uint i=0;i<channels.size();++i) {
        channels[i] = channels[i].mul(alpha);
    }

    cv::Mat dst; cv::merge(channels,dst);
    return dst;
}

bool inline is_seed(float v,float s,float high,float e) {
    return v>=high && s>e;
}

std::vector<std::vector<Point> > traceFibers(const cv::Mat &_rsp, const cv::Mat &_sup,
                                                    const cv::Mat &theta, float hysteresisLow,
                                                    float hysteresisHigh,
                                                    float e, float thetaMax,
                                                    float sampleStep, float removeRadius,
                                                    int healthMax, float traceStep) {

    cv::Mat rsp = normalize(_rsp.clone());
    cv::Mat sup = normalize(_sup.clone());

    std::vector <std::vector< Point> > fibers;


    for(int r=0;r<rsp.rows;++r) {
        for(int c=0;c<rsp.cols;++c) {
            float v = rsp.at<float>(r,c);
            float s = sup.at<float>(r,c);
            if(is_seed(v,s,hysteresisHigh,e)) {
                std::vector<Point> fiber
                        = traceFiber(c,r,rsp,sup,theta,hysteresisLow,thetaMax,sampleStep,removeRadius,healthMax,traceStep);
                fibers.push_back(fiber);
            }
        }
    }

    return fibers;
}

cv::Point2f correctPoint(float x,float y,float theta,
                         const cv::Mat &rsp,const float step) {
    float ox = step*cos(theta), oy = step*sin(theta);
    float v1 = fabs(rsp.at<float>(y+oy,x+ox));
    float v2 = fabs(rsp.at<float>(y-oy,x-ox));
    float v  = fabs(rsp.at<float>(y,x));



    if(v<=v1 || v<=v2) {
        if(v1>v+0.01 && v1>v2) {  return cv::Point2f(x+ox,y+oy); }
        if(v2>v+0.01 && v2>v1) {  return cv::Point2f(x-ox,y-oy); }
        return cv::Point2f(x,y);
    }

    float vmin = fmin(v1,v2);
    v -= vmin; v1 -= vmin; v2 -= vmin;
    if(v < EPS) {  return cv::Point2f(x,y); }

    float p = (fmax(v1,v2)/v)/2;
    float nx = x, ny = y;
    if(v1>v2) { nx += p*ox; ny += p*oy; }
    else{ nx -= p*ox; ny -= p*oy; }
    return cv::Point2f(nx,ny);

}

std::vector< Point >
traceFiber(float x, float y, cv::Mat &rsp, cv::Mat &sup, const cv::Mat &theta,
           float hysteresisLow, float theta_max,bool orientation,float sampleStep, float removeRadius,
           const int healthMax,float traceStep) {

    assert(healthMax>0);
    std::vector<Point> fiber;
    std::vector<float> vr;

    rsp.at<float>(y,x) *= -1;


    float last_x = x,last_y = y;
    float angle = theta.at<float>(y,x) + (orientation ? 0 : CV_PI);
    const float step = traceStep;
    int health = healthMax;

    fiber.push_back(
                Point(
                    x,y,angle,fabs(sup.at<float>(y,x)),
                    fabs(rsp.at<float>(y,x)),false));
    bool certain = true;
    while(true) {
        bool occlude = false;

        //next point
        float oxx = -step*sin(angle), oyy = step*cos(angle);
        float xx = last_x + oxx,yy = last_y+oyy;

        //boundary
        if(xx<1 || xx>rsp.cols-2 || yy<1 || yy>rsp.rows-2)
            break;

        //correct
        cv::Point2f p = correctPoint(xx,yy,angle,rsp,sampleStep);
        xx = p.x; yy = p.y;

        //boundary
        if(xx<1 || xx>rsp.cols-2 || yy<1 || yy>rsp.rows-2) break;

        //rsp theta
        float w = rsp.at<float>(yy,xx),
                t = theta.at<float>(yy,xx);

        float dangle = minAngle(angle,t);

        //status
        if(w < hysteresisLow) {
            certain = false;
            if(dangle>theta_max && fabs(w) > hysteresisLow) {
                occlude = true;
            }
        } else if(dangle > theta_max) {
            certain = false;
            occlude = true;
        } else {
            certain = true;
            health = healthMax;
            if(fabs(t-angle)>CV_PI/2 && fabs(t-angle)<CV_PI*3/2) {
                angle = t  + CV_PI;
            }else{
                angle = t;
            }
        }
        //remove
        vr.push_back(rsp.at<float>(yy,xx));
        rsp.at<float>(yy,xx) = -rsp.at<float>(yy,xx);

        //update
        last_x = xx; last_y = yy;
        Point pt(xx,yy,angle,fabs(sup.at<float>(yy,xx)),fabs(w),occlude);
        Point &pto = fiber.back();
        pto.theta = fiber::angle(pto,pt);
        fiber.push_back(pt);

        health -= 1;
        if(health == 0) break;
    }


    //size of droped pixels
    int uncertainSize = healthMax-health;
    //recover droped pixel
    for(uint i=vr.size()-uncertainSize;i<vr.size();++i) {
        Point p = fiber[i];
        rsp.at<float>(p.y,p.x) = vr[i];
    }

    //drop
    fiber = std::vector<Point>(
                fiber.begin(),
                fiber.end()-uncertainSize);

    //remove
    for(uint i=0;i<fiber.size();++i) {
        float xx = fiber[i].x,yy=fiber[i].y;
        for(int dx=-removeRadius;dx<=removeRadius;++dx) {
            for(int dy=-removeRadius;dy<=removeRadius;++dy) {
                if(distance(dx,dy) <= removeRadius
                        && is_in_range(yy+dy,0,sup.rows-1)
                        && is_in_range(xx+dx,0,sup.cols-1)) {
                    sup.at<float>(yy+dy,xx+dx) *= -1;
                    rsp.at<float>(yy+dy,xx+dx) *= -1;
                }
            }
        }
    }

    return fiber;
}

std::vector<Point> traceFiber(float x, float y, cv::Mat &rsp, cv::Mat &sup, const cv::Mat &theta,
                              float hysteresisLow, float thetaMax, float sampleStep, float removeRadius, int healthMax, float traceStep){

    std::vector<Point> fiber1 =
            traceFiber(x,y,rsp,sup,theta,hysteresisLow,thetaMax,true,sampleStep,removeRadius,healthMax,traceStep);
    std::vector<Point> fiber2 =
            traceFiber(x,y,rsp,sup,theta,hysteresisLow,thetaMax,false,sampleStep,removeRadius,healthMax,traceStep);


    std::vector<Point> fiber; fiber.reserve(fiber1.size()+fiber2.size());

    for(std::vector<Point>::reverse_iterator it = fiber2.rbegin();
        it!=fiber2.rend();++it) {
        fiber.push_back(*it);
    }

    for(std::vector<Point>::iterator it = fiber1.begin();
        it!=fiber1.end();++it) {
        fiber.push_back(*it);
    }
    return fiber;
}


std::vector<std::vector<Point> >
smoothFibers(const std::vector<std::vector<Point> > &fibers,
             int iter,float dt) {
    std::vector<std::vector<Point> > smoothedFibers;
    smoothedFibers.reserve(fibers.size());
    for(uint i=0;i<fibers.size();++i) {
        smoothedFibers.push_back(
                    smoothFiber(fibers[i],iter,dt));
    }
    return smoothedFibers;
}

std::vector<Point>
smoothFiber(const std::vector<Point> &fiber,
            int iter,float dt) {

    std::vector<Point> fiberToSmooth = fiber;

    for(int i=0;i<iter;++i) {
        std::vector<Point> smoothedFiber;
        smoothedFiber.reserve(fiberToSmooth.size());

        for(int j=0;j<(int)fiber.size();++j) {
            Point p = fiberToSmooth.at(j);
            float sx=0, sy=0; int cnt = 0;
            for(int k=0;k<(int)fiberToSmooth.size();++k) {
                bool flag = true;

                if(j-k>=0) {
                    Point p1 = fiberToSmooth[j-k];
                    if(distance(p1.x-p.x,p1.y-p.y)<=dt) {
                        sx += p1.x; sy += p1.y; cnt += 1; flag = false;
                    }
                }
                if(j+k<(int)fiberToSmooth.size()) {
                    Point p2 = fiberToSmooth[j+k];
                    if(distance(p2.x-p.x,p2.y-p.y)<=dt) {
                        sx += p2.x; sy += p2.y; cnt += 1; flag = false;
                    }
                }
                if(flag) break;
            }
            p.x = sx/cnt; p.y = sy/cnt;
            smoothedFiber.push_back(p);
        }
        fiberToSmooth = smoothedFiber;
    }

    for(uint i=1;i<fiberToSmooth.size();++i) {
        Point &p = fiberToSmooth[i-1];
        p.theta = angle(p,fiberToSmooth[i]);
    }

    return fiberToSmooth;
}

float angle(const Point &from,const Point &to) {
    float dx = to.x - from.x, dy = to.y - from.y;
    if(distance(dx,dy) == 0) return 0;
    float a =  acos(dx/distance(dx,dy));
    if(dy<0)  a = 2*CV_PI-a;
    return a;
}

std::vector<std::vector<Point> >
uniquePoints(const std::vector<std::vector<Point> > &points,float dt) {
    std::vector<std::vector<Point> > filtedPoints;
    filtedPoints.reserve(points.size());
    for(uint i=0;i<points.size();++i) {
        filtedPoints.push_back(uniquePoints(points[i],dt));
    }
    return filtedPoints;
}



std::vector<Point>
uniquePoints(const std::vector<Point> &points,float dt) {
    std::vector<Point> set = points;
    std::vector<Point>::iterator it_end =
            std::unique(set.begin(),set.end(),point_equal_comparater_2d<Point>(dt));
    return std::vector<Point>(set.begin(),it_end);

}
}
