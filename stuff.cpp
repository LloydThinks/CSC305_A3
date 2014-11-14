#include "stuff.h"

double dist(double x0, double y0, double x1, double y1)
{
	return sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) ) ;
}

void fatal()
{
  exit(99);
}

void drawPoint(double x, double y, double z, float radius) {
    glPointSize(radius);
    glBegin(GL_POINTS);
    glVertex3d(x, y, z);
    glEnd();
}

void drawLine(double x0, double y0, double x1, double y1 )
{
	glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glEnd();
}

void drawLine(double x0, double y0, double z0, double x1, double y1, double z1, double width )
{
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y1, z1);
    glEnd();
}
void backCol()
{
  glColor3f(1.0f, 1.0f, 0.9f);
}

int sn(double posThresh)
{
  if (myrand()>posThresh) return 1; else return -1;
}

double randRange(double a, double b)
{
  return fabs(a-b)*myrand()+a;
}

int irand(int max)
{
  return (int)(max*myrand());
}

double myrand()
{
  return rand()/(double)RAND_MAX;
}

void error(bool fatal)
{
  if (fatal) exit(-1);
}

/*int nearZero(double x)
{
    return (x <= EPSILON && x >= -EPSILON);
}
*/
int nearEpsilon(double x, double epsilon)
{
  return (x <= epsilon && x >= -epsilon);
}
