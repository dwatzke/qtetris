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
	/* TODO: add menu bar */
	/* QMenuBar *mb = new QMenuBar;

	QMenu* fileMenu = new QMenu(mb);
	fileMenu->addAction(tr("&Reset game"), this, SLOT(resetBoard()));

	mb->addMenu(fileMenu);

	this->setMenuBar(mb); */
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
