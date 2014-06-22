#include "board.h"
#include "brickinfo.h"
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
	m_brickFalling(false),
	m_brickInfo(0),
	m_brickPos()
{
	this->initialize();
}

void Board::initialize()
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

	/* fill brick list */
	QList<QPoint> pointList;
	/* :: */
	pointList << QPoint(0,0) << QPoint(0,1)
		  << QPoint(1,0) << QPoint(1,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::green, this);
	pointList.clear();
	/* .:. */
	pointList << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
				 << QPoint(1,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::red, this);
	pointList.clear();
	/* .... */
	pointList << QPoint(0,0) << QPoint(1,0) << QPoint(2,0) << QPoint(3,0);
	m_brickInfoList << new BrickInfo(pointList, Qt::cyan, this);
	pointList.clear();
	/* :.. */
	pointList << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
						<< QPoint(2,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::blue, this);
	pointList.clear();
	/* ..: */
	pointList << QPoint(0,0) << QPoint(1,0) << QPoint(2,0)
		  << QPoint(0,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::darkCyan, this);
	pointList.clear();
	/* ':. */
	pointList << QPoint(0,0) << QPoint(1,0)
				 << QPoint(1,1) << QPoint(2,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::magenta, this);
	pointList.clear();
	/* .:' */
	pointList                << QPoint(1,0) << QPoint(2,0)
		  << QPoint(0,1) << QPoint(1,1);
	m_brickInfoList << new BrickInfo(pointList, Qt::darkGreen, this);
	pointList.clear();

	connect(new QShortcut(QKeySequence(Qt::Key_Left ), this), SIGNAL(activated()), this, SLOT(moveLeft()));
	connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveRight()));
	connect(new QShortcut(QKeySequence(Qt::Key_Down ), this), SIGNAL(activated()), this, SLOT(moveDown()));
	connect(new QShortcut(QKeySequence(Qt::Key_Space), this), SIGNAL(activated()), this, SLOT(moveFall()));
	connect(new QShortcut(QKeySequence(Qt::Key_Up   ), this), SIGNAL(activated()), this, SLOT(rotate()));

	/* TODO: connect moveLeft, moveRight, forceMoveDown and fallDown */
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerMoveDown()));
	m_timer->start(400);
}

/** Carries out the automatic move down.
 * Either drops a new brick or it moves down the currently falling one.
 */
void Board::timerMoveDown()
{
	if (!m_brickFalling)
		this->dropBrick();
	else
		this->moveBrick(Down);
}

/** Drops a new brick.
 * Assumes that all other bricks are stuck down.
 */
void Board::dropBrick()
{
	Q_ASSERT(m_brickFalling == false);

	m_brickInfo = m_brickInfoList.at(qrand() % m_brickInfoList.size());
	m_brickPos = QPoint((Board::COLUMNS / 2) - (m_brickInfo->width() / 2), 0 - m_brickInfo->height());

	m_brickFalling = true;

	Q_ASSERT(m_brickFalling == true);
}

/** Moves the currently falling brick.
 * Assumes that there actually is a falling brick.
 * Sets m_brickFalling = false if the brick just got stuck
 */
void Board::moveBrick(BrickMoveDirection move)
{
	if (!m_brickFalling) return;

	/* undraw previous brick state if any */
	this->drawBrick(false);

	/* determine if move causes collision and proceed with it if not */
	BrickMoveResult moveType = this->checkAndMakeBrickMove(move);

	/* handle game over */
	if (moveType == GameOver)
		this->gameOver();

	/* draw new brick state */
	this->drawBrick();

	/* TODO: should this be placed before or after drawBrick() ? */
	if (moveType == Collision)
		this->removeFilledRows();

	/* if there was collision, send another brick */
	if (moveType == Collision) {
		qDebug() << "collision occured";
		m_brickFalling = false;
	}
}

