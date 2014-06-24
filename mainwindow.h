#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "brick.h"
#include "previewwidget.h"

class Board;
class QLCDNumber;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private:
	Board *m_board;
	QLCDNumber *m_lineLCD;
	PreviewWidget *m_previewWidget;

	void initialize();

public slots:
	void resetBoard();
	void setLines(int number);
	void previewNextBrick(Brick *brick);
};

#endif // MAINWINDOW_H
