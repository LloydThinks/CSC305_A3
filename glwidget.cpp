//-------------------------------------------------------------------------------------------
//   Qtgl for graphics class
//-------------------------------------------------------------------------------------------

#include "glwidget.h"

const double RadPerPixel = - 0.01;
const double MovePerPixel = - 0.1;
GLfloat vertices[4][2] = {{100.0, 100.0}, {400.0, 100.0}, {400.0, 400.0}, {100.0, 400.0}}; //  vertex coords
GLubyte indices[] = {0,1,1,2,2,3,3,0};

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    startup();
}

GLWidget::~GLWidget()
{    

}

void GLWidget::startup()
{
    winw=width();  // width returns width of window
    winh=height();
    button = 0;
    version=MYVERSION;
    scale = 1.5;// this will be reset by the 2dView
    xfrom=yfrom=zfrom=5.0;
    xto=yto=zto=0.0;
    animationTimer = new QTimer(this);
    // the timer will send a signal timeout at regular intervals.
    // whenever this timeout signal occurs, we want it to call our drawOpenGL
    // function

    connect(animationTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    // we start the timer with a timeout interval of 20ms
    animationTimer->start(20);
    lookingAt = -1;

    showAxis = true;
    showGroundPlane = true;
}

void GLWidget::clear()
{
     updateGL();
}

void GLWidget::initializeGL()
{
    //glClearColor( 0.5, 1.0, 0.75, 0.0 ); // Let OpenGL clear to black

    glEnable (GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat position[] = { 50, 50, 50, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);// enable diffuse
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);// enable diffuse
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);// enable specular
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glShadeModel( GL_SMOOTH );

    glFrontFace(GL_CCW);
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
}

void GLWidget::redraw()
{
        updateGL();
}

void GLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(xfrom,yfrom,zfrom, xto, yto, zto, 0.0, 1.0, 0.0);

    if (showGroundPlane)
    {
        glColor3f(0.0f, 0.0f, 0.6f);
        for (double i = -1; i <= 1; i += 0.1)
        {
            drawLine(i, 0.0, -1.0, i, 0.0, 1.0);
            drawLine(-1.0, 0.0, i, 1.0, 0.0, i);
        }
    }
    if (showAxis)
    {
        // Draw Axis
        glColor3f(1.0f, 0.0f, 0.0f);
        drawLine(-1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 2.0);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawLine(0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 2.0);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawLine(0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 2.0);
    }
    glPushMatrix();
    glScalef( scale, scale, scale );
    // scale the catmul spline to fit into a 1 by 1
    if (catt!=NULL) catt->draw();
    glPopMatrix();

    sendUpdate();
}

/* 2D */
void GLWidget::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 1500.0 );
    glMatrixMode( GL_MODELVIEW );
}

void GLWidget::initLight()
{
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 128.0 };
   //   GLfloat light_position[] = { 0.3, 0.3, 0.8, 0.0 };
   GLfloat light_position[] = { 0.2, 0.2, 0.9, 0.0 };
   GLfloat light_ambient[] = { 0.05, 0.05, 0.05, 1.0 };
//   GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat light_specular[] = { 0.99, 0.99, 0.99, 1.0 };
   GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };

// glClearColor(1.0, 1.0, 0.9, 1.0);
     glShadeModel (GL_SMOOTH);
   //   glShadeModel (GL_FLAT);

// MATERIAL
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

// LIGHT0
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   //   glEnable(GL_DEPTH_TEST);
   //   glDisable(GL_LIGHTING);

}

void GLWidget::setView(int w, int h)
{
    // sets up scale
    if (w>h) scale = (double)h;
    else scale = (double)w;
    scale = 1.0/scale;
}

// mouse routines for camera control to be implemented
//****************************************************

