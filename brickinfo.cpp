#include "brickinfo.h"

BrickInfo::BrickInfo(QList<QPoint> pointList) :
	m_pointList(pointList)
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
