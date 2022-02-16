#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

#include "MainWindow.h"
#include <QObject>


//variables
extern bool wait;
extern int tick;
extern double joules;
extern std::vector<double> latestWindPower;
extern std::vector<double> latestPowerConsumption;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;

class AgentUI : public QObject {
	Q_OBJECT

public:
	AgentUI(QObject* parent = Q_NULLPTR) {};
	~AgentUI() {};
	void newAgent(std::string name, std::string type, double power, int index);
	void setPower(std::string type, int index, double power);

signals:
	void addAgentToUI(QString name, QString type, double power, int index);
	void powerChanged(QString type, int index, double power);
};


class Bucket {

	public:

		//functions
		void megaThread(MainWindow &w, std::unordered_map<std::string, int> headers, std::vector<std::string> data);
		void timer(void);

	private:
		int getTime();
		int getTimetable(std::string tableName, Timetable &datamap);
		void addPowerToVector(double powerConsumption, std::vector<double>& totalVector);
		void powerConsumption(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void windGeneration(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void solarGeneration(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void chargeBattery(double intervals);
		int checkInterval(std::function<void(int)> callback, double amount, int lastAction);
};