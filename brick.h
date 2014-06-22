#ifndef BRICK_H
#define BRICK_H

#include <QColor>
#include <QList>
#include <QPoint>
#include <QObject>

class Brick : public QObject
{
public:
	Brick(QList<QList<QPoint> > pointLists, const QColor color, QObject *parent = 0);

	int width() const;
	int height() const;

	const QList<QPoint> &pointList() const;
	const QColor& brickColor() const;

	void rotate();
	void unrotate();

private:
	QList<QList<QPoint> > m_pointLists;
	QList<int> m_widths;
	QList<int> m_heights;
	QColor m_brickColor;
	int m_rotateIndex;

public slots:
	void gameStep();
};

#endif // BRICK_H
