#include "brickinfo.h"

BrickInfo::BrickInfo(QList<QPoint> pointList, QColor color, QObject *parent) :
	m_brickColor(color), m_pointList(pointList),
	QObject(parent)
{
	/* calculate the brick height and width */
	m_width = 0;
	m_height = 0;
	foreach(QPoint p, pointList) {
		if (p.x() > m_width)
			m_width = p.x();
		if (p.y() > m_height)
			m_height = p.y();
	}
	/* count from 1 (not 0) */
	m_width += 1;
	m_height += 1;

	Q_ASSERT(m_width > 0);
	Q_ASSERT(m_height > 0);
}

int BrickInfo::width() const
{
	return m_width;
}

int BrickInfo::height() const
{
	return m_height;
}

const QList<QPoint>& BrickInfo::pointList()
{
	return m_pointList;
}

const QColor& BrickInfo::brickColor()
{
	return m_brickColor;
}
