#include "dotobject.h"
#include "common.h"

DotObject::DotObject(const std::vector<cv::Point3f> &points,const QString &name)
    : GlcontrollableObject(name)
{

    FOR_EACH(it,points) {
        GLvex3f vex(it->x,it->y,it->z);
        this->m_render.vexs.push_back(vex);
    }

    VEC(cv::Point2f) pt2f = points2f_from_points3f(points);
    VEC(cv::Vec3i)  tris = trianglate(pt2f);
    FOR_EACH(it,tris) {
        GLtri3f tri((*it)(0),(*it)(1),(*it)(2));
        this->m_render.tris.push_back(tri);
    }
    this->position.z = -200;
}
void DotObject::render() {
    this->m_render.render();
}
