#ifndef stuff_h
#define stuff_h


#include "foundation.h"
#include <qgl.h>
#include <qmessagebox.h>

int nearEpsilon(double x, double epsilon);
//int nearZero(double x);
double myrand();
void fatal();
void drawPoint(double x, double y, double z, float radius);
void drawLine(double x0, double y0, double x1, double y1);
void drawLine(double x0, double y0, double z0, double x1, double y1, double z1 , double width = 1.0f);

void backCol();
int sn(double posThresh);
int irand(int max);
double randRange(double a, double b);
void error(bool fatal);
//QString today();
double dist(double x0, double y0, double x1, double y1);


#endif
