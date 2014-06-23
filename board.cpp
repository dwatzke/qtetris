#include "board.h"
#include "brick.h"
#include "square.h"

#include <QDebug>
#include <QGridLayout>
#include <QLayoutItem>
#include <QMessageBox>
#include <QShortcut>
#include <QTimer>

#include <QtAlgorithms>

const int Board::ROWS = 20;
const int Board::COLUMNS = 10;

Board::Board(QWidget *parent) :
	QWidget(parent),
	m_layout(new QGridLayout(this)),
	m_timer(new QTimer(this)),
	m_brick(0),
	m_brickPos(),
	m_lines(0)
{
	/* remove spaces between the widgets in the layout */
	m_layout->setSpacing(0);
	m_layout->setMargin(0);

	/* add some buttons into the layout */
	for (int row = 0; row < Board::ROWS; row++) {
		for(int col = 0; col < Board::COLUMNS; col++) {
			Square * sq = new Square(this);
			m_layout->addWidget(sq, row, col);
		}
	}

	/* fill the brick list */
	QList<QList<QPoint> > pointLists;
	QList<QPoint> points;

	/* :: */
	points << QPoint(0,0) << QPoint(0,1)
	       << QPoint(1,0) << QPoint(1,1);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::green, this);
	pointLists.clear();

	/* .:. */
	points <<                QPoint(1,0)
	       << QPoint(0,1) << QPoint(1,1) << QPoint(2,1);
	pointLists << points;
	points.clear();
	points << QPoint(0,0)
	       << QPoint(0,1) << QPoint(1,1)
	       << QPoint(0,2);
	pointLists << points;
	points.clear();
	points << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
			      << QPoint(1,1);
	pointLists << points;
	points.clear();
	points	              << QPoint(1,0)
	       << QPoint(0,1) << QPoint(1,1)
			      << QPoint(1,2);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::red, this);
	pointLists.clear();

	/* .... */
	points << QPoint(0,0) << QPoint(1,0) << QPoint(2,0) << QPoint(3,0);
	pointLists << points;
	points.clear();
	points << QPoint(0,0)
	       << QPoint(0,1)
	       << QPoint(0,2)
	       << QPoint(0,3);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::cyan, this);
	pointLists.clear();

	/* :.. */
	points << QPoint(0,0)
	       << QPoint(0,1) << QPoint(1,1) << QPoint(2,1);
	pointLists << points;
	points.clear();
	points << QPoint(0,0) << QPoint(1,0)
	       << QPoint(0,1)
	       << QPoint(0,2);
	pointLists << points;
	points.clear();
	points << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
					     << QPoint(2,1);
	pointLists << points;
	points.clear();
	points                << QPoint(1,0)
			      << QPoint(1,1)
	       << QPoint(0,2) << QPoint(1,2);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::blue, this);
	pointLists.clear();

	/* ..: */
	points << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
	       << QPoint(0,1);
	pointLists << points;
	points.clear();
	points << QPoint(0,0) << QPoint(1,0)
			      << QPoint(1,1)
			      << QPoint(1,2);
	pointLists << points;
	points.clear();
	points		                     << QPoint(2,0)
	       << QPoint(0,1) << QPoint(1,1) << QPoint(2,1);
	pointLists << points;
	points.clear();
	points << QPoint(0,0)
	       << QPoint(0,1)
	       << QPoint(0,2) << QPoint(1,2);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::darkCyan, this);
	pointLists.clear();

	/* ':. */
	points << QPoint(0,0) << QPoint(1,0)
			      << QPoint(1,1) << QPoint(2,1);
	pointLists << points;
	points.clear();
	points                << QPoint(1,0)
	       << QPoint(0,1) << QPoint(1,1)
	       << QPoint(0,2);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::magenta, this);
	pointLists.clear();

	/* .:' */
	points                << QPoint(1,0) << QPoint(2,0)
	       << QPoint(0,1) << QPoint(1,1);
	pointLists << points;
	points.clear();
	points << QPoint(0,0)
	       << QPoint(0,1) << QPoint(1,1)
			      << QPoint(1,2);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::darkGreen, this);
	pointLists.clear();

	emit lines(0);

	connect(new QShortcut(QKeySequence(Qt::Key_Left ), this), SIGNAL(activated()), this, SLOT(moveLeft()));
	connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveRight()));
	connect(new QShortcut(QKeySequence(Qt::Key_Down ), this), SIGNAL(activated()), this, SLOT(moveDown()));
	connect(new QShortcut(QKeySequence(Qt::Key_Space), this), SIGNAL(activated()), this, SLOT(moveFall()));
	connect(new QShortcut(QKeySequence(Qt::Key_Up   ), this), SIGNAL(activated()), this, SLOT(rotate()));

	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerMoveDown()));
	/* the first brick would fall automatically but the rotate
	 * would crash the game if pressed before the first timer timeout occured */
	this->dropBrick();
	m_timer->setInterval(400);
	m_timer->start();
}

