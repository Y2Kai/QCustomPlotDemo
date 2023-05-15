#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MySelectionRect.h"
#include "sndfile.hh"

#include <QVector>
#include <QString>
#include <QDateTime>
#include <QOpenGLWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->centralWidget->setLayout(ui->verticalLayout);

	ui->horizontalScrollBar->setRange(0, 2500);

	connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

	//demo1
	//drawDemoSimple();
	//drawDemoSimple_2();

	//两条曲线
	//twoCurves();

	//直方图
	//histogram();

	//音频波形图
	sndfileWave();

	//缺失数据测试
	//missDataTest();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::horzScrollBarChanged(int value)
{
	if (qAbs(ui->customPlot->xAxis->range().center() - value / 10.0) > 0.01) // if user is dragging plot, we don't want to replot twice
	{
		ui->customPlot->xAxis->setRange(value / 10.0, ui->customPlot->xAxis->range().size(), Qt::AlignLeft);
		ui->customPlot->replot();
	}
}

void MainWindow::xAxisChanged(QCPRange range)
{
	ui->horizontalScrollBar->setValue(qRound(range.center()*10.0)); // adjust position of scroll bar slider
	ui->horizontalScrollBar->setPageStep(qRound(range.size()*10.0)); // adjust size of scroll bar slider
}

void MainWindow::slotRangeChanged(const QCPRange &newRange)
{
	// 获取 X 轴指针
	QCPAxis *xAxis = ui->customPlot->xAxis;

	// 获取当前选取框的 X 范围
	double selectLeft = ui->customPlot->xAxis->axisRect()->left();
	double selectRight = ui->customPlot->xAxis->axisRect()->right();

	// 如果选取框超出 X 轴范围
	if (selectRight > newRange.upper && selectLeft > newRange.lower) {
		// 向左滚动 X 轴
		double diff = selectRight - newRange.upper;
		xAxis->setRange(xAxis->range().lower + diff, xAxis->range().upper + diff);
	}
	else if (selectLeft < newRange.lower && selectRight < newRange.upper) {
		// 向右滚动 X 轴
		double diff = newRange.lower - selectLeft;
		xAxis->setRange(xAxis->range().lower - diff, xAxis->range().upper - diff);
	}
}

