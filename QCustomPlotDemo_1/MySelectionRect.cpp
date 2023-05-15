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

	//获取y轴范围
	QCPRange yRange = mParentPlot->yAxis->range();
	//获取最大的点的位置
	 m_maxYCoord = yRange.upper;
	 m_minYCoord = yRange.lower;
	//将坐标转换为像素坐标
	double maxYCoordPixel = mParentPlot->yAxis->coordToPixel(m_maxYCoord);
	double minYCoordPixel = mParentPlot->yAxis->coordToPixel(m_minYCoord);

	QPoint topLeftPoint(QPoint(event->pos().x(), maxYCoordPixel));
	QPoint bottomRight(QPoint(event->pos().x(), minYCoordPixel));

	//记住选择的坐标
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

	//记住选择的坐标
	m_maxXCoord = mParentPlot->xAxis->pixelToCoord(event->pos().x());
	//

	{
		// 获取绘图区域指针
		QCPAxisRect *axisRect = mParentPlot->axisRect();

		// 获取选取框左右边缘与绘图区域左右边缘的距离
		double leftMargin = mParentPlot->xAxis->coordToPixel(m_minXCoord) - axisRect->left();
		double rightMargin = axisRect->right() - mParentPlot->xAxis->coordToPixel(m_maxXCoord);

		// 如果选取框右边缘超过绘图区域右边缘，向右滚动
		if (rightMargin < 0) {
			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower - rightMargin, mParentPlot->xAxis->range().upper - rightMargin);
			// 注意，这里将 mSecond.x() 限制在绘图区域的右边缘处，以避免滚动过头
			//mSecond.setX(axisRect->right());
			//mDragging = false; // 设置拖动结束，以保证鼠标释放事件正常进行
		}
		// 如果选取框左边缘超过绘图区域左边缘，向左滚动
		else if (leftMargin < 0) {
			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower + leftMargin, mParentPlot->xAxis->range().upper + leftMargin);
			// 同样地，这里将 mFirst.x() 限制在绘图区域的左边缘处
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

	//记住选择的坐标
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
//	// 首先调用父类的鼠标事件函数，以保证默认行为的正常进行
//		QCPSelectionRect::mouseMoveEvent(event,startPos);
//
//	if (mActive)
//	{
//		// 获取绘图区域指针
//		QCPAxisRect *axisRect = mParentPlot->axisRect();
//
//		// 获取选取框左右边缘与绘图区域左右边缘的距离
//		double leftMargin = m_minXCoord - axisRect->left();
//		double rightMargin = axisRect->right() - m_maxXCoord;
//
//		// 如果选取框右边缘超过绘图区域右边缘，向右滚动
//		if (rightMargin < 0) {
//			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower + rightMargin, mParentPlot->xAxis->range().upper + rightMargin);
//			// 注意，这里将 mSecond.x() 限制在绘图区域的右边缘处，以避免滚动过头
//			//mSecond.setX(axisRect->right());
//			//mDragging = false; // 设置拖动结束，以保证鼠标释放事件正常进行
//		}
//		// 如果选取框左边缘超过绘图区域左边缘，向左滚动
//		else if (leftMargin < 0) {
//			mParentPlot->xAxis->setRange(mParentPlot->xAxis->range().lower - leftMargin, mParentPlot->xAxis->range().upper - leftMargin);
//			// 同样地，这里将 mFirst.x() 限制在绘图区域的左边缘处
//			//mFirst.setX(axisRect->left());
//			//mDragging = false;
//		}
//	}
//}

