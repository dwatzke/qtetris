#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QGridLayout;
class QTimer;
class Brick;

enum BrickMoveDirection {
	Down = 0,
	Left = 1,
	Right = 2,
	Fall = 3
};

enum BrickMoveResult {
	Ok = 0,
	Noop = 1,
	Collision = 2,
	GameOver = 3
};

class Board : public QWidget
{
	Q_OBJECT
public:
	explicit Board(QWidget *parent = 0);

	static const int ROWS;
	static const int COLUMNS;

private:
	QGridLayout *m_layout;
	QTimer *m_timer;
	bool m_brickFalling;
	Brick *m_brick;
	QList<Brick*> m_brickList;
	QPoint m_brickPos;

	void drawBrick(bool draw = true);
	BrickMoveResult checkBrickMove(const QPoint position, const BrickMoveDirection move);
	BrickMoveResult checkAndMakeBrickMove(BrickMoveDirection move);

	void dropBrick();
	void moveBrick(BrickMoveDirection move);
	void gameOver();
	void removeFilledRows();

	void moveRow(const int row, const int shift);

private slots:
	void timerMoveDown();

	void moveLeft();
	void moveRight();
	void moveDown();
	void moveFall();

	void rotate();

signals:
	void gameReset();
};

#endif // BOARD_H
