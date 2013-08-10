#include "usfhead.h"
#include <fstream>
#include <string>
#include <QDebug>
using namespace std;

USFHead::USFHead(const char* filename)
    : GlcontrollableObject("USFHead"){
    this->position.z = -600;
    this->position.x = 150;
    do{
        ifstream fin(filename);
        if(!fin) break;
        string off; fin>>off;
        if(off != "OFF") break;
        int nvex,ntri,unknown;
        fin>>nvex>>ntri>>unknown;
        if(!fin) break;
        this->m_render.vexs.resize(nvex,GLvex3f(0,0,0));
        for(int i=0;i<nvex && fin;++i) {
            fin>>m_render.vexs[i].x>>m_render.vexs[i].y>>m_render.vexs[i].z;
        }
        if(!fin) break;
        m_render.tris.resize(ntri,GLtri3f(0,0,0));
        for(int i=0;i<ntri;++i) {
            fin>>unknown;
            fin>>m_render.tris[i].v1>>m_render.tris[i].v2>>m_render.tris[i].v3;
        }
    }while(false);
}



