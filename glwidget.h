//-------------------------------------------------------------------------------------------
//  Multiple viewports example
//-------------------------------------------------------------------------------------------
//These two lines are header guardians against multiple includes
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QProgressBar>
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
#include "version.h"
#include "twod.h"


const int maxw=1024;
const int maxh=1024;
//This is our OpenGL Component we built it on top of QGLWidget
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    //Constructor for GLWidget
    GLWidget(QWidget *parent = 0);

    //Destructor for GLWidget
    ~GLWidget();

    void clear();
    void setCatt(catmull *c){catt=c;}
    void setView(int w, int h);
    void groundPlane(bool showGroundPlaneL);
    void axis(bool showAxisL);

public slots:
    void myupdate(){updateGL();}
    void changeLookAt();

signals:
    void sendUpdate();

protected:
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
    void startup();
    GLdouble xfrom,yfrom,zfrom, xto, yto, zto;
    int winw, winh, button,imx,imy;
    int version;
    void dopan(int x, int y, bool click);
    void zoomWheel(int z);
    QImage *myimage;
    QImage timage;

    void displayImage();
    void drawCircle(double radius, double xcen, double ycen, bool line, int nseg);
    // for the cube
    double scale;

    // Functions
    void redraw();
    void drawCircle(int radius, int xcen, int ycen,  QImage *buf);
    void initLight();
    catmull *catt;
    QTimer *animationTimer;

    // mouse routines for camera control to be implemented
    Vector3d CameraPos;
    QPoint lastMousePoint;
    bool Rotating;
    bool Scaling;
    bool showGroundPlane;
    bool showAxis;

    int lookingAt;

    void RotateY(double rad);
    void RotateZ(double rad);
    void DoRotate(QPoint desc, QPoint orig);
    void DoScale(QPoint desc, QPoint orig);
};


#endif
