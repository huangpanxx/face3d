#include "3dhelper.h"

using namespace std;

void write_off_file(const std::vector<cv::Point3f> points,
                    ostream &fout) {
    VEC(cv::Point2f) pts2f = xy_from_xyz(points);
    std::vector<cv::Vec3i> vexs = trianglate(pts2f);
    write_off_file(points,vexs,fout);
}

void write_off_file(const std::vector<cv::Point3f> points,
                    const std::vector<cv::Vec3i> vexs,
                    ostream& fout) {

    fout<<"OFF"<<endl;
    fout<<points.size()<<"\t"<<vexs.size()<<"\t0"<<endl;
    for(auto &pt : points) {
        fout<<pt.x<<"\t"<<-pt.y<<"\t"<<pt.z<<endl;
    }
    for(auto &vex : vexs) {
        fout<<"3\t"<<vex(0)<<"\t"<<vex(1)<<"\t"<<vex(2)<<endl;
    }
}

VEC(cv::Point3f) centric_points(const VEC(cv::Point3f) &points,bool flipY ) {
    float sx=0,sy=0,sz=0;
    for(auto &it : points) {
        sx += it.x;
        sy += it.y;
        sz += it.z;
    }
    sx /= points.size();
    sy /= points.size();
    sz /= points.size();

    VEC(cv::Point3f) rts;
    rts.reserve(points.size());
    for(auto &it :points) {
        float x = it.x - sx;
        float y = flipY ? (sy - it.y) : (it.y - sy);
        float z = it.z - sz;
        rts.push_back(cv::Point3f(x,y,z));
    }
    return rts;
}

VEC(cv::Point2f) centric_points(const VEC(cv::Point2f) &points,bool flipY ) {
    float sx=0,sy=0;
    for(auto &it :points) {
        sx += it.x;
        sy += it.y;
    }
    sx /= points.size();
    sy /= points.size();

    VEC(cv::Point2f) rts;
    rts.reserve(points.size());
    for(auto &it : points) {
        float x = it.x - sx;
        float y = flipY ? (sy - it.y) : (it.y - sy);
        rts.push_back(cv::Point2f(x,y));
    }
    return rts;
}

