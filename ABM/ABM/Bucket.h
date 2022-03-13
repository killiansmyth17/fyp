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
extern int tick;
extern std::vector<double> totalWindPower;
extern std::vector<double> totalSolarPower;
extern std::vector<double> totalPowerConsumption;

extern std::vector<double> totalWindEnergy;
extern std::vector<double> totalSolarEnergy;
extern std::vector<double> totalEnergyConsumption;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;
using Battery = std::unordered_map<std::string, double>;

class AgentUI : public QObject {
	Q_OBJECT

public:
	AgentUI(QObject* parent = Q_NULLPTR) {};
	~AgentUI() {};
	void newAgent(std::string name, std::string type, double power, int index);
	void newBattery(int index);
	void setPower(std::string type, int index, double power);
	void updateBattery(int index, double power, double capacity);

signals:
	void addAgentToUI(QString name, QString type, double power, int index);
	void addBatteryToUI(int index);
	void powerChanged(QString type, int index, double power);
	void batteryChanged(int index, double power, double capacity);
	void timeChanged();
};


class Bucket {

	public:
		void megaThread(MainWindow& w, std::string tableName, std::string type);
		void timer(MainWindow &w);

	private:
		int getTimetable(std::string tableName, Timetable &datamap);
		int getBattery(std::string tableName, Battery& datamap);
		void setVecSize(std::vector<double>& totalVector);
		void addEnergyToVector(double energy, std::vector<double>& totalVector, int tick);
		void addPowerToVector(double power, std::vector<double>& totalVector, int tick);
		void powerConsumption(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void windGeneration(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void solarGeneration(std::string tableName, int index, MainWindow &w, AgentUI &agentUI);
		void regularBattery(std::string tableName, int index, MainWindow& w, AgentUI& agentUI);
		void smartBattery(std::string tableName, int consumptionIndex, int smartBatteryIndex, MainWindow& w, AgentUI& agentUI);
		void chargeBattery(double intervals);
};