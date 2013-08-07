#include "dotobject.h"
#include "common.h"

DotObject::DotObject(const std::vector<cv::Point3f> &points) : GlcontrollableObject()
{
    FOR_EACH(it,points) {
        GLvex3f vex(it->x,it->y,it->z);
        this->m_render.m_vexs.push_back(vex);
    }

    this->position.z = -200;
}
void DotObject::render() {
    this->m_render.render();
}
