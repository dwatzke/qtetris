#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QGridLayout;
class QTimer;
class Brick;

enum BrickMoveDirection {
	Down = 0,
	Left = 1,
	Right = 2
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
	Brick *m_brick;
	QList<Brick*> m_brickList;
	QPoint m_brickPos;
	int m_lines;

	void drawBrick(bool draw = true);
	BrickMoveResult checkBrickMove(const QPoint position, const BrickMoveDirection move);
	BrickMoveResult checkAndMakeBrickMove(BrickMoveDirection move);

	void dropBrick();
	BrickMoveResult moveBrick(BrickMoveDirection move);
	void gameOver(QString msg = QString());
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
	void lines(int);
};

#endif // BOARD_H
