//#include "fibertest.h"
//#include "algorithm/fiber.h"
//#include "opencv2/highgui/highgui.hpp"
//#include <QDebug>
//#include "algorithm/common.h"

//void fiber_detect(const std::string &filename) {

//    //read and preprocess
//    cv::Mat bgra = cv::imread(filename,-1),bgr,alpha,gray;
//    bgr = fiber::splitImage(bgra,alpha,gray);
//    bgr = fiber::applyAlpha(bgr,alpha);
//    cv::Mat src = fiber::gaussBlur(gray,3,1);

//    //filter
//    fiber::Kernel kernel(3,3,7,30);
//    cv::Mat rsp,theta;    //resp,theta
//    for(int i=0;i<3;++i) {
//        qDebug()<<"filter:\t"<<i<<"\ttime(s)";
//        src = fiber::applyAlpha(src,alpha);
//        fiber::filterForDense(kernel,src,rsp,theta);
//        if(i==1)
//            rsp = fiber::localEnhence(rsp,3,0.05,0);
//        src = rsp;
//    }
//    //supression
//    rsp = normalize(fiber::applyAlpha(rsp,alpha));
//    cv::Mat sup = fiber::nonMaximaSuppression(rsp,theta,2,2,0.1,0.0);

//    //color image
//    cv::Mat imr  = fiber::mixImage(bgr,rsp,theta,1.5);
//    cv::Mat ims  = fiber::mixImage(bgr,sup,theta,1.5);

//    //show
//    cv::imshow("response",imr);
//    cv::imshow("supression",ims);
//    cv::imshow("theta",fiber::theta2Color(theta));
//    cv::imshow("color",fiber::buildColorStrip());

//    qDebug()<<"trace";
//    //trace
//    std::vector<std::vector<fiber::Point> > fibers =
//            fiber::traceFibers(sup,sup,theta,
//                                      0.1,//low
//                                      0.2,//high
//                                      0.2,//sup
//                                      CV_PI/4,1,2,20);

//    fibers = fiber::smoothFibers(fibers,5,5);
//    fibers = fiber::uniquePoints(fibers,3);

//    for(uint i=0;i<fibers.size();++i) {
//        std::vector<fiber::Point> fiber = fibers[i];
//        if(fiber.size()< 15) continue;

//        for(uint j=1;j<fiber.size();++j) {
//            fiber::Point pt = fiber[j];
//            fiber::Point p = fiber[j-1];
//            float rate = 1.0*j/fiber.size();
//            cv::Vec3f color = fiber::theta2Color(pt.theta,range(rate/5+0.8,0,1));
//            cv::Scalar theta_colar = cv::Scalar(color(0),color(1),color(2));
//            cv::Scalar fiber_color = cv::Scalar(rate,1-rate,125);
//            cv::line(bgr,cv::Point(p.x,p.y),cv::Point(pt.x,pt.y),
//                       fiber_color,
//                       2);
//        }
//    }
//    cv::imshow("bgr",bgr);

//}

