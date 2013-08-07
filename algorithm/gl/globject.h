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
            float z = 200-vexs[tris[i].v1].z;
            float r=0,g=0,b=0;
            const float s = 50;
            if(z<s) {
                r=z/s;  g=1-z/s;  b=0;
            }else{
                r=1;    g=0;      b=(z-s)/(100-s);
            }
            glVertex3f(vexs[tris[i].v1]);
            glColor3f( r,g,b );
            glVertex3f(vexs[tris[i].v2]);
            glColor3f( r,g,b );
            glVertex3f(vexs[tris[i].v3]);
            glColor3f( r,g,b );
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
