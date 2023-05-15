#pragma once

#include "qcustomplot.h"
#include <QPoint>

class MySelectionRect : public QCPSelectionRect
{
	Q_OBJECT

public:
	MySelectionRect(QCustomPlot *parent);
	~MySelectionRect();

protected:
	virtual void startSelection(QMouseEvent *event);
	virtual void moveSelection(QMouseEvent *event);
	virtual void endSelection(QMouseEvent *event);

	virtual void draw(QCPPainter *painter) ;

	// void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
private:

	//����
	//ѡȡ��X������
	double m_maxXCoord;
	double m_minXCoord;

	//ѡȡ��Y������
	double m_maxYCoord;
	double m_minYCoord;
};
