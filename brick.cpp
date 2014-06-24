#include "brick.h"

#include <QDebug>

Brick::Brick(QList<QList<QPoint> > pointLists, const QColor color, QObject *parent) :
	QObject(parent),
	m_pointLists(pointLists),
	m_brickColor(color),
	m_rotateIndex(0)
{
	/* calculate the brick height and width */
	foreach(QList<QPoint> pointList, pointLists) {
		int width = 0;
		int height = 0;

		foreach(QPoint p, pointList) {
			if (p.x() > width)
				width = p.x();
			if (p.y() > height)
				height = p.y();
		}

		m_widths << width+1;
		m_heights << height+1;

		Q_ASSERT(m_widths.last() > 0);
		Q_ASSERT(m_heights.last() > 0);
	}

	Q_ASSERT(m_pointLists.size() == m_widths.size());
	Q_ASSERT(m_pointLists.size() == m_heights.size());
}

void Brick::rotate()
{
	m_rotateIndex = (m_rotateIndex + 1) % m_pointLists.size();
}

void Brick::unrotate()
{
	if (--m_rotateIndex < 0)
		m_rotateIndex += m_pointLists.size();
}

void Brick::resetRotation()
{
	m_rotateIndex = 0;
}

int Brick::width() const
{
	return m_widths[m_rotateIndex];
}

int Brick::height() const
{
	return m_heights[m_rotateIndex];
}

const QList<QPoint>& Brick::pointList() const
{
	return m_pointLists[m_rotateIndex];
}

const QColor& Brick::brickColor() const
{
	return m_brickColor;
}