void MainWindow::sndfileWave()
{
	// Open the sound file
#if 0
	std::string filePath = "F:\\source\\Client\\MyDemo\\QCustomPlot\\QCustomPlotDemo_1\\res\\test.wav";
#else
	std::string filePath = "F:\\source\\Client\\MyDemo\\QCustomPlot\\QCustomPlotDemo_1\\res\\nie1.wav";
#endif
	int64_t startts= QDateTime::currentMSecsSinceEpoch();
	int64_t s = startts;
	
	SndfileHandle file(filePath);
	if (file.error()) {
		qCritical() << "Error opening file:" << file.strError();
		return ;
	}

	qDebug() << "open file use |" << QDateTime::currentMSecsSinceEpoch() - s << "ms,all|" << QDateTime::currentMSecsSinceEpoch() - startts << "ms";
	s = QDateTime::currentMSecsSinceEpoch();

	int numChannels = file.channels();
	int numSamples = file.frames();

	int resampleValue = 1;
	std::vector<double> buffer(numSamples * numChannels);

	file.read(buffer.data(), numSamples * numChannels);

	qDebug() << "read data use |" << QDateTime::currentMSecsSinceEpoch() - s << "ms,all|" << QDateTime::currentMSecsSinceEpoch() - startts << "ms";
	s = QDateTime::currentMSecsSinceEpoch();

	QSharedPointer<QCPGraphDataContainer> dataContainer(new QCPGraphDataContainer);
	dataContainer->reserve(numSamples);
	for (int64_t i = 0; i < numSamples / resampleValue; i+= resampleValue) {
		dataContainer->add(QCPGraphData(i, buffer[i * numChannels]));
	}
	qDebug() << "numSamples|" << numSamples;
	qDebug() << "loop samples use |" << QDateTime::currentMSecsSinceEpoch() - s << "ms,all|" << QDateTime::currentMSecsSinceEpoch() - startts << "ms";
	s = QDateTime::currentMSecsSinceEpoch();

	ui->customPlot->addGraph();
	ui->customPlot->setAntialiasedElements(QCP::aeNone);  // 禁用抗锯齿模式
	ui->customPlot->setNoAntialiasingOnDrag(true);  // 禁用自动重绘
	ui->customPlot->setOpenGl(true);

	//// 启用“缺失数据”功能
	ui->customPlot->graph(0)->setAdaptiveSampling(true);
	//ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
	//
	ui->customPlot->graph(0)->setData(dataContainer);


	qDebug() << "setData use |" << QDateTime::currentMSecsSinceEpoch() - s << "ms,all|" << QDateTime::currentMSecsSinceEpoch() - startts << "ms";
	s = QDateTime::currentMSecsSinceEpoch();
	// Set up the axes
	ui->customPlot->xAxis->setLabel("Sample Number");
	ui->customPlot->yAxis->setLabel("Amplitude");

	ui->customPlot->rescaleAxes();
	//ui->customPlot->yAxis->setRange(-32768, 32767);

	//设置选取框为自定义选取框
	MySelectionRect *mySelectionRect = new MySelectionRect(ui->customPlot);
	ui->customPlot->setSelectionRect(mySelectionRect);
	//
	ui->customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
	ui->customPlot->selectionRect()->setPen(QPen(Qt::black, 1, Qt::DashLine));//虚线
	ui->customPlot->selectionRect()->setBrush(QBrush(QColor(0, 0, 100, 50)));//半透明浅蓝
	ui->customPlot->graph(0)->setSelectable(QCP::SelectionType::stSingleData);


	//支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
	ui->customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);//
																								  
	 //只允许缩放X轴
	QCPAxisRect *axisRect = ui->customPlot->axisRect();
	axisRect->setRangeZoom(Qt::Horizontal);
	axisRect->setRangeDrag(Qt::Horizontal);
	//
	/*QCPSelectionDecorator selDecorator;
	selDecorator.setPen(Qt::NoPen);
	selDecorator.setBrush(QBrush(QColor(0, 0, 0, 0), Qt::NoBrush));
	ui->customPlot->selectionDecorator()->setFor(QCP::SelectionRect, selDecorator);*/

	connect(ui->customPlot, &QCustomPlot::selectionChangedByUser, this , &MainWindow::slot_selectionChangeByUser);

	ui->customPlot->replot();
	
	qDebug() << "other use |" << QDateTime::currentMSecsSinceEpoch() - s << "ms,all|" << QDateTime::currentMSecsSinceEpoch() - startts << "ms";
	s = QDateTime::currentMSecsSinceEpoch();
}

void MainWindow::slot_selectionChangeByUser()
{
	// 获取选择框对象
	QCPSelectionRect* selectionRect = ui->customPlot->selectionRect();
	// 判断选择框是否存在
	if (selectionRect) {
		// 获取选择框的开始点和结束点
		double x = selectionRect->rect().left();
		double y = selectionRect->rect().top();
		double x1 = selectionRect->rect().right();
		double y1 = selectionRect->rect().bottom();

		auto xCoord = ui->customPlot->xAxis->pixelToCoord(x);
		auto xCoordEnd = ui->customPlot->xAxis->pixelToCoord(x1);

		xCoord = floor(xCoord);
		xCoordEnd = floor(xCoordEnd);

		// 输出结果
		qDebug() << "Selection Rect Top Left Point: (" << xCoord << ", " << y << ")";
		qDebug() << "Selection Rect Bottom Right Point: (" << xCoordEnd << ", " << y1 << ")";

		ui->customPlot->graph(0)->data()->remove(xCoord, xCoordEnd,true);

	}
}

