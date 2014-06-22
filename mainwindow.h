#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Board;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private:
	Board* m_board;

	void initialize();

public slots:
	void resetBoard();
};

#endif // MAINWINDOW_H
