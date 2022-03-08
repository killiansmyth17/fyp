#include "QCustomPlotTestVS.h"
#include "ui_QCustomPlotTestVS.h"
#include "Bucket.h"

QCustomPlotTestVS::QCustomPlotTestVS(QWidget *parent)
    : QMainWindow(parent),
	ui(new Ui::QCustomPlotTestVSClass)
{
    ui->setupUi(this);

	ui->plot->xAxis->setLabel("Ticks (15 minute segments)");
	ui->plot->yAxis->setLabel("Power (watts)");
	ui->plot->legend->setVisible(true);

	appendXAxis(points_x, maxTick);

	//Wind power plot setup
	ui->plot->addGraph();
	ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(0)->setName("Wind power");
	QPen windPen;
	windPen.setWidth(1);
	windPen.setColor(QColor(0, 0, 255));
	ui->plot->graph(0)->setPen(windPen);

	appendPoints(wind_y, totalWindPower);

	//Solar power plot setup
	ui->plot->addGraph();
	ui->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->plot->graph(1)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(1)->setName("Solar power");
	QPen solarPen;
	solarPen.setWidth(1);
	solarPen.setColor(QColor(0, 255, 0));
	ui->plot->graph(1)->setPen(solarPen);

	appendPoints(solar_y, totalSolarPower);

	//Power consumption plot setup
	ui->plot->addGraph();
	ui->plot->graph(2)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->plot->graph(2)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(2)->setName("Power consumption");
	QPen consumptionPen;
	consumptionPen.setWidth(1);
	consumptionPen.setColor(QColor(0, 0, 0));
	ui->plot->graph(2)->setPen(consumptionPen);

	appendPoints(consumption_y, totalPowerConsumption);

	ui->plot->setInteractions(QCP::iSelectLegend); //show which plot you clicked on in the legend
	plot();
}

QCustomPlotTestVS::~QCustomPlotTestVS() {
	delete ui;
}

//general purpose function to add points to plot vector
void QCustomPlotTestVS::appendPoints(QVector<double> &y, std::vector<double> &data) {
	for (int i = 0; i < data.size(); i++) {
		y.append(data[i]);
	}
}

//create x axis data points
void QCustomPlotTestVS::appendXAxis(QVector<double> &x, int size) {
	for (int i = 0; i < size; i++) {
		x.append(i);
	}
}

void QCustomPlotTestVS::plot() {
	//plot all points for each graph
	ui->plot->graph(0)->setData(points_x, wind_y);
	ui->plot->graph(1)->setData(points_x, solar_y);
	ui->plot->graph(2)->setData(points_x, consumption_y);

	ui->plot->xAxis->setRange(0, maxTick); //auto-scroll X axis
	ui->plot->yAxis->rescale(); //scale Y axis automatically after adding each point
	ui->plot->replot();
	ui->plot->update();
}