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

    showControlPoints = true;
    showControlLines = true;
    showCatmullRom = true;
    showGeneralizedCylinder = true;
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

//        drawLine(b0.x(), b0.y(), b0.z(), t0.x(), t0.y(), t0.z());
        drawLine(b1.x(), b1.y(), b1.z(), t1.x(), t1.y(), t1.z());
    }
}

QVector3D catmull::vectorTransform(QVector3D v, QMatrix3x3 m)
{
    QVector3D transformed = QVector3D();
    transformed.setX( ((m(0,0))*v.x()) + ((m(0,1))*v.y()) + ((m(0,2))*v.z()) );
    transformed.setY( ((m(1,0))*v.x()) + ((m(1,1))*v.y()) + ((m(1,2))*v.z()) );
    transformed.setZ( ((m(2,0))*v.x()) + ((m(2,1))*v.y()) + ((m(2,2))*v.z()) );
    return transformed;
}

QVector<QVector3D> catmull::find2dCirclePoints(QVector3D biNorm, QVector3D norm, QVector3D point)
{
    double cX = 0.0;
    double cY = 1;
    QVector<QVector3D> cPoints;
    for (int i = 0; i < numCircleSegs; i++)
    {
        cX = radius*cos((2*M_PI*i)/numCircleSegs);
        cY = radius*sin((2*M_PI*i)/numCircleSegs);

        cPoints.append(QVector3D(point.x() + cX*norm.x() + cY*biNorm.x(),
                                 point.y() + cX*norm.y() + cY*biNorm.y(),
                                 point.z() + cX*norm.z() + cY*biNorm.z()));
        drawPoint(point.x() + cX*norm.x() + cY*biNorm.x(),
                  point.y() + cX*norm.y() + cY*biNorm.y(),
                  point.z() + cX*norm.z() + cY*biNorm.z(), 1);
    }
    return cPoints;
}

void catmull::drawCylinder(QVector<QVector3D> lastPoints, QVector<QVector3D> currentPoints)
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

    }
}

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
            glColor3f(0.2f, 0.8f, 0.1f);
            drawPoint(double(x1), double(y1), double(z1), 10);
        }
        if (showControlLines)
        {
            glColor3f(1.0f, 0.0f, 0.1f);
            drawLine(x0, y0, z0, x1, y1, z1);
        }
        x0 = x1;  y0 = y1; z0 = z1;

        // Draw the curve between each segment
//        if (i > 1 && i < (lastpt - 1))
//        {
//            if (i == 2)
//            {
//                vel0 = QVector3D(velX, velY, velZ).normalized();
//                acc = QVector3D(accX, accY, accZ).normalized();
//                biNorm = QVector3D::crossProduct(vel0, acc).normalized();
//                norm = QVector3D::crossProduct(vel0, biNorm).normalized();
//            }

//            drawCurve(pnts[i-2], pnts[i-1], pnts[i], pnts[i+1], vel0, norm, biNorm);
//        }
	}

    if (lastpt > 3)  // There are at least 4 control points
    {
        if (showCatmullRom)
        {
            catPoints = findCatPoints();
            drawCatmull(catPoints);
        }
        if (showGeneralizedCylinder)
        {
            //genCylPoints = findGenCylPoints();
            //drawGenCyl(genCylPoints);
        }
    }

}