/** Carries out the automatic move down.
 * Either drops a new brick or it moves down the currently falling one.
 */
void Board::timerMoveDown()
{
	this->moveBrick(Down);
}

/** Drops a new brick.
 * Assumes that all other bricks are stuck down.
 */
void Board::dropBrick()
{
	m_timer->start();
	m_brick = m_brickList.at(qrand() % m_brickList.size());
	m_brickPos = QPoint((Board::COLUMNS / 2) - (m_brick->width() / 2), 0 - m_brick->height());
}

/** Moves the currently falling brick.
 * Assumes that there actually is a falling brick.
 * Sets m_brickFalling = false if the brick just got stuck
 */
BrickMoveResult Board::moveBrick(BrickMoveDirection move)
{
	/* undraw the previous brick state if any */
	this->drawBrick(false);

	/* determine if the move causes collision and proceed with it if not */
	BrickMoveResult res = this->checkAndMakeBrickMove(move);

	/* handle the game over */
	if (res == GameOver)
		this->gameOver();

	/* draw the new brick state */
	this->drawBrick();

	if (res == Collision) {
		this->removeFilledRows();
		this->dropBrick();
	}

	return res;
}

void Board::removeFilledRows() {
	//qDebug() << "removing filled rows";

	/** 'upper' and 'lower' have opposite meaning in graphical
	 * and gridlayout-indexing point of view - I'm talking about
	 * the graphical point of view
	 * (i.e. lower squares have higher row indexes)
	 */
	const int upperBound = m_brickPos.y();
	const int lowerBound = upperBound + m_brick->height()-1;

	QList<int> filledRowList;
	QList<Square*> filledSquareList;
	/* get list of filled rows (and their squares) */
	for (int row = lowerBound; row >= upperBound; row--) {
		bool filled = true;
		for (int col = 0; col < Board::COLUMNS; col++) {
			QLayoutItem *item = m_layout->itemAtPosition(row, col);

			if (!item) {
				filled = false;
				break;
			}

			Square *sq = qobject_cast<Square*>(item->widget());
			if (!sq->isOccupied()) {
				filled = false;
				break;
			}
		}

		if (filled) {
			//qDebug() << "!! row" << row << "is filled";

			for (int col = 0; col < Board::COLUMNS; col++) {
				Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row, col)->widget());
				sq->freeSquare();
				filledSquareList << sq;
			}
			filledRowList << row;
		}
	}

	/* if there are no filled rows, then this is a no-op */
	if (filledRowList.isEmpty())
		return;

	/* move the non-filled rows down over filled rows */
	int shift = 1;
	for (int row = filledRowList.first()-1; row >= 0; row--) {
		if (filledRowList.contains(row)) {
			shift++;
		} else {
			this->moveRow(row, shift);
		}
	}

	/* put 'overwritten' squares from filled-rows (that were already freed)
	 * back into the board from the top */
	int sqIndex = 0;
	for (int row = 0; row < filledRowList.size(); row++) {
		for(int col = 0; col < Board::COLUMNS; col++) {
			m_layout->addWidget(filledSquareList[sqIndex], row, col);
			sqIndex++;
		}
	}

	m_lines += filledRowList.size();
	emit lines(m_lines);

	int newInterval = 500 - (m_lines / 10) * 50;

	if (newInterval <= 50) {
		this->gameOver();
	}

	m_timer->setInterval(newInterval);

	Q_ASSERT(sqIndex == filledSquareList.size());
}