void MainWindow::drawDemoSimple()
{
	QVector<double> x(101), y(101);

	for (int i = 0; i < 101; ++i) {
		x[i] = i / 50.0 - 1; // x范围[-1,1]
		y[i] = x[i] * x[i];  // y=x*x
	}

	ui->customPlot->addGraph();

	ui->customPlot->graph(0)->setData(x, y);
	ui->customPlot->graph(0)->setName("fist demo");

	//X,Y轴标签
	ui->customPlot->xAxis->setLabel("X");
	ui->customPlot->yAxis->setLabel("Y");

	// 设置坐标轴的范围，看到所有数据
	ui->customPlot->xAxis->setRange(-1, 1);
	ui->customPlot->yAxis->setRange(0, 1);
	ui->customPlot->legend->setVisible(true);

	//重画图像
	ui->customPlot->replot();
}

void MainWindow::drawDemoSimple_2()
{
	QVector<double> x(101), y(101);

	for (int i = 0; i < 101; ++i) {
		x[i] = i / 50.0 - 1; // x范围[-1,1]
		y[i] = x[i] * x[i];  // y=x*x
	}

	ui->customPlot->addGraph();

	ui->customPlot->graph(0)->setData(x, y);
	ui->customPlot->graph(0)->setName("fist demo");

	//X,Y轴标签
	ui->customPlot->xAxis->setLabel("X");
	ui->customPlot->yAxis->setLabel("Y");

	// 设置坐标轴的范围，看到所有数据
	ui->customPlot->xAxis->setRange(-1, 1);
	ui->customPlot->yAxis->setRange(0, 1);
	ui->customPlot->legend->setVisible(true);

	//选择相关的设置
	//选择框模式：选取数据
	//设置选取框为自定义选取框
	MySelectionRect *mySelectionRect = new MySelectionRect(ui->customPlot);
	ui->customPlot->setSelectionRect(mySelectionRect);
	//

	ui->customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
	ui->customPlot->selectionRect()->setPen(QPen(Qt::black, 1, Qt::DashLine));//虚线
	ui->customPlot->selectionRect()->setBrush(QBrush(QColor(0, 0, 100, 50)));//半透明浅蓝
	ui->customPlot->graph(0)->setSelectable(QCP::SelectionType::stDataRange);

	
	//支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
	ui->customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);//
	QCPSelectionRect *selectionRect = ui->customPlot->selectionRect();
	//重画图像
	ui->customPlot->replot();
}

void MainWindow::twoCurves()
{
	QCustomPlot *customPlot = ui->customPlot;//创建customPlot
											 //每条曲线都会独占一个graph()
	customPlot->addGraph();
	customPlot->graph(0)->setPen(QPen(Qt::blue)); // 曲线的颜色
	customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 曲线与X轴包围区的颜色

	customPlot->addGraph();//添加graph等价于添加一条新曲线
	customPlot->graph(1)->setPen(QPen(Qt::red)); // 曲线的颜色
												 // 生成模拟数据点 (x-y0 第一条曲线, x-y1为第2条曲线):
	QVector<double> x(251), y0(251), y1(251);
	for (int i = 0; i < 251; ++i)
	{
		x[i] = i;
		y0[i] = qExp(-i / 150.0)*qCos(i / 10.0); // 第一条曲线：y0衰减cos
		y1[i] = qExp(-i / 150.0);              // 第二条曲线：y1衰减指数
	}
	// 边框右侧和上侧均显示刻度线，但不显示刻度值:
	customPlot->xAxis2->setVisible(true);
	customPlot->xAxis2->setTickLabels(false);
	customPlot->yAxis2->setVisible(true);
	customPlot->yAxis2->setTickLabels(false);
	// 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等
	connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
	// 把已存在的数据填充进graph的数据区
	customPlot->graph(0)->setData(x, y0);
	customPlot->graph(1)->setData(x, y1);
	//自动调整XY轴的范围，以便显示出graph(0)中所有的点（下面会单独讲到这个函数）
	customPlot->graph(0)->rescaleAxes();
	//自动调整XY轴的范围，以便显示出graph(1)中所有的点
	customPlot->graph(1)->rescaleAxes(true);

	// 选择相关的设置
		//选择框模式：选取数据
	//设置选取框为自定义选取框
	MySelectionRect *mySelectionRect = new MySelectionRect(ui->customPlot);
	ui->customPlot->setSelectionRect(mySelectionRect);
	//

	ui->customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
	ui->customPlot->selectionRect()->setPen(QPen(Qt::black, 1, Qt::DashLine));//虚线
	ui->customPlot->selectionRect()->setBrush(QBrush(QColor(0, 0, 100, 50)));//半透明浅蓝
	ui->customPlot->graph(0)->setSelectable(QCP::SelectionType::stDataRange);

	// 支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
	customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

	//只允许缩放X轴
	QCPAxisRect *axisRect = ui->customPlot->axisRect();
	axisRect->setRangeZoom(Qt::Horizontal);
	axisRect->setRangeDrag(Qt::Horizontal);

	// 立即刷新图像
	ui->customPlot->replot();
}

