#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_QCustomPlotTestVS.h"
#include "Bucket.h"

namespace Ui {
	class QCustomPlotTestVS;
}

class QCustomPlotTestVS : public QMainWindow
{
    Q_OBJECT

public:
    explicit QCustomPlotTestVS(QWidget *parent = Q_NULLPTR);
	~QCustomPlotTestVS();
	
	void addPoint(double x, double y);
	void plot();
	//void plotData(int tick);

private:
    Ui::QCustomPlotTestVSClass *ui;

	QVector<double> qv_x, qv_y;
	int count = 0;

private slots:
	void on_btn_add_clicked();
	void horzScrollBarChanged(int value);
	void vertScrollBarChanged(int value);
	void xAxisChanged(QCPRange range);
	void yAxisChanged(QCPRange range);
};
