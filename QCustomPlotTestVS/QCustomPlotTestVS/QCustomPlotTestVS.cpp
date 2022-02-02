#include "QCustomPlotTestVS.h"
#include "ui_QCustomPlotTestVS.h"
#include "Bucket.h"

QCustomPlotTestVS::QCustomPlotTestVS(QWidget *parent)
    : QMainWindow(parent),
	ui(new Ui::QCustomPlotTestVSClass)
{
    ui->setupUi(this);
	

	ui->saber->addGraph();
	ui->saber->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->saber->graph(0)->setLineStyle(QCPGraph::lsLine);
	ui->saber->graph(0)->setName("Wind power");

	ui->saber->addGraph();
	ui->saber->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->saber->graph(1)->setLineStyle(QCPGraph::lsLine);
	ui->saber->graph(1)->setName("Power consumption");

	//ui->saber->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); //allows click drag and scroll wheel zoom

	setupOrigin(); // add points at origin at startup

	/*
	// create connection between axes and scroll bars:
	connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
	connect(ui->saber->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
	connect(ui->saber->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
	*/

	//set up a 1 second timer, fires slot every second
	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(plotPerSecond()));
	timer->start(1000);
}

QCustomPlotTestVS::~QCustomPlotTestVS() {
	delete ui;
}

void QCustomPlotTestVS::addConsumptionPoint(double x, std::vector<double> vec) {
	double y = 0;
	for (int i = 0; i < vec.size(); i++) {
		y += vec[i];
	}

	consumption_x.append(x);
	consumption_y.append(y);
}

void QCustomPlotTestVS::addWindPoint(double x, std::vector<double> vec) {
	double y = 0;
	for (int i = 0; i < vec.size(); i++) {
		y += vec[i];
	}

	wind_x.append(x);
	wind_y.append(y);
}

void QCustomPlotTestVS::plot() {
	ui->saber->graph(0)->setData(wind_x, wind_y);
	ui->saber->graph(1)->setData(consumption_x, consumption_y);
	ui->saber->replot();
	ui->saber->update();
}

void QCustomPlotTestVS::setupOrigin() {
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
	ui->saber->yAxis->rescale(); //scale both axes automatically after adding each point
	ui->saber->xAxis->rescale();
	ui->saber->replot();
	ui->batteryDisplay->setNum(joules); //display energy in battery
}


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