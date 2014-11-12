#include "twod.h"
#include "ui_twod.h"

twod::twod(catmull::Mode mode) :
    ui(new Ui::twod)
{
    ui->setupUi(this);
    window = mode;
}

twod::~twod()
{
    delete ui;
}

void twod::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void twod::initializeGL()
{
    //Background color will be white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glShadeModel( GL_FLAT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glPointSize(5);
}


void twod::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();

    if (window == catmull::XZ) {
        glRotatef(-90, 1, 0, 0);
    }
    else if (window == catmull::ZY) {
        glRotatef(90, 0, 1, 0);
    }
    else ;  // Do nothing

    if (window == catmull::XY)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawLine(-400, 0, 0, 400, 0, 0);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawLine(0, -400, 0, 0, 400, 0);
    }
    else if (window == catmull::XZ)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawLine(-400, 0, 0, 400, 0, 0);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawLine(0, 0, -400, 0, 0, 400);
    }
    else  //(window == catmull::ZY)
    {
        glColor3f(0.0f, 0.0f, 1.0f);
        drawLine(0, 0, -400, 0, 0, 400);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawLine(0, -400, 0, 0, 400, 0);
    }

    catt->draw();
}

void twod::redraw()
{
   updateGL();
   if (m_glWidget!=NULL) {
       m_glWidget->update();
       m_glWidget->repaint();
   }
}

/* 2D */
void twod::resizeGL( int w, int h )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(-w,GLdouble(w),-h,GLdouble(h),-1000.0,1000.0);
    glFlush();
    glMatrixMode(GL_MODELVIEW);
    glViewport( 0, 0, (GLint)w, (GLint)h );
}

/* QVector<double> twod::convertClick(QVector<double> click)
 *
 * Converts the given click to a window-normalized version.  The e->click event that
 * returns x and y, is realtive to the window, not the coordinate system.  A simple
 * calculation changes that.
 */
QVector<double> twod::convertClick(QVector<double> click)
{
    click[0] = ((click.at(0) * 2) - width());
    click[1] = ((click.at(1) * 2) - height());
    return click;
}

void twod::mousePressEvent( QMouseEvent *e )
{
    QVector<double> click = QVector<double>();
    click.append(e->x());
    click.append(height()-e->y());
    click = convertClick(click);
    if (window == catmull::XY)
        catt->mousePressEvent(click.at(0), click.at(1), 0, e->button(), window );
    else if (window == catmull::XZ)
        catt->mousePressEvent(click.at(0), 0, click.at(1), e->button(), window );
    else  // window == ZY
        catt->mousePressEvent(0, click.at(1), click.at(0), e->button(), window );
    updateGL();
}

void twod::mouseReleaseEvent( QMouseEvent *e)
{
    QVector<double> click = QVector<double>();
    click.append(e->x());
    click.append(height()-e->y());
    click = convertClick(click);
    catt->mouseReleaseEvent();
    updateGL();
}

void twod::mouseMoveEvent ( QMouseEvent *e )
{
    QVector<double> click = QVector<double>();
    click.append(e->x());
    click.append(height()-e->y());
    click = convertClick(click);
    if (window == catmull::XY)
        catt->mouseMoveEvent(click.at(0), click.at(1), 0, window);
    else if (window == catmull::XZ)
        catt->mouseMoveEvent(click.at(0), 0, click.at(1), window);
    else  // window == ZY
        catt->mouseMoveEvent(0, click.at(1), click.at(0), window);
    updateGL();
}
