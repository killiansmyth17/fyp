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

void MainWindow::addWidget(QString name, QString type, double power, int index) {
	//QGridLayout* layout = qobject_cast<QGridLayout*>(ui->agents->layout());
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->agents->layout());

	//add agent to widget
	QHBoxLayout* agent = new QHBoxLayout(ui->agents);
	QLabel* agentName = new QLabel(name, ui->agents);
	agent->addWidget(agentName);
	QLabel* agentType = new QLabel(type, ui->agents);
	agent->addWidget(agentType);
	QString powerString = QString::number(power);
	QLabel* agentPower = new QLabel(powerString, ui->agents);
	agent->addWidget(agentPower);



	layout->insertLayout(layout->count()-1, agent);
	//layout->addWidget(button, 0,0);
}