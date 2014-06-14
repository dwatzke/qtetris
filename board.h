#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QGridLayout;
class QTimer;
class BrickInfo;

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
	QPoint m_lastMove;

	void initialize();

	void dropBrick();
	void moveBrick();

signals:

private slots:
	void timerMoveDown();

public slots:

};

#endif // BOARD_H
