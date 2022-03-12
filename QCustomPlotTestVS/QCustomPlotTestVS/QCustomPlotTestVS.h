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

private:
    Ui::QCustomPlotTestVSClass *ui;
	QVector<double>points_x, wind_y, solar_y, generation_y, consumption_y;
	void appendPoints(QVector<double>& y, std::vector<double> data);
	void appendGenerationPoints(QVector<double>& y, std::vector<double> windData, std::vector<double> solarData);
	void appendXAxis(QVector<double>& x, int size);
};