void Board::removeFilledRows() {
	qDebug() << "removing filled rows";

	/** 'upper' and 'lower' have opposite meaning in graphical
	 * and gridlayout-indexing point of view - I'm talking about
	 * the graphical point of view
	 * (i.e. lower squares have higher row indexes)
	 */
	const int upperBound = m_brickPos.y();
	const int lowerBound = upperBound + m_brickInfo->height()-1;

	QList<int> filledRowList;
	QList<Square*> filledSquareList;
	/* get list of filled rows (and their squares) */
	for (int row = lowerBound; row >= upperBound; row--) {
		bool filled = true;
		for (int col = 0; col < Board::COLUMNS; col++) {
			Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row, col)->widget());
			if (!sq->isOccupied()) {
				filled = false;
				break;
			}
		}

		if (filled) {
			qDebug() << "!! row" << row << "is filled";

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

	/* move non-filled rows down over filled rows */
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

	Q_ASSERT(sqIndex == filledSquareList.size());
}

/** Moves row (graphically) down by 'shift' rows */
void Board::moveRow(const int row, const int shift) {
	qDebug() << "shifting row" << row << "by" << shift;
	for(int col = 0; col < Board::COLUMNS; col++) {
		QWidget *w = m_layout->itemAtPosition(row, col)->widget();
		m_layout->addWidget(w, row+shift, col);
	}
}

/** Stops the game.
  * Stop the main timer(s), show info to player, offer Retry
  */
void Board::gameOver() {
	disconnect(m_timer, SIGNAL(timeout()), this, SLOT(timerMoveDown()));

	int ret = QMessageBox::critical(this,
		tr("Game over"), tr("Game over - bricks collided!"),
		QMessageBox::Retry, QMessageBox::Close);

	switch(ret) {
		case QMessageBox::Retry:
			emit gameReset();
			break;
		default:
			/* noop */
			break;
	}
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
	case Fall:
		/* TODO */
		break;
	}

	QPoint test = m_brickPos + change;

	/* horizontal board bounds */
	if (test.x() < 0 || test.x() > (Board::COLUMNS - m_brickInfo->width()))
		return Noop;

	/* vertical end of board */
	if ((test.y() + m_brickInfo->height()) > Board::ROWS)
		return Collision;

	/* check if any of the squares that new position requires is occupied */
	const QList<QPoint> &pointList = m_brickInfo->pointList();
	foreach (QPoint p, pointList) {
		int col = p.x() + test.x();
		int row = p.y() + test.y();

		if (row < 0)
			continue;

		Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row, col)->widget());
		//qDebug() << "occupy test for" << "row =" << row << ", col =" << col;

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

	/* shift position */
	m_brickPos += change;

	return Ok;
}

/** Draws (or undraws if draw=false) current brick on m_brickPos.
  */
void Board::drawBrick(bool draw)
{
	const QList<QPoint> &pointList = m_brickInfo->pointList();
	const QColor& color = m_brickInfo->brickColor();

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
	const QList<QPoint>& oldList = m_brickInfo->pointList();
	const QColor& color = m_brickInfo->brickColor();

	QList<QPoint> pointList;

	foreach(const QPoint& p, oldList) {
		QPoint np = QPoint(p.y(), p.x());
		pointList << np;

		/* check for collision */
		if (oldList.contains(np))
			continue; /* skip collision with itself */
		QPoint tmp = np + m_brickPos;
		QLayoutItem *item = m_layout->itemAtPosition(tmp.y(), tmp.x());
		if (!item) {
			/* TODO: try moving the brick left or right */
			qDebug() << "prevented rotation out of bounds";
			return;
		}
		/* noop if collision would happen */
		if (item && qobject_cast<Square*>(item->widget())->isOccupied()) {
			qDebug() << "prevented rotate collision";
			return;
		}
	}

	/* undraw original brick */
	this->drawBrick(false);

	/* if ptr not in brick list, delete it */
	if (!m_brickInfoList.contains(m_brickInfo))
		delete m_brickInfo;
	m_brickInfo = new BrickInfo(pointList, color);

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
	this->moveBrick(Fall);
}
