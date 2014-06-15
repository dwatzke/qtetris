#include "board.h"
#include "brickinfo.h"

#include <QGridLayout>
#include <QPushButton>
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
			QPushButton * btn = new QPushButton;
			btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			btn->setDisabled(true);
			btn->setDown(true);
			m_layout->addWidget(btn, row, col);
		}
	}

	/* TODO: connect moveLeft, moveRight, forceMoveDown and fallDown */
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerMoveDown()));
	m_timer->start(400);
}

/** Carries out the automatic move down.
 * Either drops a new brick or it moves down the currently falling one.
 */
void Board::timerMoveDown()
{
	if (!m_brickFalling) {
		this->dropBrick();
	} else {
		this->moveBrick(Down);
	}
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
	m_brickInfo = new BrickInfo(pointList);
	m_brickPos = QPoint((Board::COLUMNS / 2) - (m_brickInfo->width() / 2), -1);

	m_brickFalling = true;

	Q_ASSERT(m_brickFalling == true);
}

/** Moves the currently falling brick.
 * Assumes that there actually is a falling brick.
 * Sets m_brickFalling = false if the brick just got stuck
 */
void Board::moveBrick(BrickMove move)
{
	Q_ASSERT(m_brickFalling == true);

	/* TODO: undraw previous brick state if any */
	if (m_brickPos.y() != -1) {
		this->styleBrick(m_brickInfo->pointList());
	}

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

	m_brickPos += change;

	/* TODO: draw new brick state */
	this->styleBrick(m_brickInfo->pointList(), "background: black");
}

void Board::styleBrick(const QList<QPoint> &pointList, QString styleSheet)
{
	foreach (QPoint p, pointList) {
		int col = p.x() + m_brickPos.x();
		int row = p.y() + m_brickPos.y();

		QWidget *btn = m_layout->itemAtPosition(row, col)->widget();
		btn->setStyleSheet(styleSheet);
	}
}
