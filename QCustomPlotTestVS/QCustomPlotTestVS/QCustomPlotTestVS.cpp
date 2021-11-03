#include "QCustomPlotTestVS.h"
#include "ui_QCustomPlotTestVS.h"

QCustomPlotTestVS::QCustomPlotTestVS(QWidget *parent)
    : QMainWindow(parent),
	ui(new Ui::QCustomPlotTestVSClass)
{
    ui->setupUi(this);
	

	ui->saber->addGraph();
	ui->saber->graph()->setScatterStyle(QCPScatterStyle::ssCircle);
	ui->saber->graph()->setLineStyle(QCPGraph::lsLine);

	ui->saber->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(on_btn_add_clicked()));

	// create connection between axes and scroll bars:
	connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vertScrollBarChanged(int)));
	connect(ui->saber->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
	connect(ui->saber->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisChanged(QCPRange)));
}

QCustomPlotTestVS::~QCustomPlotTestVS() {
	delete ui;
}

void QCustomPlotTestVS::addPoint(double x, double y) {
	qv_x.append(x);
	qv_y.append(y);
}

void QCustomPlotTestVS::plot() {
	ui->saber->graph()->setData(qv_x, qv_y);
	ui->saber->replot();
	ui->saber->update();
}

void QCustomPlotTestVS::on_btn_add_clicked() {
	addPoint(ui->x->value(), ui->y->value());
	plot();
}

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
		ui->saber->yAxis->setRange(-value / 100.0, ui->saber->yAxis->range().size(), Qt::AlignCenter);
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