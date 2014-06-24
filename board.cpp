#include "board.h"
#include "brick.h"
#include "square.h"

#include <QDebug>
#include <QGridLayout>
#include <QLayoutItem>
#include <QMessageBox>
#include <QShortcut>
#include <QTime>
#include <QTimer>

#include <QtAlgorithms>

const int Board::ROWS = 20;
const int Board::COLUMNS = 10;
const int Board::START_INTERVAL = 500;

Board::Board(QWidget *parent) :
	QWidget(parent),
	m_layout(new QGridLayout(this)),
	m_timer(new QTimer(this)),
	m_brick(0), m_brickNext(0),
	m_lines(0)
{
	/* remove spaces between the widgets in the layout */
	m_layout->setSpacing(0);
	m_layout->setMargin(0);

	/* add some buttons into the layout */
	for (int row = 0; row < ROWS; row++) {
		for(int col = 0; col < COLUMNS; col++) {
			Square * sq = new Square(this);
			m_layout->addWidget(sq, row, col);
		}
	}

	this->initializeBrickList();

	connect(new QShortcut(QKeySequence(Qt::Key_Left ), this), SIGNAL(activated()), SLOT(moveLeft()));
	connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), SLOT(moveRight()));
	connect(new QShortcut(QKeySequence(Qt::Key_Down ), this), SIGNAL(activated()), SLOT(moveDown()));
	connect(new QShortcut(QKeySequence(Qt::Key_Space), this), SIGNAL(activated()), SLOT(moveFall()));
	connect(new QShortcut(QKeySequence(Qt::Key_Up   ), this), SIGNAL(activated()), SLOT(rotateBrick()));

	connect(this, SIGNAL(lines(int)), SLOT(adjustMoveDownInterval(int)));
	connect(m_timer, SIGNAL(timeout()), SLOT(moveDown()));

	/* the first brick would fall automatically but the rotate
	 * would crash the game if pressed before the first timer timeout occured */
	m_timer->setInterval(START_INTERVAL);
}

/** Starts the game.
 * Runs the timer and drops a brick.
 * Avoids doing that in the constructor so that objects have a chance to connect
 *  the board signals to their slots before the game starts.
 */
void Board::startGame()
{
	this->prepareNextBrick();
	this->dropBrick();
}

/** Prepares the next brick.
 * Sends a signal with pointer to the next Brick
 */
void Board::prepareNextBrick()
{
	if (m_brickBag.isEmpty())
		this->fillBrickBag();

	m_brickNext = m_brickBag.takeAt(qrand() % m_brickBag.size());

	//qDebug() << "next brick prepared";
	emit nextBrick(m_brickNext);
}

/** Drops a new brick.
 * Assumes that all other bricks are stuck down.
 */
void Board::dropBrick()
{
	m_timer->start();

	m_brick = m_brickNext;
	m_brick->resetRotation();
	m_brickPos = QPoint((COLUMNS / 2) - (m_brick->width() / 2), 0 - m_brick->height());

	this->prepareNextBrick();
}

/** Moves the currently falling brick.
 * Assumes that there actually is a falling brick.
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


/** Checks if the brick move from the given position causes collision.
 * If it does, it classifies what type of collision occurred.
 */
BrickMoveResult Board::checkBrickMove(const QPoint position, const BrickMoveDirection move)
{
	/* horizontal board bounds */
	if (position.x() < 0 || position.x() > (COLUMNS - m_brick->width())) {
		/* if we hit horizontal bounds while moving down, then that's a bug! */
		Q_ASSERT(move != Down);
		return Noop;
	}

	/* vertical end of board */
	if ((position.y() + m_brick->height()) > ROWS)
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
		default: {}
	}

	BrickMoveResult res = this->checkBrickMove(m_brickPos + change, move);

	if (res == Ok)
		/* shift position */
		m_brickPos += change;
	else
		return res;

	return Ok;
}

/** Rotate the currently falling brick. */
void Board::rotateBrick()
{
	/* undraw the original brick */
	this->drawBrick(false);

	/* rotate the brick */
	m_brick->rotate();

	/* check for rotate collision */
	BrickMoveResult res = this->checkBrickMove(m_brickPos, Rotate);

	/* if the brick would hit the horizontal bounds, check if that could be solved
	 * by bouncing it off the edge */
	if (res == Noop && m_brickPos.x() > (COLUMNS - m_brick->width())) {
		QPoint adjustedPos = QPoint(COLUMNS - m_brick->width(), m_brickPos.y());

		res = this->checkBrickMove(adjustedPos, Rotate);
		/* if it worked, apply the adjusted position */
		if (res == Ok)
			m_brickPos = adjustedPos;
	}

	if (res != Ok)
		m_brick->unrotate();

	/* draw the rotated brick */
	this->drawBrick();
}

