//-------------------------------------------------------------------------------------------
//  Multiple viewports example
//-------------------------------------------------------------------------------------------
#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QColorDialog>
#include "ui_MainWindow.h"
#include "glwidget.h"
#include "twod.h"
#include "catmull.h"

class GLWidget;

//Create a window and load the components from generated code ui_MainWindow.h
class Window : public QDialog, public Ui::frmMain
{
	Q_OBJECT;

public:
	//Constructor 
	Window(QWidget *parent = 0);

public slots:
    void changeLookAt();
    void tensionSlider(int tensionValue);
    void groundPlane(bool showPlane);
    void axis(bool showAxis);
    void controlPoints(bool showControlPoints);
    void controlLines(bool showControlLines);
    void catmullRom(bool showCatmullRom);

	
private:
    GLWidget *m_glWidget;
    twod *sideview1;
    twod *sideview2;
    twod *sideview3;
    catmull *catt;

private slots:
    void clear();

protected:
     void  resizeEvent(QResizeEvent * );

};


#endif
