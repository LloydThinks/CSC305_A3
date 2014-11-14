#ifndef foundation_h
#define foundation_h

#define EPSILON 0.00001
#define SEP << " " <<
#define NL  << "\n"

// I can't seem to make the -LANG:bool=ON switch work so :

//---------------------------------------------------------------------------
// if C++ type `bool' is not defined with compiler being used ...

#ifdef NOBOOL
typedef int bool;

#define true  1
#define false 0
#endif

#define SEP << " " <<
#define NL  << "\n"
//---------------------------------------------------------------------------

#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>
#include <qmessagebox.h>
#include <QPainter>
//#include "matvec.h"
#include <QFile>
#include <stdio.h>
#include <QEvent>
#include <qfiledialog.h>
#include <qdebug.h>
#include <QVector3D>
#include <QVector>
#include <QMatrix3x3>
#include <QTime>



//const  double   M_PI = 3.1415925;
const  double pib5 = M_PI/5.0;
const  double torr = sin (2.0*pib5) / sin(pib5);
const  int  X = 0;
const  int  Y = 1;
const  int  Z = 2;

const double svth=1.0/7.0;
const double todeg = 180.0/M_PI;
const double r3 = sqrt(3.0);
const double r7 = sqrt(7.0);
const double r32= sqrt(3.0)/2.0;
const double ir7=1.0/sqrt(7.0);
const double ir72=0.5/sqrt(7.0);



class iostream;

//////////////////////////////////////////

struct Vector3d
{
public:
    Vector3d() { x = y = z = 0; }
public:
    double x, y, z;
};

struct Matrix33d
{
public:
    Matrix33d() {
        M11 = M12 = M13 = M21 = M22 = M23 = M31 = M32 = M33 = 0;
    }
public:
    double M11, M12, M13, M21, M22, M23, M31, M32, M33;
};

inline Vector3d MultiplyMatrix33Vec3(const Vector3d & vec_in, const Matrix33d & matrix_in)
{
    Vector3d result;
    result.x = vec_in.x * matrix_in.M11 + vec_in.y * matrix_in.M12 + vec_in.z * matrix_in.M13;
    result.y = vec_in.x * matrix_in.M21 + vec_in.y * matrix_in.M22 + vec_in.z * matrix_in.M23;
    result.z = vec_in.x * matrix_in.M31 + vec_in.y * matrix_in.M32 + vec_in.z * matrix_in.M33;
    return result;
}
#endif

