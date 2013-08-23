#include "dotobject.h"
#include "common.h"

DotObject::DotObject(const std::vector<cv::Point3f> &points,bool centric,bool flipY,const QString &name )
    : GlcontrollableObject(name)
{

    VEC(cv::Point3f) src;
    if(centric) {
        src = centric_points(points,flipY);
    } else {
        src.reserve(points.size());
        int coef = flipY?-1:1;
        for(auto &it :points) {
            src.push_back(cv::Point3f(it.x,coef*it.y,it.z));
        }
    }
    src = remove_near_points2d(src,1);

    for(auto &it : src) {
        GLvex3f vex(it.x,it.y,it.z);
        this->m_render.vexs.push_back(vex);
    }

    VEC(cv::Point2f) pt2f = xy_from_xyz(src);
    VEC(cv::Vec3i)  tris = trianglate(pt2f);
    for(auto &it :tris) {
        GLtri3f tri(it(0),it(1),it(2));
        this->m_render.tris.push_back(tri);
    }
    this->position.z = -500;
}
void DotObject::render() {
    this->m_render.render();
}
