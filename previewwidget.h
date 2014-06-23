#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>

class QGridLayout;
class Brick;

class PreviewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PreviewWidget(QWidget *parent = 0);

private:
	QGridLayout *m_layout;

public:
	void drawBrick(Brick *brick);
};

#endif // PREVIEWWIDGET_H
