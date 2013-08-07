#include "glviewer.h"
#include <glu.h>
#include <QKeyEvent>
#include <QDebug>

GLViewer::GLViewer(QWidget *parent, QString title) :
    QGLWidget(parent)
{
    this->setWindowTitle(title);
    m_selectedIndex = -1;
}

GLViewer::~GLViewer() {

}
void GLViewer::addObject(GLobject *obj) {
    this->m_objects.push_back(
                QSharedPointer<GLobject>(obj));
    if(this->getSelectedObject().isNull()) {
        this->selectObject(0);
    }
}

void GLViewer::initializeGL() {
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


    glEnable( GL_TEXTURE_2D );

    GLfloat lightAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightPosition[4] = { 0.0, 0.0, 2.0, 1.0 };

    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
    glLightfv( GL_LIGHT1, GL_POSITION, lightPosition );

    glEnable( GL_LIGHT1 );

    glEnable(GL_COLOR_MATERIAL);

    m_light = false;
}

void GLViewer::paintGL() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    this->render();
}

void GLViewer::resizeGL( int width, int height ) {
    if ( height == 0 ) { height = 1; }
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 1000.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void GLViewer::switchLight() {
    m_light = !m_light;
    if(m_light) {
        glEnable( GL_LIGHTING );
    }else{
        glDisable( GL_LIGHTING );
    }
}

void GLViewer::keyPressEvent( QKeyEvent *e )
{
    int key = e->key();
    if (key == Qt::Key_Escape) {
        this->close();
    } else if(key == Qt::Key_L) {
        this->switchLight();
    } else if(key>='0' && key<='9') {
        this->selectObject(key==0?10:key-'1');
    }else {
        sendKeyMessage(e);
    }
    updateGL();
}

bool GLViewer::selectObject(int index)  {
    if(index < 0 || index >= (int)this->m_objects.size()) {
        return false;
    }
    this->m_selectedIndex = index;
    this->setWindowTitle(this->getSelectedObject()->name());
    return true;
}

void GLViewer::sendKeyMessage( QKeyEvent *e ) const {
    QSharedPointer<GLobject> obj = this->getSelectedObject();
    if(!obj.isNull()) {
        obj->keyEvent(e);
    }
}

void GLViewer::render() {
    for(uint i=0;i<this->m_objects.size();++i) {
        QSharedPointer<GLobject> obj =  this->m_objects[i];
        glLoadIdentity();
        glTranslatef(obj->position.x,obj->position.y,obj->position.z);
        glRotatef(obj->rotate.x ,  1.0,  0.0,  0.0 );
        glRotatef(obj->rotate.y ,  0.0,  1.0,  0.0 );
        glRotatef(obj->rotate.z ,  0.0 , 0.0,  1.0 );
        this->m_objects[i]->render();
    }
}

QSharedPointer<GLobject>
GLViewer::getSelectedObject() const {
    if(this->m_selectedIndex < 0 || this->m_selectedIndex>= (int)this->m_objects.size()) {
        return QSharedPointer<GLobject>();
    }
    return this->m_objects[this->m_selectedIndex];
}
