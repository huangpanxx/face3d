#ifndef OBJECT_H
#define OBJECT_H

#include <QtOpenGL>

class GLvex3f {
public:
    GLvex3f() {
        this->x = 0; this->y = 0; this->z = 0;
    }

    GLvex3f (GLfloat x,GLfloat y,GLfloat z) {
        this->x = x; this->y = y; this->z = z;
    }
    GLfloat x,y,z;
};

typedef std::vector<GLvex3f> GLvexs;


class GLtri3f{
public:
    GLtri3f() {
        this->v1 = 0;  this->v2 = 0;  this->v3 = 0;
    }
    GLtri3f(GLint v1,GLint v2,GLint v3) {
        this->v1 = v1; this->v2 = v2; this->v3 = v3;
    }
    GLint v1,v2,v3;
};

typedef std::vector<GLtri3f> GLtris;


inline void glVertex3f(const GLvex3f& vex) {
    glVertex3f(vex.x,vex.y,vex.z);
}
inline void glColor3f(const GLvex3f& color) {
    glColor3f(color.x,color.y,color.z);
}


class GLobject
{
public:
    GLobject() { this->position.z = -100; }
    virtual void render() = 0;
    virtual void keyEvent(QKeyEvent *) { }
    virtual QString name() const = 0;
    GLvex3f rotate;
    GLvex3f position;
};

class GLrender {
public:
    virtual void render() = 0;
};

class GLtriRender : public GLrender {
public:
    virtual void render() {
        glBegin(GL_TRIANGLES);
        for(uint i=0;i<this->tris.size();++i) {
            const GLtri3f &tri = tris[i];
            const GLvex3f &p1  = vexs[tri.v1];
            const GLvex3f &p2  = vexs[tri.v2];
            const GLvex3f &p3  = vexs[tri.v3];
            float x1 = p2.x - p1.x; float y1 = p2.y - p1.y; float z1 = p2.z - p1.z;
            float x2 = p3.x - p1.x; float y2 = p3.y - p1.y; float z2 = p3.z - p1.z;
            float nx = y1*z2 - y2*z1;
            float ny = z1*x2 - z2*x1;
            float nz = x1*y2 - x2*y1;
            float s = sqrt(nx*nx + ny*ny + nz*nz);
            glNormal3f(nx/s,ny/s,nz/s);
            glVertex3f(p1);
            glVertex3f(p2);
            glVertex3f(p3);
        }
        glEnd();
    }
    GLvexs vexs;
    GLtris tris;
};

class GLdotRender : public GLrender {
public:
    virtual void render() {
        glPointSize(5);
        glBegin(GL_POINTS);
        for(uint i=0;i<this->m_vexs.size();++i) {
            GLvex3f vex = this->m_vexs[i];
            if(this->m_colors.size() != 0) {
                GLvex3f color = this->m_colors[i];
                glColor3f(color);
            }
            glVertex3f(vex);
        }
        glEnd();
    }
    GLvexs m_colors;
    GLvexs m_vexs;
};

class GlcontrollableObject : public GLobject
{
public:
    GlcontrollableObject() { }
    virtual void keyEvent(QKeyEvent *e) {
        switch(e->key()) {
        case 'A': this->position.x -= 10; break;
        case 'D': this->position.x += 10; break;
        case 'W': this->position.y += 10; break;
        case 'S': this->position.y -= 10; break;
        case 'Q': this->position.z += 10; break;
        case 'E': this->position.z -= 10; break;
        case Qt::Key_Left: this->rotate.y -= 10;  break;
        case Qt::Key_Right: this->rotate.y += 10; break;
        case Qt::Key_Up: this->rotate.x -= 10;    break;
        case Qt::Key_Down: this->rotate.x += 10;  break;
        };
    }
};



#endif // OBJECT_H
