#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_Graph.h"
#include "Bucket.h"

namespace Ui { class Graph; };

class Graph : public QMainWindow
{
    Q_OBJECT

public:
    explicit Graph(QWidget *parent = Q_NULLPTR);
	~Graph();

	void plot();

private:
    Ui::Graph *ui;
	QVector<double>points_x, wind_y, solar_y, generation_y, consumer_y, battery_y, consumption_y;
	void appendPoints(QVector<double>& y, std::vector<double> data);
	void appendTotalPoints(QVector<double>& y, std::vector<double> vector1, std::vector<double> vector2);
	void appendXAxis(QVector<double>& x, int size);

public slots:
	void toggleVisibility(QCPLegend* legend, QCPAbstractLegendItem* item);
};
