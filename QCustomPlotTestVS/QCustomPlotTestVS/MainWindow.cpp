#include "MainWindow.h"
#include "QCustomPlotTestVS.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->openGraphButton, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(ui->addWidgetButton, SIGNAL(clicked()), this, SLOT(addWidget()));
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

void MainWindow::addWidget() {
	//QGridLayout* layout = qobject_cast<QGridLayout*>(ui->agents->layout());
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->agents->layout());

	QHBoxLayout* agent = new QHBoxLayout(ui->agents);
	QLabel* agentName = new QLabel("Name", ui->agents);
	agent->addWidget(agentName);
	QLabel* agentType = new QLabel("Type", ui->agents);
	agent->addWidget(agentType);
	QLabel* agentPower = new QLabel("Power", ui->agents);
	agent->addWidget(agentPower);

	QString buttonName = tr("Button #%1").arg(layout->count());
	QPushButton* button = new QPushButton(buttonName, ui->agents);
	layout->insertLayout(0, agent);
	//layout->addWidget(button, 0,0);
}