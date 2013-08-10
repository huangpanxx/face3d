#include "3dhelper.h"

using namespace std;

void write_off_file(const std::vector<cv::Point3f> points,
                    ostream &fout) {
    VEC(cv::Point2f) pts2f = points2f_from_points3f(points);
    std::vector<cv::Vec3i> vexs = trianglate(pts2f);
    write_off_file(points,vexs,fout);
}

void write_off_file(const std::vector<cv::Point3f> points,
                    const std::vector<cv::Vec3i> vexs,
                    ostream& fout) {

    fout<<"OFF"<<endl;
    fout<<points.size()<<"\t"<<vexs.size()<<"\t0"<<endl;
    FOR_EACH(pt,points) {
        fout<<pt->x<<"\t"<<-pt->y<<"\t"<<pt->z<<endl;
    }
    FOR_EACH(vex,vexs) {
        fout<<"3\t"<<(*vex)(0)<<"\t"<<(*vex)(1)<<"\t"<<(*vex)(2)<<endl;
    }
}
