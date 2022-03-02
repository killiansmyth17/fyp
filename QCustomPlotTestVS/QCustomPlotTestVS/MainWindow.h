#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

extern int maxTick;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	~MainWindow();

private:
	Ui::MainWindow* ui;

private slots:
	void showGraph();

public slots:
	void addWidget(QString name, QString type, double power, int index);
	void addBattery(int index);
	void changePower(QString type, int index, double power);
	void updateBattery(int index, double power, double capacity);
	void updateProgressBar();
};