void catmull::drawCurve(int pnt1[], int pnt2[], int pnt3[], int pnt4[], QVector3D vel0, QVector3D norm, QVector3D biNorm)
{
    // Save the original points per segment
    double stepX0 = double(pnt2[0]);
    double stepY0 = double(pnt2[1]);
    double stepZ0 = double(pnt2[2]);
    double stepX1, stepY1, stepZ1;
    double t = double(tensionValue)/50.0;
    double step = 1.0/double(numSteps);
    double u = step;

    // Generalized Cylinder Code
    double sqx, sqy, sqz, cos, cos1, xycos1, yzcos1, zxcos1, sin, xsin, ysin, zsin;
    double velX, velY, velZ;
    QVector3D vel1;
    QVector<QVector3D> lastPoints, currentPoints;

    currentPoints = find2dCirclePoints(biNorm, norm, QVector3D(stepX0, stepY0, stepZ0));

    // Each time through the loop, use a calculation on u to find the next step
    // Each step is a point to be drawn
    for (int i = 0; i < numSteps; i++)
    {
        stepX1 = (double(pnt1[0]) * (-t*u + 2*t*u*u - t*u*u*u) + \
                  double(pnt2[0]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                  double(pnt3[0]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                  double(pnt4[0]) * (-t*u*u + t*u*u*u));
        stepY1 = (double(pnt1[1]) * ((-t*u + 2*t*u*u - t*u*u*u)) + \
                  double(pnt2[1]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                  double(pnt3[1]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                  double(pnt4[1]) * (-t*u*u + t*u*u*u));
        stepZ1 = (double(pnt1[2]) * ((-t*u + 2*t*u*u - t*u*u*u)) + \
                  double(pnt2[2]) * (1 + (t-3)*u*u + (2-t)*u*u*u) + \
                  double(pnt3[2]) * (t*u + (3-2*t)*u*u + (t-2)*u*u*u) + \
                  double(pnt4[2]) * (-t*u*u + t*u*u*u));
        if (showCatmullRom)
        {
            glColor3f(0.129f, 0.850f, 0.768f);
            drawLine(stepX0, stepY0, stepZ0, stepX1, stepY1, stepZ1);
        }
        stepX0 = stepX1;
        stepY0 = stepY1;
        stepZ0 = stepZ1;

        if (showGeneralizedCylinder)
        {
            lastPoints = currentPoints;

            velX = (double(pnt1[0]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnt2[0]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnt3[0]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnt4[0]) * (-2*t*u + 3*t*u*u));
            velY = (double(pnt1[1]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnt2[1]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnt3[1]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnt4[1]) * (-2*t*u + 3*t*u*u));
            velZ = (double(pnt1[2]) * (-t + 4*t*u - 3*t*u*u) + \
                    double(pnt2[2]) * (2*(t-3)*u + 3*(2-t)*u*u) + \
                    double(pnt3[2]) * (t + 2*(3-2*t)*u + 3*(t-2)*u*u) + \
                    double(pnt4[2]) * (-2*t*u + 3*t*u*u));

            vel1 = QVector3D(velX, velY, velZ).normalized();

            // Following math is to calculate the next frenet frame based on rotation
            QVector3D A = QVector3D::crossProduct(vel0, vel1).normalized();
            sqx = A.x()*A.x();
            sqy = A.y()*A.y();
            sqz = A.z()*A.z();
            cos = QVector3D::dotProduct(vel0, vel1);
            cos1 = 1 - cos;
            xycos1 = A.x()*A.y()*cos1;
            yzcos1 = A.y()*A.z()*cos1;
            zxcos1 = A.z()*A.x()*cos1;
            sin = sqrt(1 - cos*cos);
            xsin = A.x()*sin;
            ysin = A.y()*sin;
            zsin = A.z()*sin;

            double values[] = {(sqx + ((1 - sqx) * cos)), (xycos1 + zsin), (zxcos1 - ysin),
                               (xycos1 - zsin), (sqy + ((1 - sqy) * cos)), (yzcos1 + xsin),
                               (zxcos1 + ysin), (yzcos1 - xsin), ((sqz + (1 - sqz) * cos))};
            QMatrix3x3 R = QMatrix3x3( values );

            biNorm = vectorTransform(biNorm ,R).normalized();
            norm = QVector3D::crossProduct(vel1, biNorm).normalized();

            currentPoints = find2dCirclePoints(biNorm, norm, QVector3D(stepX1, stepY1, stepZ1));

//            drawCylinder(lastPoints, currentPoints);
            drawWireFrame(lastPoints, currentPoints);
        vel0 = vel1;
        }
        u += step;
    }
    // Draw the final line per segment, to connect with the next segment
    if (showCatmullRom)
    {
        glColor3f(0.129f, 0.850f, 0.768f);
        drawLine(stepX1, stepY1, stepZ1, double(pnt3[0]), double(pnt3[1]), double(pnt3[2]));
    }
}

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

double catmull::arcLength(QVector3D arcStart, QVector3D arcEnd)
{
    // This function is not doing anything.  I may redefine it later and use it.
    return arcStart.x() + arcEnd.x();
}

void catmull::drawCatmull(QVector<QVector3D> catPoints)
{
    glColor3f(0.129f, 0.850f, 0.768f);
    for (int i = 0; i < (catPoints.size() - 1); i++)
    {
        drawLine(catPoints[i].x(), catPoints[i].y(), catPoints[i].z(),
                 catPoints[i+1].x(), catPoints[i+1].y(), catPoints[i+1].z());
    }
}

QVector< QVector<QVector3D> > catmull::findGenCylPoints()
{

}

void catmull::drawGenCyl(QVector< QVector<QVector3D> > genCylPoints)
{

}



