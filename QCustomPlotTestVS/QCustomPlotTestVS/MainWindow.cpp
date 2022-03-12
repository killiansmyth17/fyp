#include <iostream>
#include <fstream>
#include <Windows.h>
#include "MainWindow.h"
#include "QCustomPlotTestVS.h"
#include "Instructions.h"
#include "Bucket.h"

int maxTick = 0;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint); //fix main window size

	//connect event handlers for buttons
	connect(ui->openGraphButton, SIGNAL(clicked()), this, SLOT(showGraph()));
	connect(ui->instructionsButton, SIGNAL(clicked()), this, SLOT(showInstructions()));
	connect(ui->launchDBBrowserButton, SIGNAL(clicked()), this, SLOT(openDBBrowser()));
	connect(ui->exportDataButton, SIGNAL(clicked()), this, SLOT(exportData()));

	//startup dialog to input maxTicks
	QInputDialog inputDialog;
	bool ok;
	maxTick = inputDialog.getInt(0, "Input dialog", "Time to run:", 0, 0, 10000, 1, &ok);
	if (!ok) {
		exit(0);
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}





//open graph window on button press
void MainWindow::showGraph() {
	QCustomPlotTestVS* w = new QCustomPlotTestVS();
	w->show();
}

//open instructions window on button press
void MainWindow::showInstructions() {
	Instructions* w = new Instructions();
	w->show();
}

//open DB Browser on button press
void MainWindow::openDBBrowser() {
	//relative path 
	QString path = QCoreApplication::instance()->applicationDirPath() + "/../../QCustomPlotTestVS/DB Browser (SQLite).lnk";
	QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}

//create CSV file report on button press
void MainWindow::exportData() {
	std::ofstream outfile("Report.csv");


	//create & populate vectors for power generation and surplus
	std::vector<double> totalPowerGeneration;
	std::vector<double> powerSurplus;
	std::vector<double> totalEnergyGeneration;
	std::vector<double> energySurplus;

	for (int i = 0; i < maxTick; i++) {
		totalPowerGeneration.push_back(totalWindPower[i] + totalSolarPower[i]);
		powerSurplus.push_back(totalPowerGeneration[i] - totalPowerConsumption[i]);
		totalEnergyGeneration.push_back(totalWindEnergy[i] + totalSolarEnergy[i]);
		energySurplus.push_back(totalEnergyGeneration[i] - totalEnergyConsumption[i]);
	}


	//generate headers row for total power for each tick
	outfile << "Tick, Wind Power (W), Solar Power (W), Power Generation (W), Power Consumption (W), Power Surplus (W)" << std::endl;

	//populate power data
	for (int i = 0; i < maxTick; i++) {
		outfile << i + 1 << ", " << totalWindPower[i] << ", " << totalSolarPower[i] << ", " << totalPowerGeneration[i] << ", " << totalPowerConsumption[i] << ", " << powerSurplus[i] << std::endl;
	}

	//generate headers row for total energy generated/consumed in each tick
	outfile << std::endl << "Tick, Wind Energy (J), Solar Energy (J), Energy Generation (J), Energy Consumption (J), Energy Surplus (J)" << std::endl;

	//populate energy data
	for (int i = 0; i < maxTick; i++) {
		outfile << i + 1 << ", " << totalWindEnergy[i] << ", " << totalSolarEnergy[i] << ", " << totalEnergyGeneration[i] << ", " << totalEnergyConsumption[i] << ", " << energySurplus[i] << std::endl;
	}


	//write changes to file
	outfile.close();
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

	//add widget to window
	layout->insertLayout(layout->count()-1, agent);
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

	//add widget to window
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

//update simulation progress bar
void MainWindow::updateProgressBar() {
	int progress = (int)(((double)(tick-1) / (double)maxTick) * 100.0);
	ui->progressBar->setValue(progress);

	//enable graph and data export buttons when simulation completes
	if (progress == 100) {
		ui->openGraphButton->setEnabled(true);
		ui->exportDataButton->setEnabled(true);
	}
}