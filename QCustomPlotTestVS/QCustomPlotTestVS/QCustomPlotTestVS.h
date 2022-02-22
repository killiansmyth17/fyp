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

	void plot();
	//double sumVector(std::vector<double> vec);
	//void addConsumptionPoint(double x, std::vector<double> vec);
	//void addWindPoint(double x, std::vector<double> vec);
	//void addSolarPoint(double x, std::vector<double> vec);
	//void addGasPoint(double x, std::vector<double> wind, std::vector<double> solar, std::vector<double> consumption);
	//void setupOrigin();

private:
    Ui::QCustomPlotTestVSClass *ui;
	QVector<double>points_x, wind_y, solar_y, consumption_y, gas_y;
	void appendPoints(QVector<double>& y, std::vector<double>& data);
	void appendXAxis(QVector<double>& x, int size);

private slots:
	//void plotPerSecond();
	//void on_btn_add_clicked();
	//void horzScrollBarChanged(int value);
	//void vertScrollBarChanged(int value);
	//void xAxisChanged(QCPRange range);
	//void yAxisChanged(QCPRange range);
};
