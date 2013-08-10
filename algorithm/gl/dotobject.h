#ifndef DOTOBJECT_H
#define DOTOBJECT_H

#include "globject.h"
#include <vector>
#include <opencv2/core/core.hpp>

class DotObject : public GlcontrollableObject
{
public:
    DotObject(const std::vector<cv::Point3f> &points, const QString &name = "DotObject");
    virtual void render();

protected:
    GLtriRender m_render;
};


#endif // DOTOBJECT_H
