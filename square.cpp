#include "square.h"

Square::Square(QWidget *parent) :
	QPushButton(parent), m_occupied(false)
{
	m_defaultBackgroundColor = this->palette().color(this->backgroundRole());

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setDisabled(true);
	this->setDown(true);
}

void Square::setSquareColor(QColor color)
{
	m_occupied = true;

	QPalette p = this->palette();
	p.setColor(this->backgroundRole(), color);
	this->setPalette(p);
}

void Square::freeSquare()
{
	this->setSquareColor(m_defaultBackgroundColor);
	m_occupied = false;
}

bool Square::isOccupied() const
{
	return m_occupied;
}
