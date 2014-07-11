#include "square.h"

#include "board.h"
#include <QDebug>

Square::Square(QWidget *parent) :
	QPushButton(parent), m_occupied(false)
{
	m_defaultBackgroundColor = this->palette().color(this->backgroundRole());

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setDisabled(true);
	this->setDown(true);
}

void Square::resetSquareColor(QColor color)
{
	m_occupied = (color.isValid() && color != Board::SHADOW_COLOR);

	//qDebug() << (m_occupied ? "" : "un") << "drawing" << (color == Board::SHADOW_COLOR ? "shadow" : "square");

	QPalette p = this->palette();
	p.setColor(QPalette::Button, color.isValid() ? color : m_defaultBackgroundColor);
	this->setPalette(p);
}

bool Square::isOccupied() const
{
	return m_occupied;
}
