#include "matting.h"

#include <QImage>
#include <QDebug>
#include <algorithm>

#include <cmath>
#include <fstream>
#include "assert.h"
#include "common.h"

using namespace std;

namespace matting {


class ImageHelper{
public:
    ImageHelper(const QImage &image);
    ImageHelper(int w,int h);
    ~ImageHelper();
    int width() const;
    int height() const;
    int size() const;

    void setPixel(int x,int y,int r,int g,int b);

    void pixel(int x,int y,int out_color[3]) const;
    void pixel(int index,int out_color[3]) const;
    void pixel(int wx,int wy,int w,int i,int out_color[3]) const; //计算窗口w第i个元素的颜色

    int index(int x,int y) const;
    int index(int wx,int wy,int w, int i) const;    //计算窗口w第i个元素的序号

    int index_in_window(int wx,int wy,int w,int x,int y) const; //计算坐标在某个窗口的序号

    bool is_color(int x, int y, QRgb color) const;
    void mean(int wx,int wy,int w,double out_p[3]) const;
    void covariance(int wx, int wy, int w, double out_covariance[9], double tmp[]) const; //tmp长度应为(2w+1)^2 * 3
    void coff(int wx,int wy,int w,double out_coff[9],double tmp[], double e) const;
    QImage toImage() const;

private:
    void init(int w,int h,bool clear = false);
    int *_pGray;
    int _width;
    int _height;
};

ImageHelper::ImageHelper(const QImage &image) {
    this->init(image.width(),image.height());
    for(int x=0;x<_width;++x) {
        for(int y=0;y<_height;++y) {
            int index = this->index(x,y)*3;
            QRgb rgb = image.pixel(x,y);
            _pGray[index+0] = qRed(rgb);
            _pGray[index+1] = qGreen(rgb);
            _pGray[index+2] = qBlue(rgb);
        }
    }
}

void ImageHelper::init(int w,int h,bool clear) {
    this->_width = w; this->_height = h;
    const int sz = this->_width*this->_height*3;
    _pGray = new int[sz];
    if(clear) {
        memset(_pGray,0,sz*sizeof(int));
    }
}

ImageHelper::ImageHelper(int w,int h) {
    this->init(w,h,true);
}

bool ImageHelper::is_color(int x, int y, QRgb color) const{
    int c[3];   this->pixel(x,y,c);
    bool b =   std::abs(c[0]-qRed(color))   < 40
            && std::abs(c[1]-qGreen(color)) < 40
            && std::abs(c[2]-qBlue(color))  < 40;
    return b;
}

void ImageHelper::mean(int wx, int wy, int w, double out_p[3]) const {
    zeros(out_p,3);
    int l = 2*w+1; double s = l*l; int color[3];
    for(int x=wx-w;x<=wx+w;++x) {
        for(int y=wy-w;y<=wy+w;++y) {
            pixel(x,y,color);
            for(int i=0;i<3;++i)
                out_p[i] += color[i];
        }
    }
    for(int i=0;i<3;++i)
        out_p[i] /= s;
}

int ImageHelper::index_in_window(int wx,int wy,int w,int x,int y) const{
    return (y+w-wy)*(2*w+1)+x+w-wx;
}


void ImageHelper::covariance(int wx, int wy, int w, double out_covariance[9], double tmp[]) const {
    double _mean[3];   int c[3];
    mean(wx,wy,w,_mean); double l = 2*w+1;
    for(int x=wx-w;x<=wx+w;++x) {
        for(int y=wy-w;y<=wy+w;++y) {
            pixel(x,y,c);
            int idx = index_in_window(wx,wy,w,x,y);
            for(int i=0;i<3;++i){
                tmp[idx+i*9] = c[i]-_mean[i];
            }
        }
    }
    maat(tmp,out_covariance,3,9);
    for(int i=0;i<9;++i)
        out_covariance [i] /= l*l;
}

int ImageHelper::size() const {
    return _width * _height;
}

void ImageHelper::pixel(int index, int out_color[3]) const {
    for(int i=0;i<3;++i) out_color[i] = _pGray[index*3+i];
}
int ImageHelper::index(int x, int y) const {
    return y*_width + x;
}

void ImageHelper::pixel(int x, int y,int out_color[3]) const {
    int index = this->index(x,y);
    pixel(index,out_color);
}

int ImageHelper::width() const {
    return _width;
}
int ImageHelper::height() const {
    return _height;
}

int ImageHelper::index(int wx,int wy,int w, int i) const {
    int l = 2*w+1;
    int x = i%l - w + wx; int y = i/l - w + wy;
    return index(x,y);
}


void ImageHelper::pixel(int wx, int wy, int w, int i, int out_color[3]) const {
    int index = this->index(wx,wy,w,i);
    this->pixel(index,out_color);
}

void ImageHelper::coff(int wx, int wy, int w, double out_coff[9], double tmp[], double e) const {
    double _covariance[9]; int wk = square(2*w+1);
    this->covariance(wx,wy,w,_covariance,tmp);
    for(int i=0;i<3;++i) _covariance[i*3+i] += e/wk;
    gauss_elim(_covariance,out_coff,3);
}


ImageHelper::~ImageHelper() {
    delete[] _pGray;
}


QImage ImageHelper::toImage() const {
    QImage img(this->width(),this->height(),QImage::Format_ARGB32);
    for(int x = 0;x<this->_width;++x) {
        for(int y=0;y<this->_height;++y) {
            int index = this->index(x,y)*3;
            int rgb = qRgb(_pGray[index+0],_pGray[index+1],_pGray[index+2]);
            img.setPixel(x,y,rgb);
        }
    }
    return img;
}
void ImageHelper::setPixel(int x,int y,int r,int g,int b) {
    int idx = this->index(x,y)*3;
    this->_pGray[idx+0] = r;
    this->_pGray[idx+1] = g;
    this->_pGray[idx+2] = b;
}



double getWindowMatrixEntry(int vp1[3],int vp2[3],double wk, double delta,
                            double mean[3],double coff[9]) {
    double t[3] = {0,0,0};
    for(int i=0;i<3;++i) {
        double s = 0;
        for(int j=0;j<3;++j) {
            s += (vp1[j]-mean[j])*coff[j*3+i];
        }
        t[i] = s;
    }
    double s = 0;
    for(int i=0;i<3;++i) {
        s += t[i]*(vp2[i]-mean[i]);
    }

    return delta - (1+s)/wk;
}

/*从alpha数组中构建图片*/
QImage imageFromAlpha(int width,int height,double *pAlpha) {

    QImage image = QImage(width,height,QImage::Format_ARGB32);
    for(int x=0;x<width;++x) {
        for(int y=0;y<height;++y) {
            double alpha = pAlpha[y*width+x];
            double v = alpha*255;
            v = std::min(std::max(v,0.0),255.0);


            image.setPixel(x,y,qRgb(v,v,v));
        }
    }
    return image;
}

/*计算(x,y)坐标*/
void fillPxPy(const ImageHelper &image, int w,int *px,int *py,double *pv) {
    const int width = image.width();    const int height = image.height();
    const int l = 1+4*w;                const int S = l*l;
    const int N = width*height;         const int nz = N*S;
    for(int n=0;n<nz;++n) {
        int i=n/S; px[n] = i;                   //p1序号
        int x_p1 = i%width; int y_p1 = i/width; //p1坐标
        int idx = n%S;                          //p2在p1窗口中序号
        int x_p2 = idx%l-2*w+x_p1; int y_p2 = idx/l-2*w+y_p1; //p2序号
        if(x_p2<0 || x_p2 >=width || y_p2 < 0 || y_p2 >= height) {
            px[n] = py[n] =pv[n] = -1;
        } else {
            py[n] = image.index(x_p2,y_p2);
        }
    }
}

/* 遍历窗口 */
void scanWindow(const ImageHelper&image,int w,double e,double *pv) {
    const int wl = 1+2*w;   //窗口边长
    const int wk = wl*wl;   //点数量
    const int width = image.width();    const int height = image.height();
    const int l = 1 + 4*w;   //与某点同一窗口的点构成的矩形边长
    const int S = l * l; //该矩形元素数量
    double *tmp = new double[S*3];

    for(int wy = w; wy < height-w; wy+=1) {
        for(int wx = w; wx < width-w; wx+=1) {
            double mean[3]; image.mean(wx,wy,w,mean); //平均值

            double coff[9];
            image.coff(wx,wy,w,coff,tmp,e); //系数


            //计算w窗口矩阵(遍历点p1 p1)
            for(int p1=0;p1<wk;++p1) {
                for(int p2=0;p2<wk;++p2) {
                    int delta = p1==p2 ? 1:0;
                    int vp1[3],vp2[3];
                    image.pixel(wx,wy,w,p1,vp1);   image.pixel(wx,wy,w,p2,vp2); //像素点

                    double v = getWindowMatrixEntry(vp1,vp2,wk,delta,mean,coff);
                    int index_p1 = image.index(wx,wy,w,p1); //p1在图像中的位置
                    int index_p2 = image.index(wx,wy,w,p2); //p2在图像中的位置
                    int x_p1 = index_p1 % width; int y_p1 = index_p1 / width;  //p1图像坐标
                    int x_p2 = index_p2 % width; int y_p2 = index_p2 / width;  //p2图像坐标
                    int index_w_p1 = (y_p2-y_p1+2*w)*l + (x_p2-x_p1+2*w); //p2在p1窗口中的序号
                    int index_L = index_p1*S + index_w_p1;//L[p1,p2]的位置
                    pv[index_L] += v;
                }
            }
        }
    }

    delete []tmp;

}


void processScribble(const ImageHelper &scribble,int w,int foreground,int background,double lambda,
                      double*pv,double*pB) {
    const int l = 1 + 4*w;    //与某点同一窗口的点构成的矩形边长
    const int S = l * l;      //该矩形元素数量
    const int width = scribble.width(); const int height = scribble.height();
    for(int x=0;x<width;++x) {
        for(int y=0;y<height;++y) {
            int index = scribble.index(x,y);
            double v = scribble.is_color(x,y,foreground)
                    ? lambda : 0;
            pB[index] = v;
            if(scribble.is_color(x,y,foreground)
                    || scribble.is_color(x,y,background)) {
                int i = index*S + (S-1)/2;
                pv[i] += lambda;
            }
        }
    }
}

int clearMatrix(int *px,int *py,double *pv,int nz) {
    int offset = 0;
    int pos = 0;
    while(pos < nz - offset) {
        if(px[pos] == -1) {
            offset += 1;
            int last = nz - offset;
            std::swap(px[pos],px[last]);
            std::swap(py[pos],py[last]);
            std::swap(pv[pos],pv[last]);
        } else {
            pos += 1;
        }
    }
    return nz - offset;
}

void getAlpha(const ImageHelper &image, const ImageHelper &scribble, int foreground,
                  int background, int w, double e, double lambda,double *alpha) {

    const int width  = image.width();
    const int height = image.height();

    const int N = width * height; //点总数量
    const int l = 1 + 4*w;   //与某点同一窗口的点构成的矩形边长
    const int S = l * l; //该矩形元素数量
    const int nz = N * S;   //稀疏矩阵元素数量

    int *px = new int[nz];
    int *py = new int[nz];  //横纵坐标
    double *pv = new double[nz];    //值
    memset(pv,0,sizeof(pv[0])*nz);

    fillPxPy(image,w,px,py,pv);
    scanWindow(image,w,e,pv);

    processScribble(scribble,w,foreground,background,lambda,pv,alpha);

    int len = clearMatrix(px,py,pv,nz);

    solve_sparse(px,py,pv,alpha,len,N);

    delete []px;
    delete []py;
    delete []pv;
}

double p(double x){
    const double th = 0.8;
    if(x > th) {
        return 0;
    } else {
        return 1 - x/th;
    }
}

Matting getFBA(const ImageHelper  &image, double *pAlpha) {
    const int width = image.width();  const int height = image.height();
    const int sz = image.size();

    double *pb = malloc_zeroed_buffer(sz*6);
    double *pv = malloc_zeroed_buffer(sz*36);
    int *pi = new int[sz*36];
    int *pj = new int[sz*36];

    //构造v
    for(int x=0;x<width;++x) {
        for(int y=0;y<height;++y) {
            int idx = image.index(x,y);
            int color[3]; image.pixel(idx,color);
            double a = pAlpha[idx];
            double b = 1-a;

            for(int c=0;c<3;++c) {

                double i = color[c];

                int iF = 36*idx+12*c;
                int iB = iF+1;
                int iF1 = iF+2;
                int iFw = iF+3;
                int iF_1 = iF+4;
                int iF_w = iF+5;

                int fc = 6*idx+c*2;
                int bc = fc+1;

                /*方程1*/
                double k = 1;
                //F
                pv[iF] += 2*a*a*k;
                pv[iB] += 2*a*b*k;
                pb[fc] += 2*a*i*k;

                pi[iF] = pi[iB] = fc;
                pj[iF] = fc;
                pj[iB] = bc;

                //B
                pv[iF+6] += 2*b*b*k;
                pv[iB+6] += 2*a*b*k;
                pb[bc] += 2*b*i*k;

                pi[iF+6] = pj[iF+6] = bc;

                pi[iB+6] = bc;
                pj[iB+6] = fc;

                /*方程2*/
                if(x!=width-1) {
                    double cf = 2*p(a);
                    double cb = 2*p(1-a);
                    //F
                    pv[iF] += cf;
                    pv[iF1] -= cf;

                    pi[iF1] = fc;
                    pj[iF1] = 6*(idx+1)+c*2;

                    //B
                    pv[iF+6] += cb;
                    pv[iF1+6] -= cb;

                    pi[iF1+6] = bc;
                    pj[iF1+6] = 6*(idx+1)+c*2+1;

                } else {
                    pj[iF1] = pj[iF1+6] = pi[iF1] = pi[iF1+6] = -1;
                }

                /*方程3*/
                if(y!=height-1) {
                    double cf = 2*p(a);
                    double cb = 2*p(1-a);
                    //F
                    pv[iF] += cf;
                    pv[iFw] -= cf;

                    pi[iFw] = fc;
                    pj[iFw] = 6*(idx+width)+c*2;

                    //B
                    pv[iF+6] += cb;
                    pv[iFw+6] -= cb;

                    pi[iFw+6] = bc;
                    pj[iFw+6] = 6*(idx+width)+c*2+1;

                } else {
                    pj[iFw] = pj[iFw+6] = pi[iFw] = pi[iFw+6] = -1;
                }

                /*方程4*/
                if(x!=0) {
                    double a = pAlpha[idx-1];
                    double cf = 2*p(a);
                    double cb = 2*p(1-a);
                    //F
                    pv[iF] += cf;
                    pv[iF_1] -= cf;

                    pi[iF_1] = fc;
                    pj[iF_1] = 6*(idx-1)+c*2;

                    //B
                    pv[iF+6] += cb;
                    pv[iF_1+6] -= cb;

                    pi[iF_1+6] = bc;
                    pj[iF_1+6] = 6*(idx-1)+c*2+1;
                } else {
                    pj[iF_1] = pj[iF_1+6] = pi[iF_1] = pi[iF_1+6] = -1;
                }

                /*方程5*/
                if(y!=0) {
                    double a = pAlpha[idx-width];
                    double cf = 2*p(a);
                    double cb = 2*p(1-a);
                    //L
                    pv[iF] += cf;
                    pv[iF_w] -= cf;

                    pi[iF_w] = fc;
                    pj[iF_w] = 6*(idx-width)+c*2;

                    //B
                    pv[iF+6] += cb;
                    pv[iF_w+6] -= cb;

                    pi[iF_w+6] = bc;
                    pj[iF_w+6] = 6*(idx-width)+c*2+1;
                } else {
                    pj[iF_w] = pj[iF_w+6] = pi[iF_w] = pi[iF_w+6] = -1;
                }
            }
        }
    }

    int len = clearMatrix(pi,pj,pv,sz*36);
    solve_sparse(pj,pi,pv,pb,len,6*sz);

    QImage imgF(width,height,QImage::Format_ARGB32);
    QImage imgB(width,height,QImage::Format_ARGB32);

    for(int x=0;x<width;++x) {
        for(int y=0;y<height;++y) {
            int i = y*width+x;
            int j = i*6;

            double rf = pb[j+0];
            double gf = pb[j+2];
            double bf = pb[j+4];

            double rb = pb[j+1];
            double gb = pb[j+3];
            double bb = pb[j+5];

            double a = range(pAlpha[i],0,1);
            double b = 1-a;

            rf = range(rf,0,255);
            gf = range(gf,0,255);
            bf = range(bf,0,255);

            rb = range(rb,0,255);
            gb = range(gb,0,255);
            bb = range(bb,0,255);

            //a = b = 1;
            imgF.setPixel(x,y,qRgba(rf,gf,bf,255*a));
            imgB.setPixel(x,y,qRgba(rb,gb,bb,255*b));

        }
    }
    QImage alpha = imageFromAlpha(width,height,pAlpha);
    return Matting(imgF,imgB,alpha);
}

Matting getMatting(const QImage &image, const QImage &scribble, QRgb foreground,
                  QRgb background,  int w, double e, double lambda)
{
    assert(image.size() == scribble.size());
    const int width = image.width();
    const int height = image.height();
    const int sz = width*height;
    double *pAlpha = new double[sz];

    BEGIN_TIME

    //solve alpha
    getAlpha(ImageHelper(image),ImageHelper(scribble), foreground,
                      background, w, e, lambda,pAlpha);

    PRINT_TIME("compute alpha:");
    REBEGIN_TIME

    //rectify alpha
    for(int i=0;i<sz;++i){
        pAlpha[i] = range(pAlpha[i],0,1);
    }

    //get foreground background alpha
    Matting p = getFBA(image,pAlpha);

    PRINT_TIME("compute FBA:");

    delete []pAlpha;
    return p;
}

}

