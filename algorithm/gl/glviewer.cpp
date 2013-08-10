#include "glviewer.h"
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
    glEnable(GL_COLOR_MATERIAL);
    m_light = true;
    glEnable( GL_LIGHTING );
}

void GLViewer::paintGL() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();



    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat lightColor[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat lightPos[] = {0 , 0 , -500, 10.0f};
    //Diffuse (non-shiny) light component
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    //Specular (shiny) light component
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);



    //The color of the sphere
    GLfloat materialColor[] = {0.2f, 0.2f, 1.0f, 1.0f};
    //The specular (shiny) component of the material
    GLfloat materialSpecular[] = {1, 1, 1, 1.0f};
    //The color emitted by the material
    GLfloat materialEmission[] = {0.05, 0.05, 0.05, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialf(GL_FRONT, GL_SHININESS, 25); //The shininess parameter

    this->render();
}

void GLViewer::resizeGL( int width, int height ) {
    if ( height == 0 ) { height = 1; }
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLfloat zNear = 0.1;
    GLfloat zFar = 1000.0;
    GLfloat aspect = (GLfloat)width/(GLfloat)height;
    GLfloat fH = tan(GLfloat(90.0/360.0*3.1415926))*zNear/2;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

   // glMatrixMode( GL_PROJECTION );
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
