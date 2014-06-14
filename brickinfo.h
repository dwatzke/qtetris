#ifndef BRICKINFO_H
#define BRICKINFO_H

#include <QList>
#include <QPoint>
#include <QObject>

class BrickInfo : public QObject
{
public:
	BrickInfo(QList<QPoint> pointList);

	int width() const;
	int height() const;

	const QList<QPoint> &pointList();

private:
	int m_width;
	int m_height;

	QList<QPoint> m_pointList;

public slots:
	void gameStep();
};

#endif // BRICKINFO_H
