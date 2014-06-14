#include "mainwindow.h"

#include "board.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  m_board(new Board(this))
{
	this->initialize();
}

void MainWindow::initialize()
{
	this->setCentralWidget(m_board);
}

MainWindow::~MainWindow()
{

}