void GLWidget::mousePressEvent( QMouseEvent *e )
{
    //TODO:When a button is pressed!
    if ( e->button() == Qt::LeftButton)
    {
        Rotating = true;
        lastMousePoint = e->pos();
    }

    if ( e->button() == Qt::RightButton)
    {
        Scaling = true;
        lastMousePoint = e->pos();
    }

}

void GLWidget::mouseReleaseEvent( QMouseEvent *e)
{
    //TODO:When some pressed mouse button is release!
    if ( e->button() == Qt::LeftButton)
    {
        Rotating = false;
        DoRotate(e->pos(), lastMousePoint);
    }

    if ( e->button() == Qt::RightButton)
    {
        Scaling = false;
        DoScale(e->pos(), lastMousePoint);
    }
    updateGL();
}

void GLWidget::mouseMoveEvent( QMouseEvent *e )
{
    //TODO: when the mouse is moved!
    if ( (e->buttons() & Qt::LeftButton) && Rotating)
    {
        DoRotate(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }

    if ( (e->buttons() & Qt::RightButton) && Scaling)
    {
        DoScale(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }
    updateGL();

}

void GLWidget::RotateY(double rad)
{
    //TODO: rotate pVec around Y axis by the angle rad
    double cosPhi = cos(rad);
    double sinPhi = sin(rad);

    Matrix33d m;
    m.M11 = cosPhi;
    m.M13 = sinPhi;
    m.M22 = 1;
    m.M31 = -sinPhi;
    m.M33 = cosPhi;

    Vector3d pVec = Vector3d();
    pVec.x = xfrom;
    pVec.y = yfrom;
    pVec.z = zfrom;

    pVec = MultiplyMatrix33Vec3(pVec, m);

    xfrom = pVec.x;
    yfrom = pVec.y;
    zfrom = pVec.z;
}

void GLWidget::RotateZ(double rad)
{
    //TODO: rotate pVec around Z axis by the angle rad
    double cosPhi = cos(rad);
    double sinPhi = sin(rad);

    Matrix33d m;
    m.M11 = cosPhi;
    m.M12 = -sinPhi;
    m.M21 = sinPhi;
    m.M22 = cosPhi;
    m.M33 = 1;

    Vector3d pVec = Vector3d();
    pVec.x = xfrom;
    pVec.y = yfrom;
    pVec.z = zfrom;

    pVec = MultiplyMatrix33Vec3(pVec, m);

    xfrom = pVec.x;
    yfrom = pVec.y;
    zfrom = pVec.z;
}

void GLWidget::DoRotate(QPoint desc, QPoint orig)
{
    //TODO: adjust the camera position so the viewport is rotated
    double YRot = (desc.x() - orig.x()) * RadPerPixel;
    double ZRot = -1 * (desc.y() - orig.y()) * RadPerPixel;

    RotateY(YRot);
    RotateZ(ZRot);
}

void GLWidget::DoScale(QPoint desc, QPoint orig)
{
    //TODO: adjust the camera position so the viewport is scaled.
    double length = sqrt(xfrom * xfrom + yfrom * yfrom + zfrom * zfrom);

    double newLength = length + (desc.y() - orig.y()) * MovePerPixel;

    if (newLength > 6)
    {
        double ratio = newLength / length;
        xfrom *= ratio;
        yfrom *= ratio;
        zfrom *= ratio;
    }
}

void GLWidget::changeLookAt()
{
    lookingAt++;
    if (lookingAt == catt->lastpt)
    {
        lookingAt = -1;
        xto = 0; yto = 0; zto = 0;
    }
    else
    {
        xto = double(catt->pnts[lookingAt][0])/300.0;
        yto = double(catt->pnts[lookingAt][1])/300.0;
        zto = double(catt->pnts[lookingAt][2])/300.0;
    }

}

void GLWidget::groundPlane(bool showGroundPlaneL)
{
    showGroundPlane = showGroundPlaneL;
}

void GLWidget::axis(bool showAxisL)
{
    showAxis = showAxisL;
}
