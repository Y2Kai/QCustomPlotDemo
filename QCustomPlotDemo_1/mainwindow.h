#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public Q_SLOTS:
	
	void horzScrollBarChanged(int value);
	void xAxisChanged(QCPRange range);

	void slotRangeChanged(const QCPRange &newRange);

	void sndfileWave();

	void slot_selectionChangeByUser();
private:
    Ui::MainWindow *ui;

	//demo 1
	void drawDemoSimple();

	//加上数据选择设置
	void drawDemoSimple_2();

	//双曲线
	void twoCurves();

	//柱状图
	void histogram();

	//缺失数据测试
	void  missDataTest();
};

#endif // MAINWINDOW_H
