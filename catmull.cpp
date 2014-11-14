// catmull.cpp: implementation of the catmull Rom spline class.
//
//////////////////////////////////////////////////////////////////////

#include "catmull.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

catmull::catmull()
{
	lastpt=0;
	hull = true;
	mousedown=false;
	animatenow=false;
	tvmethod=0;
	lastArc=0;
    makeArcMode=false;
    motionType =0;
	frames = 5;
	error=0;
    tensionValue = 50;

    numSteps = 25;
    numCircleSegs = 25.0;
    radius = 50.0;

    frenetFrameBoxIndex = 0;
    fbSize = 25.0;
    frenetFrameBox = QVector<QVector3D>(8);

    frenetFrameBox[0] = QVector3D(-50, 50, 50);
    frenetFrameBox[1] = QVector3D(-50 , 50, -50);
    frenetFrameBox[2] = QVector3D(50, 50, -50);
    frenetFrameBox[3] = QVector3D(50, 50, 50);
    frenetFrameBox[4] = QVector3D(-50, -50, 50);
    frenetFrameBox[5] = QVector3D(-50 , -50, -50);
    frenetFrameBox[6] = QVector3D(50, -50, -50);
    frenetFrameBox[7] = QVector3D(50, -50, 50);

    showControlPoints = true;
    showControlLines = false;
    showCatmullRom = true;
    showGeneralizedCylinder = false;
    showFrenetFrameBox = false;
    drawCylinderEnabled = false;
    drawWireFrameEnabled = false;

    time.start();
}

catmull::~catmull()
{

}

void catmull::mousePressEvent( int x, int y, int z, int button, Mode window )
{
    cpt= -1;

	if (mousedown) return;
    mousedown = true;
	// left to select and move right to add new point
    if (button == Qt::RightButton) addPoint(x, y, z);
    if (button == Qt::LeftButton)  movePoint(x, y, z, window);
}

void catmull::mouseReleaseEvent()
{
	mousedown = false;
}

void catmull::mouseMoveEvent ( int x, int y, int z, Mode window )
{
    if (mousedown) movePoint(x, y, z, window);
}

void catmull::animate()
{
    animatenow=true;
    param =  0;
    nframe=0;
    seg = 0;
}

void catmull::tensionSlider(int tensionValueL)
{
    tensionValue = tensionValueL;
}

void catmull::controlPoints(bool showControlPointsL)
{
    showControlPoints = showControlPointsL;
}

void catmull::controlLines(bool showControlLinesL)
{
    showControlLines = showControlLinesL;
}

void catmull::catmullRom(bool showCatmullRomL)
{
    showCatmullRom = showCatmullRomL;
}

void catmull::addPoint(int x, int y, int z)
{
	if (lastpt<MAXP) {
        pnts[lastpt][X]=x;
        pnts[lastpt][Y]=y;
        pnts[lastpt][Z]=z;
		lastpt++;
    }
}

void catmull::movePoint(int x, int y, int z, Mode window)
{	
    if (cpt < 0) cpt = select(x, y, z, window);
    if (cpt>=0)
    {
        if (window == XY)
        {
            pnts[cpt][0]=x;
            pnts[cpt][1]=y;
        }
        else if (window == XZ)
        {
            pnts[cpt][0]=x;
            pnts[cpt][2]=z;
        }
        else  // window == ZY
        {
            pnts[cpt][1]=y;
            pnts[cpt][2]=z;
        }
	}
}

int catmull::select(int x, int y, int z, Mode window)
{
    int i;

    if (window == XY)
    {
        for (i=0; i<lastpt; i++) {
            if (   ( (pnts[i][0]-RADIUS) <x && (pnts[i][0]+RADIUS) >x )
                && ( (pnts[i][1]-RADIUS) <y && (pnts[i][1]+RADIUS) >y ) )
                return i;
        }
    }
    else if (window == XZ)
    {
        for (i=0; i<lastpt; i++) {
            if (   ( (pnts[i][0]-RADIUS) <x && (pnts[i][0]+RADIUS) >x )
                && ( (pnts[i][2]-RADIUS) <z && (pnts[i][2]+RADIUS) >z ) )
                return i;
        }
    }
    else  // window == ZY
    {
        for (i=0; i<lastpt; i++) {
            if (   ( (pnts[i][1]-RADIUS) <y && (pnts[i][1]+RADIUS) >y )
                && ( (pnts[i][2]-RADIUS) <z && (pnts[i][2]+RADIUS) >z ) )
                return i;
        }
    }
    return -1;
}

