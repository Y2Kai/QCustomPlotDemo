#include "MySelectionRect.h"

MySelectionRect::MySelectionRect(QCustomPlot *parent)
	: QCPSelectionRect(parent),
	m_minXCoord(0),
	m_maxXCoord(0),
	m_minYCoord(0),
	m_maxYCoord(0)
{
}

MySelectionRect::~MySelectionRect()
{
}

void MySelectionRect::startSelection(QMouseEvent *event)
{
	mActive = true;

	//��ȡy�᷶Χ
	QCPRange yRange = mParentPlot->yAxis->range();
	//��ȡ���ĵ��λ��
	 m_maxYCoord = yRange.upper;
	 m_minYCoord = yRange.lower;
	//������ת��Ϊ��������
	double maxYCoordPixel = mParentPlot->yAxis->coordToPixel(m_maxYCoord);
	double minYCoordPixel = mParentPlot->yAxis->coordToPixel(m_minYCoord);

	QPoint topLeftPoint(QPoint(event->pos().x(), maxYCoordPixel));
	QPoint bottomRight(QPoint(event->pos().x(), minYCoordPixel));

	//��סѡ�������
	m_minXCoord = mParentPlot->xAxis->pixelToCoord(event->pos().x());
	m_maxXCoord = m_minXCoord;
	//

	mRect = QRect(topLeftPoint, bottomRight);
	emit started(event);
}

void MySelectionRect::moveSelection(QMouseEvent *event)
{
	QCPRange yRange = mParentPlot->yAxis->range();
	double minYCoord = mParentPlot->yAxis->coordToPixel(yRange.lower);

	QPoint bottomRight(QPoint(event->pos().x(), minYCoord));
	mRect.setBottomRight(bottomRight);

	//��סѡ�������
	m_maxXCoord = mParentPlot->xAxis->pixelToCoord(event->pos().x());
	//

	{
		// ��ȡ��ͼ����ָ��
		QCPAxisRect *axisRect = mParentPlot->axisRect();

		// ��ȡѡȡ�����ұ�Ե���ͼ�������ұ�Ե�ľ���
		double leftMargin = mParentPlot->xAxis->coordToPixel(m_minXCoord) - axisRect->left();
		double rightMargin = axisRect->right() - mParentPlot->xAxis->coordToPixel(m_maxXCoord);

		// ���ѡȡ���ұ�Ե������ͼ�����ұ�Ե�����ҹ���
		if (rightMargin < 0) {
			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower - rightMargin, mParentPlot->xAxis->range().upper - rightMargin);
			// ע�⣬���ｫ mSecond.x() �����ڻ�ͼ������ұ�Ե�����Ա��������ͷ
			//mSecond.setX(axisRect->right());
			//mDragging = false; // �����϶��������Ա�֤����ͷ��¼���������
		}
		// ���ѡȡ�����Ե������ͼ�������Ե���������
		else if (leftMargin < 0) {
			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower + leftMargin, mParentPlot->xAxis->range().upper + leftMargin);
			// ͬ���أ����ｫ mFirst.x() �����ڻ�ͼ��������Ե��
			//mFirst.setX(axisRect->left());
			//mDragging = false;
		}
	}

	emit changed(mRect, event);
	layer()->replot();
}

void MySelectionRect::endSelection(QMouseEvent *event)
{
	QCPRange yRange = mParentPlot->yAxis->range();
	double minYCoord = mParentPlot->yAxis->coordToPixel(yRange.lower);

	QPoint bottomRight(QPoint(event->pos().x(), minYCoord));
	mRect.setBottomRight(bottomRight);

	//��סѡ�������
	m_maxXCoord = mParentPlot->xAxis->pixelToCoord(event->pos().x());
	//

	mActive = false;
	emit accepted(mRect, event);
}

void MySelectionRect::draw(QCPPainter *painter)
{
	QPoint topLeftPoint(QPoint(mParentPlot->xAxis->coordToPixel(m_minXCoord) , mParentPlot->yAxis->coordToPixel(m_maxYCoord)));
	QPoint bottomRight(QPoint(mParentPlot->xAxis->coordToPixel(m_maxXCoord), mParentPlot->yAxis->coordToPixel(m_minYCoord)));

	mRect = QRect(topLeftPoint, bottomRight);
	painter->setPen(mPen);
	painter->setBrush(mBrush);
	painter->drawRect(mRect);
}

//void MySelectionRect::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos)
//{
//	// ���ȵ��ø��������¼��������Ա�֤Ĭ����Ϊ����������
//		QCPSelectionRect::mouseMoveEvent(event,startPos);
//
//	if (mActive)
//	{
//		// ��ȡ��ͼ����ָ��
//		QCPAxisRect *axisRect = mParentPlot->axisRect();
//
//		// ��ȡѡȡ�����ұ�Ե���ͼ�������ұ�Ե�ľ���
//		double leftMargin = m_minXCoord - axisRect->left();
//		double rightMargin = axisRect->right() - m_maxXCoord;
//
//		// ���ѡȡ���ұ�Ե������ͼ�����ұ�Ե�����ҹ���
//		if (rightMargin < 0) {
//			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower + rightMargin, mParentPlot->xAxis->range().upper + rightMargin);
//			// ע�⣬���ｫ mSecond.x() �����ڻ�ͼ������ұ�Ե�����Ա��������ͷ
//			//mSecond.setX(axisRect->right());
//			//mDragging = false; // �����϶��������Ա�֤����ͷ��¼���������
//		}
//		// ���ѡȡ�����Ե������ͼ�������Ե���������
//		else if (leftMargin < 0) {
//			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower - leftMargin, mParentPlot->xAxis->range().upper - leftMargin);
//			// ͬ���أ����ｫ mFirst.x() �����ڻ�ͼ��������Ե��
//			//mFirst.setX(axisRect->left());
//			//mDragging = false;
//		}
//	}
//}

