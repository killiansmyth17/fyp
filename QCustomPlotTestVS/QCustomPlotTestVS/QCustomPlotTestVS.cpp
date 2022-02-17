#include "QCustomPlotTestVS.h"
#include "ui_QCustomPlotTestVS.h"
#include "Bucket.h"

QCustomPlotTestVS::QCustomPlotTestVS(QWidget *parent)
    : QMainWindow(parent),
	ui(new Ui::QCustomPlotTestVSClass)
{
    ui->setupUi(this);

	ui->plot->xAxis->setLabel("Ticks (minutes)");
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

	//Power consumption plot setup
	ui->plot->addGraph();
	ui->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->plot->graph(1)->setLineStyle(QCPGraph::lsLine);
	ui->plot->graph(1)->setName("Power consumption");
	QPen consumptionPen;
	consumptionPen.setWidth(1);
	consumptionPen.setColor(QColor(0, 0, 0));
	ui->plot->graph(1)->setPen(consumptionPen);

	appendPoints(consumption_y, totalPowerConsumption);

	ui->plot->setInteractions(QCP::iSelectLegend); //show which plot you clicked on in the legend
	plot();
	//ui->saber->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); //allows click drag and scroll wheel zoom

	//setupOrigin(); // add points at origin at startup

	/*
	// create connection between axes and scroll bars:
	connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
	connect(ui->saber->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
	connect(ui->saber->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
	*/

	/*//set up a 1 second timer, fires slot every second
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(plotPerSecond()));
	timer->start(1000);*/
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

/*void QCustomPlotTestVS::addConsumptionPoint(double x, std::vector<double> vec) {
	double y = sumVector(vec);

	consumption_x.append(x);
	consumption_y.append(y);
}

void QCustomPlotTestVS::addWindPoint(double x, std::vector<double> vec) {
	double y = sumVector(vec);

	wind_x.append(x);
	wind_y.append(y);
}

void QCustomPlotTestVS::addSolarPoint(double x, std::vector<double> vec) {
	double y = sumVector(vec);

	solar_x.append(x);
	solar_y.append(y);
}

void QCustomPlotTestVS::addGasPoint(double x, std::vector<double> wind, std::vector<double> solar, std::vector<double> consumption) {
	double totalGeneration = sumVector(wind);
	totalGeneration += sumVector(solar);

	double totalConsumption = sumVector(consumption);

	double y = (totalConsumption > totalGeneration) ? totalConsumption - totalGeneration : 0;

	gas_x.append(x);
	gas_y.append(y);
}*/

void QCustomPlotTestVS::plot() {
	ui->plot->graph(0)->setData(points_x, wind_y);
	ui->plot->graph(1)->setData(points_x, consumption_y);
	ui->plot->xAxis->setRange(0, maxTick); //auto-scroll X axis
	ui->plot->yAxis->rescale(); //scale Y axis automatically after adding each point
	ui->plot->replot();
	ui->plot->update();
}

/*void QCustomPlotTestVS::setupOrigin() {
	std::vector<double> zeroVec;
	zeroVec.push_back(0);
	addWindPoint(0, zeroVec);
	addConsumptionPoint(0, zeroVec);
	plot();
}

void QCustomPlotTestVS::plotPerSecond() {
	double thisTick = (double)tick;
	std::vector<double> thisPowerConsumption = latestPowerConsumption;
	std::vector<double> thisWindPower = latestWindPower;

	addWindPoint(thisTick, thisWindPower);
	addConsumptionPoint(thisTick, thisPowerConsumption);

	plot();
	ui->saber->xAxis->setRange(0, maxTick); //auto-scroll X axis
	ui->saber->yAxis->rescale(); //scale Y axis automatically after adding each point
	ui->saber->replot();
	ui->batteryDisplay->setNum(joules); //display energy in battery
}*/


/*
/// SCROLL BAR FUNCTIONS  BEGIN ///
void QCustomPlotTestVS::horzScrollBarChanged(int value)
{
	if (qAbs(ui->saber->xAxis->range().center() - value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
	{
		ui->saber->xAxis->setRange(value / 100.0, ui->saber->xAxis->range().size(), Qt::AlignCenter);
		ui->saber->replot();
	}
}

void QCustomPlotTestVS::vertScrollBarChanged(int value)
{
	if (qAbs(ui->saber->yAxis->range().center() + value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
	{
		ui->saber->yAxis->rescale();
		//ui->saber->yAxis->setRange(-value / 100.0, ui->saber->yAxis->range().size(), Qt::AlignCenter);
		ui->saber->replot();
	}
}

void QCustomPlotTestVS::xAxisChanged(QCPRange range)
{
	ui->horizontalScrollBar->setValue(qRound(range.center() * 100.0)); // adjust position of scroll bar slider
	ui->horizontalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}

void QCustomPlotTestVS::yAxisChanged(QCPRange range)
{
	ui->verticalScrollBar->setValue(qRound(-range.center() * 100.0)); // adjust position of scroll bar slider
	ui->verticalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}
/// SCROLL BAR FUNCTIONS END ///
*/