bool catmull::nearzero(double x)
{
	if ( fabs(x) > -0.01  && fabs(x) < 0.01) return true;
	else return false;
}

/** CATMULL-ROM **/

void catmull::draw()
{
    int x0,x1,y0,y1,z0,z1;

    QVector<QVector3D> catPoints;
    QVector< QVector<QVector3D> > genCylPoints;

	glColor3f(1.0f, 0.0f, 0.1f);
	x1 = pnts[0][0];
    y1 = pnts[0][1];
    z1 = pnts[0][2];

    for (int i = 0; i < lastpt; i++)
    {
		x0=x1;
		y0=y1;
        z0=z1;
		x1 = pnts[i][0];
		y1 = pnts[i][1];
        z1 = pnts[i][2];
        if (showControlPoints)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            drawPoint(double(x1), double(y1), double(z1), 10);
        }
        if (showControlLines)
        {
            glColor3f(1.0f, 0.0f, 0.1f);
            drawLine(x0, y0, z0, x1, y1, z1);
        }
        x0 = x1;  y0 = y1; z0 = z1;
	}

    if (lastpt > 3)  // There are at least 4 control points
    {
        catPoints = findCatPoints();
        genCylPoints = findGenCylPoints(catPoints);
        if (showCatmullRom)
            drawCatmull(catPoints);
        if (showGeneralizedCylinder)
            drawGenCyl(genCylPoints, catPoints);
    }
}

// Draws the frenet Frame box at the currently stored global coordinates
void catmull::drawFrenetFrameBox()
{
    glColor3f(0.129f, 0.850f, 0.768f);
    //glColor3f(1.0f, 0.0f, 0.0f);

    // Draw Top of Frenet Box
    drawLine(frenetFrameBox[0].x(), frenetFrameBox[0].y(), frenetFrameBox[0].z(),
             frenetFrameBox[1].x(), frenetFrameBox[1].y(), frenetFrameBox[1].z());
    drawLine(frenetFrameBox[1].x(), frenetFrameBox[1].y(), frenetFrameBox[1].z(),
             frenetFrameBox[2].x(), frenetFrameBox[2].y(), frenetFrameBox[2].z());
    drawLine(frenetFrameBox[2].x(), frenetFrameBox[2].y(), frenetFrameBox[2].z(),
             frenetFrameBox[3].x(), frenetFrameBox[3].y(), frenetFrameBox[3].z());
    drawLine(frenetFrameBox[3].x(), frenetFrameBox[3].y(), frenetFrameBox[3].z(),
             frenetFrameBox[0].x(), frenetFrameBox[0].y(), frenetFrameBox[0].z());

    // Draw Bottom of Frenet Box
    drawLine(frenetFrameBox[4].x(), frenetFrameBox[4].y(), frenetFrameBox[4].z(),
             frenetFrameBox[5].x(), frenetFrameBox[5].y(), frenetFrameBox[5].z());
    drawLine(frenetFrameBox[5].x(), frenetFrameBox[5].y(), frenetFrameBox[5].z(),
             frenetFrameBox[6].x(), frenetFrameBox[6].y(), frenetFrameBox[6].z());
    drawLine(frenetFrameBox[6].x(), frenetFrameBox[6].y(), frenetFrameBox[6].z(),
             frenetFrameBox[7].x(), frenetFrameBox[7].y(), frenetFrameBox[7].z());
    drawLine(frenetFrameBox[7].x(), frenetFrameBox[7].y(), frenetFrameBox[7].z(),
             frenetFrameBox[4].x(), frenetFrameBox[4].y(), frenetFrameBox[4].z());

    // Draw Middle of Frenet Box
    drawLine(frenetFrameBox[0].x(), frenetFrameBox[0].y(), frenetFrameBox[0].z(),
             frenetFrameBox[4].x(), frenetFrameBox[4].y(), frenetFrameBox[4].z());
    drawLine(frenetFrameBox[1].x(), frenetFrameBox[1].y(), frenetFrameBox[1].z(),
             frenetFrameBox[5].x(), frenetFrameBox[5].y(), frenetFrameBox[5].z());
    drawLine(frenetFrameBox[2].x(), frenetFrameBox[2].y(), frenetFrameBox[2].z(),
             frenetFrameBox[6].x(), frenetFrameBox[6].y(), frenetFrameBox[6].z());
    drawLine(frenetFrameBox[3].x(), frenetFrameBox[3].y(), frenetFrameBox[3].z(),
             frenetFrameBox[7].x(), frenetFrameBox[7].y(), frenetFrameBox[7].z());
}

