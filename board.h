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
	Rotate = 3
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
	static const int START_INTERVAL;
	static const QColor SHADOW_COLOR;

	void startGame();

private:
	QGridLayout *m_layout;
	QTimer *m_timer;

	Brick *m_brick;
	Brick *m_brickNext;
	QList<Brick*> m_brickList;
	QPoint m_brickPos;
	QPoint m_brickShadowPos;
	QList<Brick*> m_brickBag;
	bool m_skipShadow;

	int m_lines;

	void initializeBrickList();

	void dropBrick();
	BrickMoveResult moveBrick(BrickMoveDirection move);
	BrickMoveResult checkBrickMove(QPoint position, const BrickMoveDirection move);
	BrickMoveResult checkAndMakeBrickMove(BrickMoveDirection move);
	void fillBrickBag();
	void prepareNextBrick();

	void removeFilledRows();
	void moveRow(const int row, const int shift);

	void drawBrickImpl(Brick *brick, const QPoint &brickPos, const bool draw, const bool shadow = false);
	void drawBrick(bool draw = true);
	void drawBrickShadow(bool draw = true);
	void setBrickShadow(const bool enabled);

	void gameOver(bool fail = true);

private slots:
	void moveLeft();
	void moveRight();
	void moveDown();
	void moveFall();
	void rotateBrick();

	void adjustMoveDownInterval(int lines);

signals:
	void gameReset();
	void lines(int);
	void nextBrick(Brick*);
};

#endif // BOARD_H
