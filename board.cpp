#include "board.h"
#include "brickinfo.h"
#include "square.h"

#include <QDebug>
#include <QGridLayout>
#include <QLayoutItem>
#include <QShortcut>
#include <QTimer>

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

	connect(new QShortcut(QKeySequence(Qt::Key_Left ), this), SIGNAL(activated()), this, SLOT(moveLeft()));
	connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveRight()));
	connect(new QShortcut(QKeySequence(Qt::Key_Down ), this), SIGNAL(activated()), this, SLOT(moveDown()));
	connect(new QShortcut(QKeySequence(Qt::Key_Space), this), SIGNAL(activated()), this, SLOT(moveFall()));

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

	QList<QPoint> pointList;
	/* test brick that looks like :: */
	pointList << QPoint(0, 0) << QPoint(0, 1)
		  << QPoint(1, 0) << QPoint(1, 1);

	if (m_brickInfo) {
		delete m_brickInfo;
		m_brickInfo = 0;
	}
	m_brickInfo = new BrickInfo(pointList, this->randomColor());
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

	BrickMoveType moveType = this->checkBrickMove(move);

	/* draw new brick state */
	this->drawBrick();

	/* brick hit bottom of board */
	if (moveType == Collision) {
		qDebug() << "a collision occured";
		m_brickFalling = false;
	}
}

/** Check if changing brick position using 'change' parameter results in
  * valid or invalid move.
  */
BrickMoveType Board::checkBrickMove(BrickMoveDirection move)
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

	if (test.x() < 0 || test.x() > (Board::COLUMNS - m_brickInfo->width()))
		return Noop;

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
		qDebug() << "occupy test for" << "row =" << row << ", col =" << col;
		if (sq->isOccupied()) {
			if (change == QPoint(-1, 0) || change == QPoint(1, 0))
				return Noop;
			else
				return Collision;
		}
	}

	/* shift position */
	m_brickPos += change;

	return Valid;
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

		//qDebug() << (draw ? "draw" : "undraw") << "row =" << row << ", col =" << col;
		QLayoutItem *item = m_layout->itemAtPosition(row, col);
		if (item) {
			Square *sq = qobject_cast<Square*>(item->widget());
			if (draw) {
				qDebug() << "occupying row =" << row << ", col =" << col;
				sq->setSquareColor(color);
			} else {
				qDebug() << "freeing row =" << row << ", col =" << col;
				sq->resetSquareColor();
			}
		}
	}
}

Qt::GlobalColor Board::randomColor()
{
	Qt::GlobalColor colors[] = { Qt::black, Qt::blue, Qt::red, Qt::green, Qt::yellow };
	return colors[qrand() % 5];
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
