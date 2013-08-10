#ifndef MATHEAD_H
#define MATHEAD_H

#include "globject.h"
#include <vector>



class MatHead : public GlcontrollableObject
{
public:
    MatHead(const char* shapeFileName, const char *colorFileName);
    void render() { this->m_render.render(); }
protected:
    GLdotRender m_render;
};

#endif // MATHEAD_H