/** Moves row (graphically) down by 'shift' rows */
void Board::moveRow(const int row, const int shift) {
	//qDebug() << "shifting row" << row << "by" << shift;
	for(int col = 0; col < Board::COLUMNS; col++) {
		QWidget *w = m_layout->itemAtPosition(row, col)->widget();
		m_layout->addWidget(w, row+shift, col);
	}
}

/** Stops the game.
  * Stop the main timer(s), show info to player, offer Retry
  */
void Board::gameOver(QString msg) {
	disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerMoveDown()));

	int ret;
	if (msg.isEmpty()) {
		ret = QMessageBox::critical(this,
			tr("Game over"), tr("Game over - bricks collided!"),
			QMessageBox::Retry, QMessageBox::Close);
	} else {
		ret = QMessageBox::information(this,
			tr("Game over"), tr("Game over - you have won at tetris! Congratulations, Chuck Norris."),
			QMessageBox::Retry, QMessageBox::Close);
	}

	switch (ret) {
		case QMessageBox::Retry:
			emit gameReset();
			break;
		default:
			/* noop */
			break;
	}
}

BrickMoveResult Board::checkBrickMove(const QPoint position, const BrickMoveDirection move)
{
	/* horizontal board bounds */
	if (position.x() < 0 || position.x() > (Board::COLUMNS - m_brick->width())) {
		/* if we hit horizontal bounds while moving down, then that's a bug! */
		Q_ASSERT(move == Left || move == Right);
		return Noop;
	}

	/* vertical end of board */
	if ((position.y() + m_brick->height()) > Board::ROWS)
		return Collision;

	/* check if any of the squares that new position requires is occupied */
	const QList<QPoint> &pointList = m_brick->pointList();
	foreach (QPoint p, pointList) {
		int col = p.x() + position.x();
		int row = p.y() + position.y();

		if (row < 0)
			continue;

		Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row, col)->widget());

		/* if square is occupied */
		if (sq->isOccupied()) {
			/* if we wanted to move left or right */
			if (move == Left || move == Right) {
				/* do nothing */
				return Noop;
			} else {
				/* if collision occurs on the first row, it's gameover */
				if (row != 0)
					return Collision;
				else
					return GameOver;
			}
		}
	}

	return Ok;
}

/** Check if changing brick position using 'change' parameter results in
  * valid or invalid move. If it's valid, change the position.
  */
BrickMoveResult Board::checkAndMakeBrickMove(BrickMoveDirection move)
{
	/* prepare position change based on direction of movement */
	QPoint change;
	switch (move) {
	case Left:
		change = QPoint(-1, 0);
		break;
	case Right:
		change = QPoint(1, 0);
		break;
	case Down:
		change = QPoint(0, 1);
		break;
	}

	BrickMoveResult res = this->checkBrickMove(m_brickPos + change, move);

	if (res == Ok)
		/* shift position */
		m_brickPos += change;
	else
		return res;

	return Ok;
}

/** Draws (or undraws if draw=false) current brick on m_brickPos.
  */
void Board::drawBrick(bool draw)
{
	const QList<QPoint> &pointList = m_brick->pointList();
	const QColor& color = m_brick->brickColor();

	foreach (QPoint p, pointList) {
		int col = p.x() + m_brickPos.x();
		int row = p.y() + m_brickPos.y();

		QLayoutItem *item = m_layout->itemAtPosition(row, col);
		if (item) {
			Square *sq = qobject_cast<Square*>(item->widget());
			if (draw) {
				//qDebug() << "occupying row =" << row << ", col =" << col;
				sq->setSquareColor(color);
			} else {
				//qDebug() << "freeing row =" << row << ", col =" << col;
				sq->freeSquare();
			}
		}
	}
}

/** Rotate currently falling brick. */
void Board::rotate()
{
	/* undraw original brick */
	this->drawBrick(false);

	/* rotate the brick */
	m_brick->rotate();

	/* check rotate collision */
	if (this->checkBrickMove(m_brickPos, Right) != Ok)
		m_brick->unrotate();

	/* draw rotated brick */
	this->drawBrick();
}

void Board::moveLeft()
{
	this->moveBrick(Left);
}

void Board::moveRight()
{
	this->moveBrick(Right);
}

void Board::moveDown()
{
	this->moveBrick(Down);
}

void Board::moveFall()
{
	while (this->moveBrick(Down) == Ok) {};
}
