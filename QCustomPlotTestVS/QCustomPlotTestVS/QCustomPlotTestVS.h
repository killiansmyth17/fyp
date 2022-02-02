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

	void addConsumptionPoint(double x, std::vector<double> vec);
	void addWindPoint(double x, std::vector<double> vec);
	void plot();
	void setupOrigin();
	//void plotData(int tick);

private:
    Ui::QCustomPlotTestVSClass *ui;

	QVector<double> wind_x, wind_y, consumption_x, consumption_y;
	int count = 0;

private slots:
	void plotPerSecond();
	//void on_btn_add_clicked();
	//void horzScrollBarChanged(int value);
	//void vertScrollBarChanged(int value);
	//void xAxisChanged(QCPRange range);
	//void yAxisChanged(QCPRange range);
};
