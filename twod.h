#ifndef TWOD_H
#define TWOD_H

#include <QGLWidget>
#include "foundation.h"
#include <QtGui>
#include <QtOpenGL>
#if defined(Q_WS_MAC)
# include <OpenGL/glu.h>
#else
# ifndef QT_LINUXBASE
#   include <GL/glu.h>
# endif
#endif
#include "catmull.h"
#include "glwidget.h"

namespace Ui {
    class twod;
}

class GLWidget;
class twod : public QGLWidget {
    Q_OBJECT
public:
    twod(catmull::Mode mode);
    ~twod();
    void setCatt(catmull *c){catt=c;}
    void setgl(GLWidget *m){m_glWidget=m;}

public slots:
    void myUpdate() { updateGL(); }

signals:
    void sendUpdate();

protected:
    void changeEvent(QEvent *e);


    //Initialize the OpenGL Graphics Engine
    void initializeGL();

    //All our painting stuff are here
    void paintGL();

    //When user resizes main window, the scrollArea will be resized and it will call this function from
    //its attached GLWidget
    void resizeGL(int width, int height);
    //Handle mouse
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::twod *ui;
    catmull *catt;
    GLWidget *m_glWidget;
    void redraw();
    catmull::Mode window;

    QVector<double> convertClick(QVector<double> click);
};

#endif // TWOD_H
