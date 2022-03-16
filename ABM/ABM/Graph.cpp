#include "Graph.h"
#include "ui_Graph.h"
#include "Bucket.h"

Graph::Graph(QWidget *parent)
    : QMainWindow(parent),
	ui(new Ui::Graph)
{
    ui->setupUi(this);

	ui->plot->xAxis->setLabel("Ticks (15 minute segments)");
	ui->plot->yAxis->setLabel("Power (watts)");
	ui->plot->legend->setVisible(true);
	ui->plot->legend->setSelectableParts(QCPLegend::spItems); //limit selection to legend items and not legend box

	appendXAxis(points_x, maxTick);

	//CREATE GENERATOR PLOTS
	QCPScatterStyle generatorStyle = QCPScatterStyle::ssTriangle;
	generatorStyle.setSize(10);

	//Wind power plot setup
	ui->plot->addGraph();
	ui->plot->graph(0)->setScatterStyle(generatorStyle);
	ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(0)->setName("Wind power");
	QPen windPen;
	windPen.setWidth(1);
	windPen.setColor(QColor(0, 0, 255));
	ui->plot->graph(0)->setPen(windPen);

	appendPoints(wind_y, totalWindPower);

	//Solar power plot setup
	ui->plot->addGraph();
	ui->plot->graph(1)->setScatterStyle(generatorStyle);
	ui->plot->graph(1)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(1)->setName("Solar power");
	QPen solarPen;
	solarPen.setWidth(1);
	solarPen.setColor(QColor(0, 255, 0));
	ui->plot->graph(1)->setPen(solarPen);

	appendPoints(solar_y, totalSolarPower);

	//Power generation plot setup
	ui->plot->addGraph();
	ui->plot->graph(2)->setScatterStyle(generatorStyle);
	ui->plot->graph(2)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(2)->setName("Total power generation");
	QPen generationPen;
	generationPen.setWidth(1);
	generationPen.setColor(QColor(127, 127, 127));
	ui->plot->graph(2)->setPen(generationPen);
	appendTotalPoints(generation_y, totalWindPower, totalSolarPower);

	
	
	//CREATE CONSUMER PLOTS
	QCPScatterStyle consumerStyle = QCPScatterStyle::ssTriangleInverted;
	consumerStyle.setSize(10);

	//Consumer power consumption plot setup
	ui->plot->addGraph();
	ui->plot->graph(3)->setScatterStyle(consumerStyle);
	ui->plot->graph(3)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(3)->setName("Consumer power consumption");
	QPen consumerPen;
	consumerPen.setWidth(1);
	consumerPen.setColor(QColor(255, 0, 0));
	ui->plot->graph(3)->setPen(consumerPen);

	appendPoints(consumer_y, consumerPowerConsumption);

	//Battery power consumption plot setup
	ui->plot->addGraph();
	ui->plot->graph(4)->setScatterStyle(consumerStyle);
	ui->plot->graph(4)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(4)->setName("Battery power consumption");
	QPen batteryPen;
	batteryPen.setWidth(1);
	batteryPen.setColor(QColor(255, 127, 0));
	ui->plot->graph(4)->setPen(batteryPen);

	appendPoints(battery_y, batteryPowerConsumption);

	//Battery power consumption plot setup
	ui->plot->addGraph();
	ui->plot->graph(5)->setScatterStyle(consumerStyle);
	ui->plot->graph(5)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(5)->setName("Total power consumption");
	QPen consumptionPen;
	consumptionPen.setWidth(1);
	consumptionPen.setColor(QColor(0, 0, 0));
	ui->plot->graph(5)->setPen(consumptionPen);

	appendTotalPoints(consumption_y, consumerPowerConsumption, batteryPowerConsumption);

	ui->plot->setInteractions(QCP::iSelectLegend); //allow selecting plots in legend
	connect(
		ui->plot,
		SIGNAL(legendClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)),
		this,
		SLOT(toggleVisibility(QCPLegend*, QCPAbstractLegendItem*))
	); //event to toggle visibility when plot in legend double clicked

	plot(); //plot the points in graph

	QTextStream out(stdout);
	QString x = "Here";
	out << x;
}

Graph::~Graph() {
	delete ui;
}

//general purpose function to add points to plot vector
void Graph::appendPoints(QVector<double> &y, std::vector<double> data) {
	for (int i = 0; i < data.size(); i++) {
		y.append(data[i]);
	}
}

//function to total values for total vectors (generation and consumption)
void Graph::appendTotalPoints(QVector<double>& y, std::vector<double> vector1, std::vector<double> vector2) {
	for (int i = 0; i < vector1.size(); i++) {
		y.append(vector1[i] + vector2[i]);
	}
}

//create x axis data points
void Graph::appendXAxis(QVector<double> &x, int size) {
	for (int i = 0; i < size; i++) {
		x.append(i);
	}
}

void Graph::plot() {
	//plot all points for each graph
	ui->plot->graph(0)->setData(points_x, wind_y);
	ui->plot->graph(1)->setData(points_x, solar_y);
	ui->plot->graph(2)->setData(points_x, generation_y);
	ui->plot->graph(3)->setData(points_x, consumer_y);
	ui->plot->graph(4)->setData(points_x, battery_y);
	ui->plot->graph(5)->setData(points_x, consumption_y);

	ui->plot->xAxis->setRange(0, maxTick); //auto-scroll X axis
	ui->plot->yAxis->rescale(); //scale Y axis automatically after adding each point
	ui->plot->replot();
	ui->plot->update();
}

void Graph::toggleVisibility(QCPLegend* legend, QCPAbstractLegendItem* item) {
	Q_UNUSED(legend); //avoid unused parameter warnings

	if (item) { //only react if item was clicked (could be null if whitespace of legend was clicked)
		QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);


		if (plItem->plottable()->visible()) {
			plItem->plottable()->setVisible(false);
		}
		else {
			plItem->plottable()->setVisible(true);
		}

		ui->plot->update();
	}
}