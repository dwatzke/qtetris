#ifndef SQUARE_H
#define SQUARE_H

#include <QPushButton>
#include <QColor>

class Square : public QPushButton
{
	Q_OBJECT
public:
	explicit Square(QWidget *parent = 0);

	void resetSquareColor(QColor color = QColor());

	bool isOccupied() const;

private:
	QColor m_defaultBackgroundColor;
	bool m_occupied;

signals:

public slots:

};

#endif // SQUARE_H
