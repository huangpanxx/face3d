#ifndef USFHEAD_H
#define USFHEAD_H

#include "globject.h"
#include <vector>
#include <QtOpenGL>

class  USFHead : public GlcontrollableObject
{
public:
    USFHead(const char* filename);
    bool load(const char* filename);
    void render() {
        this->m_render.render();
    }
protected:
    GLtriRender m_render;
};

#endif // USFHEAD_H
