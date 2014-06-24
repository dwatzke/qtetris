#include "mainwindow.h"
#include "board.h"
#include "previewwidget.h"

#include <QLabel>
#include <QLCDNumber>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  m_board(0), m_lineLCD(new QLCDNumber(4)), m_previewWidget(new PreviewWidget)
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

	QToolBar *tb = this->addToolBar(tr("Stats"));
	m_lineLCD->setDecMode();
	m_lineLCD->setFixedHeight(36);
	QLabel *lcdLabel = new QLabel(tr("Lines "));
	lcdLabel->setStyleSheet("font-size: 20px");
	tb->addWidget(lcdLabel);
	tb->addWidget(m_lineLCD);
	tb->addSeparator();
	QLabel *previewLabel = new QLabel(tr("Next "));
	previewLabel->setStyleSheet("font-size: 20px");
	tb->addWidget(previewLabel);
	tb->addWidget(m_previewWidget);
}

void MainWindow::resetBoard()
{
	QWidget *oldBoard = this->centralWidget();

	m_board = new Board(this);
	connect(m_board, SIGNAL(gameReset()), SLOT(resetBoard()));
	connect(m_board, SIGNAL(lines(int)), SLOT(setLines(int)));
	connect(m_board, SIGNAL(nextBrick(Brick*)), SLOT(previewNextBrick(Brick*)));
	this->setCentralWidget(m_board);

	if (oldBoard)
		oldBoard->deleteLater();

	m_lineLCD->display(0);

	m_board->startGame();
}

void MainWindow::setLines(int number)
{
	m_lineLCD->display(number);
}

void MainWindow::previewNextBrick(Brick *brick) {
	m_previewWidget->drawBrick(brick);
}
