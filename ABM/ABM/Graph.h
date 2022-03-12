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
	QVector<double>points_x, wind_y, solar_y, generation_y, consumption_y;
	void appendPoints(QVector<double>& y, std::vector<double> data);
	void appendGenerationPoints(QVector<double>& y, std::vector<double> windData, std::vector<double> solarData);
	void appendXAxis(QVector<double>& x, int size);
};
