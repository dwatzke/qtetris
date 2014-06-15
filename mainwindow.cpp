#include "mainwindow.h"

#include "board.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  m_board(0)
{
	this->resetBoard();
}

void MainWindow::resetBoard()
{
	QWidget *oldBoard = this->centralWidget();

	m_board = new Board(this);
	connect(m_board, SIGNAL(gameReset()), this, SLOT(resetBoard()));
	this->setCentralWidget(m_board);

	if (oldBoard)
		oldBoard->deleteLater();
}

MainWindow::~MainWindow()
{
}
