#include "previewwidget.h"
#include "brick.h"
#include "square.h"

#include <QDebug>
#include <QGridLayout>

PreviewWidget::PreviewWidget(QWidget *parent) :
	QWidget(parent), m_layout(new QGridLayout(this))
{
	m_layout->setMargin(0);
	m_layout->setSpacing(0);

	for (int row = 0; row < 2; row++) {
		for(int col = 0; col < 4; col++) {
			Square * sq = new Square(this);
			m_layout->addWidget(sq, row, col);
		}
	}

	this->setFixedSize(80, 40);
}

void PreviewWidget::drawBrick(Brick *brick)
{
	//qDebug() << "drawing preview of the next brick";

	for (int pos = 0; pos < 8; pos++) {
		Square * sq = qobject_cast<Square*>(m_layout->itemAt(pos)->widget());
		sq->resetSquareColor();
	}

	if (brick->height() > 2)
		brick->rotate();

	const QList<QPoint> &pointList = brick->pointList();
	const QColor& color = brick->brickColor();

	foreach (QPoint p, pointList) {
		/* the '!!' thing aligns the O brick preview */
		int col = p.x() + !!(brick->width() == 2);
		int row = p.y();

		Square *sq = qobject_cast<Square*>(m_layout->itemAtPosition(row,col)->widget());
		sq->resetSquareColor(color);
	}
}
