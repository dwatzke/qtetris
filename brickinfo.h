#ifndef BRICKINFO_H
#define BRICKINFO_H

#include <QColor>
#include <QList>
#include <QPoint>
#include <QObject>

class BrickInfo : public QObject
{
public:
	BrickInfo(QList<QPoint> pointList, QColor color, QObject *parent = 0);

	int width() const;
	int height() const;

	const QList<QPoint> &pointList();
	const QColor& brickColor();

private:
	int m_width;
	int m_height;
	QColor m_brickColor;

	QList<QPoint> m_pointList;

public slots:
	void gameStep();
};

#endif // BRICKINFO_H
