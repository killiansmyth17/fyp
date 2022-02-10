#include "MainWindow.h"
#include "QCustomPlotTestVS.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(clicked()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::clicked() {
	this->close();
	QCustomPlotTestVS* w = new QCustomPlotTestVS(this);
	w->show();
}