void MainWindow::histogram()
{
	//绘制柱状图
	QCPAxis *xAxis = ui->customPlot->xAxis;//x轴
	QCPAxis *yAxis = ui->customPlot->yAxis;//y轴
	QCPBars *bars = new QCPBars(xAxis, yAxis);  // 使用xAxis作为柱状图的x轴，yAxis作为y轴

	bars->setAntialiased(false); // 为了更好的边框效果，关闭抗齿锯
	bars->setName("Bars"); // 设置图例
	bars->setPen(QPen(QColor(0, 160, 140).lighter(130))); // 设置柱状图的边框颜色
	bars->setBrush(QColor(20, 68, 106));  // 设置柱状图的画刷颜色

	QVector<double> ticks;
	QVector<QString> labels;
	ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;//轴的范围
	labels << "A" << "B" << "C" << "D" << "E" << "F" << "G";//轴的刻度文字显示
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(ticks, labels);
	xAxis->setTicker(textTicker);        // 设置为文字轴
	xAxis->setTickLabelRotation(60);     // 轴刻度文字旋转60度
	xAxis->setSubTicks(false);           // 不显示子刻度
	xAxis->setTickLength(0, 4);          // 轴内外刻度的长度分别是0,4,也就是轴内的刻度线不显示
	xAxis->setRange(0, 8);               // 设置x轴范围
	xAxis->setLabel("x");
	xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

	yAxis->setRange(0, 12.1);          //设置y轴范围
	yAxis->setPadding(35);             // 轴的内边距
	yAxis->setLabel("y");
	yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
	QVector<double> fossilData;
	fossilData << 10 << 9 << 2 << 5 << 7 << 4 << 1;//y轴坐标值
	bars->setData(ticks, fossilData);
	//设置选取框为自定义选取框
	MySelectionRect *mySelectionRect = new MySelectionRect(ui->customPlot);
	ui->customPlot->setSelectionRect(mySelectionRect);
	//
	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	// 立即刷新图像
	
	ui->customPlot->replot();
}

void MainWindow::missDataTest()
{
	// 创建图形
	QCPGraph *graph = ui->customPlot->addGraph();

	// 设置线条样式和点的形状
	graph->setLineStyle(QCPGraph::lsLine);
	graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

	// 设置数据
	QVector<double> xData, yData;
	xData << 1 << 2 << 3 << 4 << 5;
	yData << 3 << 4 << NAN << 6 << NAN;

	// 创建数据容器
	QSharedPointer<QCPGraphDataContainer> dataContainer(new QCPGraphDataContainer);


	// 添加数据到容器中
	for (int i = 0; i < xData.size(); ++i) {
		dataContainer->add(QCPGraphData(xData.at(i), yData.at(i)));
	}

	// 设置图形的数据
	graph->setData(dataContainer);

	ui->customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iMultiSelect);//

	// 更新图形
	ui->customPlot->replot();
}
