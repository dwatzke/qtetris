#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

	void initialize();

public slots:
	void resetBoard();
	void setLines(int number);
};

#endif // MAINWINDOW_H
