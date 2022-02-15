#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	void userHasInput();

signals:
	void endWait();

private slots:
	void clicked();

public slots:
	void addWidget(QString name, QString type, double power, int index);
	void changePower(QString type, int index, double power);
};
