#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QGridLayout;
class QTimer;
class BrickInfo;


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
	BrickInfo *m_brickInfo;
	QList<BrickInfo*> m_brickInfoList;
	QPoint m_brickPos;

	void initialize();
	void drawBrick(bool draw = true);
	BrickMoveResult checkAndMakeBrickMove(BrickMoveDirection move);
	Qt::GlobalColor randomColor();

	void dropBrick();
	void moveBrick(BrickMoveDirection move);

	void rotate(bool right);

private slots:
	void timerMoveDown();

	void moveLeft();
	void moveRight();
	void moveDown();
	void moveFall();

	void rotateLeft();
	void rotateRight();

signals:
	void gameReset();
};

#endif // BOARD_H
