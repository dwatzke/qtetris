#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QGridLayout;
class QTimer;
class BrickInfo;


enum BrickMove {
	Down = 0,
	Left = 1,
	Right = 2,
	Fall = 3
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
	QPoint m_brickPos;

	void initialize();
	void drawBrick(const QString &styleSheet = QString());

	void dropBrick();
	void moveBrick(BrickMove move);

signals:

private slots:
	void timerMoveDown();

	void moveLeft();
	void moveRight();
	void moveDown();
	void moveFall();

public slots:

};

#endif // BOARD_H
