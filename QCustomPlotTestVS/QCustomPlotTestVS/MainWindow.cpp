#include "MainWindow.h"
#include "QCustomPlotTestVS.h"

int maxTick = 0;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(ui->openGraphButton, SIGNAL(clicked()), this, SLOT(showGraph()));

	QInputDialog inputDialog;
	bool ok;
	maxTick = inputDialog.getInt(0, "Input dialog", "Time to run:", 0, 0, 10000, 1, &ok);
	if (!ok) {
		abort();
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showGraph() {
	//this->close();
	QCustomPlotTestVS* w = new QCustomPlotTestVS(this);
	w->show();
}

void MainWindow::addWidget(QString name, QString type, double power, int index) {
	//QGridLayout* layout = qobject_cast<QGridLayout*>(ui->agents->layout());
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->agents->layout());

	//add agent to widget
	QHBoxLayout* agent = new QHBoxLayout(ui->agents);
	QLabel* nameLabel = new QLabel(name, ui->agents);
	agent->addWidget(nameLabel);
	QLabel* typeLabel = new QLabel(type, ui->agents);
	agent->addWidget(typeLabel);
	QString powerString = QString::number(power);
	QLabel* powerLabel = new QLabel(powerString, ui->agents);
	QString objectName = type + QString::number(index);
	powerLabel->setObjectName(objectName);
	agent->addWidget(powerLabel);

	layout->insertLayout(layout->count()-1, agent);
	//layout->addWidget(button, 0,0);
}

void MainWindow::changePower(QString type, int index, double power) {
	QString objectName = type + QString::number(index);
	QLabel* powerLabel = ui->agents->findChild<QLabel*>(objectName);
	QString powerString = QString::number(power);
	if(powerLabel) { //not null
		powerLabel->setText(powerString);
	}
}

void MainWindow::updateProgressBar() {
	ui->progressBar->setValue((int)(((double)(tick-1)/(double)maxTick)*100.0));
}