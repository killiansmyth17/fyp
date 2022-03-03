#include "MainWindow.h"
#include "QCustomPlotTestVS.h"

int maxTick = 0;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
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

//add agent display information to UI
void MainWindow::addWidget(QString name, QString type, double power, int index) {
	QWidget* agents = ui->agentsScrollContents; //pointer to agents group box for brevity
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(agents->layout());

	//add agent to widget
	QHBoxLayout* agent = new QHBoxLayout(agents);
	QLabel* nameLabel = new QLabel(name, agents);
	agent->addWidget(nameLabel);
	QLabel* typeLabel = new QLabel(type, agents);
	agent->addWidget(typeLabel);
	QString powerString = QString::number(power);
	QLabel* powerLabel = new QLabel(powerString, agents);
	QString objectName = type + QString::number(index);
	powerLabel->setObjectName(objectName);
	agent->addWidget(powerLabel);

	layout->insertLayout(layout->count()-1, agent);
	//layout->addWidget(button, 0,0);
}

//add battery display information to UI
void MainWindow::addBattery(int index) {
	QWidget* batteries = ui->batteriesScrollContents; //pointer to batteries group box for brevity
	QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(batteries->layout());

	//add battery to widget
	QProgressBar* batteryChargeBar = new QProgressBar(batteries);
	QString objectName = "battery" + QString::number(index);
	batteryChargeBar->setObjectName(objectName);
	batteryChargeBar->setValue(0); //initialize to 0%

	layout->insertWidget(layout->count()-1, batteryChargeBar);
}

//update agent display information
void MainWindow::changePower(QString type, int index, double power) {
	QString objectName = type + QString::number(index);
	QLabel* powerLabel = ui->agents->findChild<QLabel*>(objectName);
	QString powerString = QString::number(power);
	if(powerLabel) { //not null
		powerLabel->setText(powerString);
	}
}

//update battery display information
void MainWindow::updateBattery(int index, double power, double capacity) {
	QString objectName = "battery" + QString::number(index);
	QProgressBar* batteryChargeBar = ui->batteries->findChild<QProgressBar*>(objectName);
	if (batteryChargeBar) { //not null
		batteryChargeBar->setValue((int)((power / capacity) * 100.0));
	}
}

void MainWindow::updateProgressBar() {
	ui->progressBar->setValue((int)(((double)(tick-1)/(double)maxTick)*100.0));
}