// Finds all of the sub-points needed to construct the catmull rom spline with drawLine()
QVector<QVector3D> catmull::findCatPoints()
{
    QVector<QVector3D> catPoints = QVector<QVector3D>();
    double stepX, stepY, stepZ;
    double t = double(tensionValue)/50.0;
    double step = 1.0/double(numSteps);
    double u;

    for (int i = 0; i < (lastpt - 3); i++)  // Loop through the number of segments (# control points - 3)
    {
        u = 0;
        for (int j = 0; j < numSteps; j++)  // Loop through the steps in each segment
        {
            stepX = (double(pnts[i  ][0]) * (-t*u + 2*t*u*u - t*u*u*u) + \
                     double(pnts[i+1][0]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                     double(pnts[i+2][0]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                     double(pnts[i+3][0]) * (-t*u*u + t*u*u*u));
            stepY = (double(pnts[i  ][1]) * ((-t*u + 2*t*u*u - t*u*u*u)) + \
                     double(pnts[i+1][1]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                     double(pnts[i+2][1]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                     double(pnts[i+3][1]) * (-t*u*u + t*u*u*u));
            stepZ = (double(pnts[i  ][2]) * ((-t*u + 2*t*u*u - t*u*u*u)) + \
                     double(pnts[i+1][2]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                     double(pnts[i+2][2]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                     double(pnts[i+3][2]) * (-t*u*u + t*u*u*u));
            catPoints.append(QVector3D(stepX, stepY, stepZ));

            u += step;
        }
    }
    return catPoints;
}

// Give all of the catmull rom points are given, will draw lines between the those points
void catmull::drawCatmull(QVector<QVector3D> catPoints)
{
    glColor3f(0.129f, 0.850f, 0.768f);
    for (int i = 0; i < (catPoints.size() - 1); i++)
    {
        drawLine(catPoints[i].x(), catPoints[i].y(), catPoints[i].z(),
                 catPoints[i+1].x(), catPoints[i+1].y(), catPoints[i+1].z());
    }
}

/** GENERALIZED CYLINDER **/

// Finds the circular points around the catmull rom points given to it.  Requires frenet calculation
QVector< QVector<QVector3D> > catmull::findGenCylPoints(QVector<QVector3D> catPoints)
{
    QVector< QVector<QVector3D> > genCylPoints = QVector< QVector<QVector3D> >();
    QVector3D w, vel0, vel1, acc, norm, biNorm;
    double velX, velY, velZ, accX, accY, accZ, theta;
    double t = double(tensionValue)/50.0;
    double step = 1.0/double(numSteps);

    double u = 0;

    // Calculate the first frenet frame using absolute calculations
    velX = (double(pnts[0][0]) * (-t + 4*t*u - 3*t*u*u) + \
            double(pnts[1][0]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
            double(pnts[2][0]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
            double(pnts[3][0]) * (-2*t*u + 3*t*u*u));
    velY = (double(pnts[0][1]) * (-t + 4*t*u - 3*t*u*u) + \
            double(pnts[1][1]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
            double(pnts[2][1]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
            double(pnts[3][1]) * (-2*t*u + 3*t*u*u));
    velZ = (double(pnts[0][2]) * (-t + 4*t*u - 3*t*u*u) + \
            double(pnts[1][2]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
            double(pnts[2][2]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
            double(pnts[3][2]) * (-2*t*u + 3*t*u*u));

    accX = (double(pnts[0][0]) * (4*t - 6*t*u) + \
            double(pnts[1][0]) * (2*(t-3) + 6*(2-t)*u) + \
            double(pnts[2][0]) * (2*(3-2*t) + 6*(t-2)*u) + \
            double(pnts[3][0]) * (-2*t + 6*t*u));
    accY = (double(pnts[0][1]) * (4*t - 6*t*u) + \
            double(pnts[1][1]) * (2*(t-3) + 6*(2-t)*u) + \
            double(pnts[2][1]) * (2*(3-2*t) + 6*(t-2)*u) + \
            double(pnts[3][1]) * (-2*t + 6*t*u));
    accZ = (double(pnts[0][2]) * (4*t - 6*t*u) + \
            double(pnts[1][2]) * (2*(t-3) + 6*(2-t)*u) + \
            double(pnts[2][2]) * (2*(3-2*t) + 6*(t-2)*u) + \
            double(pnts[3][2]) * (-2*t + 6*t*u));

    // Create the velocity and acceleration vectors
    vel0 = QVector3D(velX, velY, velZ).normalized();
    acc = QVector3D(accX, accY, accZ).normalized();

    // Binormal vector is vel X acc
    biNorm = QVector3D::crossProduct(vel0, acc).normalized();

    // Normal vector is vel X biNorm
    norm = QVector3D::crossProduct(vel0, biNorm).normalized();

    for (int i = 0; i < (lastpt - 3); i++)  // Loop through the number of segments (# control points - 3)
    {
        u = 0;
        for (int j = 0; j < numSteps; j++)  // Loop through the steps in each segment
        {
            velX = (double(pnts[i  ][0]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnts[i+1][0]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnts[i+2][0]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnts[i+3][0]) * (-2*t*u + 3*t*u*u));
            velY = (double(pnts[i  ][1]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnts[i+1][1]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnts[i+2][1]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnts[i+3][1]) * (-2*t*u + 3*t*u*u));
            velZ = (double(pnts[i  ][2]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnts[i+1][2]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnts[i+2][2]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnts[i+3][2]) * (-2*t*u + 3*t*u*u));
            vel1 = QVector3D(velX, velY, velZ).normalized();

            w = QVector3D::crossProduct(vel0, vel1).normalized();

            theta = acos(QVector3D::dotProduct(vel0, vel1)/(vel0.length()*vel1.length()));

            biNorm = (cos(theta)*biNorm + (sin(theta))*(QVector3D::crossProduct(w, biNorm)) + \
                      (1 - cos(theta))*(QVector3D::dotProduct(w, biNorm))*w).normalized();

            norm = QVector3D::crossProduct(vel1, biNorm).normalized();

            genCylPoints.append(find3dCirclePoints(norm, biNorm,
                QVector3D(catPoints[(i*numSteps) + j].x(), catPoints[(i*numSteps) + j].y(), catPoints[(i*numSteps) + j].z())));


            // Moves the frenet frame cube every X milliseconds
            if (frenetFrameBoxIndex >= catPoints.size())
                frenetFrameBoxIndex = 0;
            if ( time.elapsed() > 20 && frenetFrameBoxIndex == ((i * numSteps) + j) )
            {
                time.restart();

                if (++frenetFrameBoxIndex >= catPoints.size())
                    frenetFrameBoxIndex = 0;
            }

            if (showFrenetFrameBox && frenetFrameBoxIndex == ((i * numSteps) + j))
            {
                frenetFrameBox[0].setX( catPoints[frenetFrameBoxIndex].x() + norm.x()*fbSize - biNorm.x()*fbSize - vel1.x()*fbSize);
                frenetFrameBox[0].setY( catPoints[frenetFrameBoxIndex].y() + norm.y()*fbSize - biNorm.y()*fbSize - vel1.y()*fbSize);
                frenetFrameBox[0].setZ( catPoints[frenetFrameBoxIndex].z() + norm.z()*fbSize - biNorm.z()*fbSize - vel1.z()*fbSize);

                frenetFrameBox[1].setX( catPoints[frenetFrameBoxIndex].x() + norm.x()*fbSize - biNorm.x()*fbSize + vel1.x()*fbSize);
                frenetFrameBox[1].setY( catPoints[frenetFrameBoxIndex].y() + norm.y()*fbSize - biNorm.y()*fbSize + vel1.y()*fbSize);
                frenetFrameBox[1].setZ( catPoints[frenetFrameBoxIndex].z() + norm.z()*fbSize - biNorm.z()*fbSize + vel1.z()*fbSize);

                frenetFrameBox[2].setX( catPoints[frenetFrameBoxIndex].x() + norm.x()*fbSize + biNorm.x()*fbSize + vel1.x()*fbSize);
                frenetFrameBox[2].setY( catPoints[frenetFrameBoxIndex].y() + norm.y()*fbSize + biNorm.y()*fbSize + vel1.y()*fbSize);
                frenetFrameBox[2].setZ( catPoints[frenetFrameBoxIndex].z() + norm.z()*fbSize + biNorm.z()*fbSize + vel1.z()*fbSize);

                frenetFrameBox[3].setX( catPoints[frenetFrameBoxIndex].x() + norm.x()*fbSize + biNorm.x()*fbSize - vel1.x()*fbSize);
                frenetFrameBox[3].setY( catPoints[frenetFrameBoxIndex].y() + norm.y()*fbSize + biNorm.y()*fbSize - vel1.y()*fbSize);
                frenetFrameBox[3].setZ( catPoints[frenetFrameBoxIndex].z() + norm.z()*fbSize + biNorm.z()*fbSize - vel1.z()*fbSize);

                frenetFrameBox[4].setX( catPoints[frenetFrameBoxIndex].x() - norm.x()*fbSize - biNorm.x()*fbSize - vel1.x()*fbSize);
                frenetFrameBox[4].setY( catPoints[frenetFrameBoxIndex].y() - norm.y()*fbSize - biNorm.y()*fbSize - vel1.y()*fbSize);
                frenetFrameBox[4].setZ( catPoints[frenetFrameBoxIndex].z() - norm.z()*fbSize - biNorm.z()*fbSize - vel1.z()*fbSize);

                frenetFrameBox[5].setX( catPoints[frenetFrameBoxIndex].x() - norm.x()*fbSize - biNorm.x()*fbSize + vel1.x()*fbSize);
                frenetFrameBox[5].setY( catPoints[frenetFrameBoxIndex].y() - norm.y()*fbSize - biNorm.y()*fbSize + vel1.y()*fbSize);
                frenetFrameBox[5].setZ( catPoints[frenetFrameBoxIndex].z() - norm.z()*fbSize - biNorm.z()*fbSize + vel1.z()*fbSize);

                frenetFrameBox[6].setX( catPoints[frenetFrameBoxIndex].x() - norm.x()*fbSize + biNorm.x()*fbSize + vel1.x()*fbSize);
                frenetFrameBox[6].setY( catPoints[frenetFrameBoxIndex].y() - norm.y()*fbSize + biNorm.y()*fbSize + vel1.y()*fbSize);
                frenetFrameBox[6].setZ( catPoints[frenetFrameBoxIndex].z() - norm.z()*fbSize + biNorm.z()*fbSize + vel1.z()*fbSize);

                frenetFrameBox[7].setX( catPoints[frenetFrameBoxIndex].x() - norm.x()*fbSize + biNorm.x()*fbSize - vel1.x()*fbSize);
                frenetFrameBox[7].setY( catPoints[frenetFrameBoxIndex].y() - norm.y()*fbSize + biNorm.y()*fbSize - vel1.y()*fbSize);
                frenetFrameBox[7].setZ( catPoints[frenetFrameBoxIndex].z() - norm.z()*fbSize + biNorm.z()*fbSize - vel1.z()*fbSize);

                drawFrenetFrameBox();
            }

            vel0 = vel1;

            u += step;
        }
    }
    return genCylPoints;
}

void catmull::drawGenCyl(QVector< QVector<QVector3D> > genCylPoints, QVector<QVector3D> catPoints)
{
    for (int i = 0; i < (genCylPoints.size() - 1); i++)
    {
        if (drawCylinderEnabled)
            drawCylinder(genCylPoints[i], genCylPoints[i + 1], catPoints[i+1]);
        else  // (drawWireFrameEnabled || drawWireFrameHeart)
            drawWireFrame(genCylPoints[i], genCylPoints[i + 1]);
    }
}

// Given the norm, biNorm, and point, this will calculate the actual points to make the ring of a cylinder
QVector<QVector3D> catmull::find3dCirclePoints(QVector3D norm, QVector3D biNorm, QVector3D point)
{
    double cX, cY;
    QVector<QVector3D> cPoints;
    //radius = 50;
    for (int i = 0; i < numCircleSegs; i++)
    {
        if (drawWireFrameEnabled || drawCylinderEnabled)
        {
            cX = radius*cos((2*M_PI*i)/numCircleSegs);
            cY = radius*sin((2*M_PI*i)/numCircleSegs);
        }
        else
        {
            cX = (radius/10)*(16*sin((2*M_PI*i)/numCircleSegs)*sin((2*M_PI*i)/numCircleSegs)*sin((2*M_PI*i)/numCircleSegs));
            cY = (radius/10)*(13*cos(((2*M_PI*i)/numCircleSegs)) - 5*cos(2*((2*M_PI*i)/numCircleSegs)) -\
                    2*cos(3*((2*M_PI*i)/numCircleSegs)) - cos(4*((2*M_PI*i)/numCircleSegs)));
        }

        cPoints.append(QVector3D(point.x() + cX*norm.x() + cY*biNorm.x(),
                                 point.y() + cX*norm.y() + cY*biNorm.y(),
                                 point.z() + cX*norm.z() + cY*biNorm.z()));
    }
    return cPoints;
}

void catmull::drawCylinder(QVector<QVector3D> lastPoints, QVector<QVector3D> currentPoints, QVector3D center)
{
    QVector3D t0, t1, b0, b1, normal;
    b1 = lastPoints[lastPoints.size() - 1];
    t1 = currentPoints[currentPoints.size() - 1];

    glColor3f(0.129f, 0.850f, 0.768f);

    for (int i = 0; i < numCircleSegs; i++)
    {
        b0 = b1;
        t0 = t1;

        b1 = lastPoints[i];
        t1 = currentPoints[i];

        glBegin(GL_TRIANGLES);
        glVertex3f(b1.x(), b1.y(), b1.z());
        glVertex3f(b0.x(), b0.y(), b0.z());
        glVertex3f(t0.x(), t0.y(), t0.z());
        glEnd();

        glBegin(GL_TRIANGLES);
        glVertex3f(b1.x(), b1.y(), b1.z());
        glVertex3f(t0.x(), t0.y(), t0.z());
        glVertex3f(t1.x(), t1.y(), t1.z());
        glEnd();

        normal = (currentPoints[i] - center).normalized();
        glNormal3d(normal.x(), normal.y(), normal.z());
        glVertex3d(currentPoints[i].x(), currentPoints[i].y(), currentPoints[i].z());
    }
}

void catmull::drawWireFrame(QVector<QVector3D> lastPoints, QVector<QVector3D> currentPoints)
{
    QVector3D t0, t1, b0, b1;
    b1 = lastPoints[lastPoints.size() - 1];
    t1 = currentPoints[currentPoints.size() - 1];

    glColor3f(0.129f, 0.850f, 0.768f);
    for (int i = 0; i < numCircleSegs; i++)
    {
        b0 = b1;
        t0 = t1;

        b1 = lastPoints[i];
        t1 = currentPoints[i];

        drawLine(b0.x(), b0.y(), b0.z(), b1.x(), b1.y(), b1.z());
        drawLine(t0.x(), t0.y(), t0.z(), t1.x(), t1.y(), t1.z());
        drawLine(b1.x(), b1.y(), b1.z(), t1.x(), t1.y(), t1.z());
    }
}

/** HELPER FUNCTIONS **/

QVector3D catmull::vectorTransform(QVector3D v, QMatrix3x3 m)
{
    QVector3D transformed = QVector3D();
    transformed.setX( ((m(0,0))*v.x()) + ((m(0,1))*v.y()) + ((m(0,2))*v.z()) );
    transformed.setY( ((m(1,0))*v.x()) + ((m(1,1))*v.y()) + ((m(1,2))*v.z()) );
    transformed.setZ( ((m(2,0))*v.x()) + ((m(2,1))*v.y()) + ((m(2,2))*v.z()) );
    return transformed;
}

/** All the helper functions for the UI to function properly **/

void catmull::genCylRadius(int tradius)
{
    radius = tradius;
}

void catmull::genCylCircComplexity(int complexity)
{
    numCircleSegs = complexity;
}

void catmull::genCylSegComplexity(int complexity)
{
    numSteps = complexity;
}

void catmull::genCyl(bool enabled)
{
    showGeneralizedCylinder = !enabled;
}

void catmull::triangleMesh(bool enabled)
{
    drawCylinderEnabled = enabled;
}

void catmull::wireFrameCyl(bool enabled)
{
    drawWireFrameEnabled = enabled;
}

void catmull::frenetFrameBoxFunc(bool enabled)
{
    showFrenetFrameBox = enabled;
}
