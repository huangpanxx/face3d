#ifndef GLVIEWER_H
#define GLVIEWER_H


#include <QWidget>
#include <QGLWidget>
#include <vector>
#include "globject.h"
#include <QSharedPointer>

class GLViewer : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit GLViewer(QWidget *parent = 0, QString title="GLViewer");
    ~GLViewer();
    void addObject(GLobject *obj);


protected:
  void keyPressEvent( QKeyEvent *e );
  void initializeGL();
  void paintGL();
  void resizeGL( int width, int height );
  void render();
  void sendKeyMessage( QKeyEvent *e ) const;
  bool selectObject(int index);
  void switchLight();
  QSharedPointer<GLobject> getSelectedObject() const;

protected:
  std::vector< QSharedPointer<GLobject> > m_objects;
  int m_selectedIndex;
  bool m_light;


};

#endif // GLVIEWER_H