/** Draws (or undraws if draw=false) current brick on m_brickPos.
  */
void Board::drawBrick(bool draw)
{
	const QList<QPoint> &pointList = m_brick->pointList();
	QColor color;

	/* if we want to draw it, set a color, else leave the color invalid */
	if (draw)
		color = m_brick->brickColor();

	foreach (QPoint p, pointList) {
		int col = p.x() + m_brickPos.x();
		int row = p.y() + m_brickPos.y();

		QLayoutItem *item = m_layout->itemAtPosition(row, col);
		if (item) {
			Square *sq = qobject_cast<Square*>(item->widget());
			sq->resetSquareColor(color);
		}
	}
}

/** Removes the filled rows ("lines") keeping the board in the consistent state
 *  by reseting and reusing the removed squares.
 */
void Board::removeFilledRows() {
	//qDebug() << "removing filled rows";

	/* 'upper' and 'lower' have opposite meaning in graphical
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
		for (int col = 0; col < COLUMNS; col++) {
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

			for (int col = 0; col < COLUMNS; col++) {
				Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row, col)->widget());
				sq->resetSquareColor();
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
		for(int col = 0; col < COLUMNS; col++) {
			m_layout->addWidget(filledSquareList[sqIndex], row, col);
			sqIndex++;
		}
	}

	m_lines += filledRowList.size();
	emit lines(m_lines);

	Q_ASSERT(sqIndex == filledSquareList.size());
}

/** Speed up the game as the player progresses. */
void Board::adjustMoveDownInterval(int lines)
{
	int newInterval = START_INTERVAL - (lines / 10) * 30;

	if (newInterval <= 20) {
		this->gameOver(false);
	}

	m_timer->setInterval(newInterval);

}

/** Slots for brick movements. */
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

/** Stops the game.
  * Stop the main timer(s), show info to player, offer Retry
  */
void Board::gameOver(bool fail) {
	//disconnect(m_timer, SIGNAL(timeout()), this, SLOT(moveDown()));
	m_timer->disconnect(SIGNAL(timeout()));

	int ret;
	if (fail) {
		ret = QMessageBox::critical(this,
			tr("Game over"), tr("Game over - bricks collided!"),
			QMessageBox::Retry, QMessageBox::Close);
	} else {
		/* be funny as hell */
		ret = QMessageBox::information(this,
			tr("Game over"), tr("Hmm, you have won at tetris! Congratulations, Chuck Norris."),
			QMessageBox::Retry, QMessageBox::Close);
	}

	switch (ret) {
		case QMessageBox::Retry:
			/* TODO: support the board reset */
			emit gameReset();
			break;
		default:
			/* noop */
			break;
	}
}

/** Moves row (graphically) down by 'shift' rows.
 * It overwrites the destination rows' squares and keeps the original squares in place.
 */
void Board::moveRow(const int row, const int shift) {
	//qDebug() << "shifting row" << row << "by" << shift;
	for(int col = 0; col < COLUMNS; col++) {
		QWidget *w = m_layout->itemAtPosition(row, col)->widget();
		m_layout->addWidget(w, row+shift, col);
	}
}

/** Fills the brick bag with one of each shapes in random order.
 * The bricks are then randomly drawn from it.
 */
void Board::fillBrickBag()
{
	//qDebug() << "(re)filling the brick bag";

	/* provide seed to srand() */
	qsrand((uint)QTime::currentTime().msecsTo(QTime(0,0)));

	/* fill the brick bag with one of each brick-type in random order */
	while (m_brickBag.size() != m_brickList.size()) {
		Brick *brick = m_brickList.at(qrand() % m_brickList.size());
		if (!m_brickBag.contains(brick))
			m_brickBag << brick;
	}

	//m_brickBag << m_brickList.at(qrand() % m_brickList.size());
}

/** Fills the game brick list with all the shapes and their variants.
 */
void Board::initializeBrickList()
{
	QList<QList<QPoint> > pointLists;
	QList<QPoint> points;

	/* O */
	points << QPoint(0,0) << QPoint(0,1)
	       << QPoint(1,0) << QPoint(1,1);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, Qt::green, this);
	pointLists.clear();

	/* T */
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

	/* I */
	points << QPoint(0,0)
	       << QPoint(0,1)
	       << QPoint(0,2)
	       << QPoint(0,3);
	pointLists << points;
	points.clear();
	points << QPoint(0,0) << QPoint(1,0) << QPoint(2,0) << QPoint(3,0);
	pointLists << points;
	points.clear();
	m_brickList << new Brick(pointLists, QColor(0, 205, 255), this);
	pointLists.clear();

	/* J */
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

	/* L */
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

	/* Z */
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

	/* S */
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
}
