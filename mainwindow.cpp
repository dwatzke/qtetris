#include "mainwindow.h"
#include "board.h"

#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  m_board(0)
{
	this->initialize();
	this->resetBoard();
}

void MainWindow::initialize()
{
	QMenuBar *mb = this->menuBar();

	QMenu* fileMenu = mb->addMenu(tr("&File"));
	fileMenu->addAction(tr("&Reset game"), this, SLOT(resetBoard()));
	fileMenu->addAction(tr("&Quit"), this, SLOT(close()));

	mb->addMenu(fileMenu);
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
