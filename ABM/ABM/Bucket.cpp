#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional> //for callbacks using class member
#include <math.h> //for pi

#include "sqlite3.h"
//#include "stdafx.h"
#include "Bucket.h"
#include "MainWindow.h"
#include "Graph.h"
#include <QObject>

int tick = 0; //increments 10 times every second, let 10 ticks represent a minute (1 tick = 6 seconds)
int waitTime = 20; //wait 20 milliseconds per agent action

//graphing values:
int windCount = 0;
int solarCount = 0;
int consumerCount = 0;
int smartBatteryCount = 0;

std::mutex totalMutex;
std::vector<double> totalWindPower;
std::vector<double> totalSolarPower;
std::vector<double> consumerPowerConsumption;
std::vector<double> batteryPowerConsumption;
std::vector<bool> smartBatteryCommunication;

std::mutex joulesMutex;
std::vector<double> totalWindEnergy;
std::vector<double> totalSolarEnergy;
std::vector<double> consumerEnergyConsumption;
std::vector<double> batteryEnergyConsumption;

std::mutex countMutex;

//create alias for 2D Vector
using Vector1D = std::vector<std::string>;
using Vector2D = std::vector<Vector1D>;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;
using Battery = std::unordered_map<std::string, double>;

////// GENERAL THREAD FUNCTIONS BEGIN //////
void suspendThread(int milliseconds) {
	std::chrono::milliseconds timespan(milliseconds);
	std::this_thread::sleep_for(timespan);
}

void Bucket::timer(MainWindow& w) {
	AgentUI agentUI;
	QObject::connect(&agentUI, &AgentUI::timeChanged, &w, &MainWindow::updateProgressBar);
	while (tick <= maxTick) { //simulate for [user input] ticks
		suspendThread(300); //one tick every 300ms
		tick++;

		agentUI.timeChanged();
	}
}





//callback function for sqlite3 query, puts data in 2D vector
static int handleData(void* data, int argc, char** argv, char** colName) {
	Vector2D* records = static_cast<Vector2D*>(data); //cast void* back to Vector2D*
	try {
		if (records->size() == 0) {
			records->emplace_back(colName, colName + argc); //add column headers to first index
		}
		records->emplace_back(argv, argv + argc);
	}
	catch (...) {
		return 1;
	}

	return 0;
}

//put entire [tableName] table into &data from database
int connectToDB(std::string tableName, Vector2D &data) {
	//Create database connection
	sqlite3* db; //database connection
	int rc; //return code
	char* zErrMsg = 0;

	rc = sqlite3_open("Agents.db", &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
	}

	std::string query("SELECT * FROM \"" + tableName + "\"");
	rc = sqlite3_exec(db, query.c_str(), handleData, &data, &zErrMsg); //execute query with callback function to handle data passed as pointer
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection
	return 0;
}

//get power data for agent, timetable passed by reference
int Bucket::getTimetable(std::string tableName, Timetable &datamap) {

	Vector2D data;
	connectToDB(tableName, data);

	//map time as index to data (power) as double -> power consumption at time x = y
	for (int i = 1; i < data.size(); i++) { //skip 0 index because no need for headers
		int index = std::stoi(data[i][0]);
		datamap[index] = std::stod(data[i][1]);
	}

	return 0;
}

//get capacity and charging rate data for battery
int Bucket::getBattery(std::string tableName, Battery& datamap) {
	
	Vector2D data;
	connectToDB(tableName, data);

	//map time as index to data (power) as double -> power consumption at time x = y
	std::vector<std::string> headers;
	for (int i = 0; i < data[0].size(); i++) {
		headers.push_back(data[0][i]);
	}
	for (int i = 0; i < data[1].size(); i++) {
		datamap[headers[i]] = stod(data[1][i]);
	}

	return 0;
}





//gen-purpose set vector size function (& init all vals to 0)
void Bucket::setVecSize(std::vector<double> &totalVector) {
	totalMutex.lock();
	if (totalVector.size() == 0) { //set vector size ONCE
		totalVector.resize(maxTick);
		for (int i = 0; i < maxTick; i++) {
			totalVector[i] = 0;
		}
	}
	totalMutex.unlock();
}

//Add joules generated by agent to total for that type for current tick
void Bucket::addEnergyToVector(double energy, std::vector<double> &totalVector, int tick) { //functionally identical to addPowerToVector but with different mutex
	if (tick < maxTick) { //guard against straggling agents
		joulesMutex.lock();
		totalVector[tick] += energy;
		joulesMutex.unlock();
	}
}

//Add power for one agent to total per tick for plotting purposes, in a function because of mutex lock
void Bucket::addPowerToVector(double power, std::vector<double> &totalVector, int tick) { //functionally identical to addEnergyToVector but with different mutex
	if (tick < maxTick) { //guard against straggling agents
		totalMutex.lock();
		totalVector[tick] += power;
		totalMutex.unlock();
	}
}





//power consumption data stored as watts
void Bucket::powerConsumption(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable powerTimetable;
	getTimetable(tableName, powerTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}
	
	setVecSize(consumerPowerConsumption);
	setVecSize(consumerEnergyConsumption);

	int lastTick = 0;
	while (tick<=maxTick) {
		double powerConsumption = powerTimetable[tick];
		agentUI.setPower("consumer", index, powerConsumption);

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(powerConsumption, consumerPowerConsumption, tick-1);
		}

		addEnergyToVector(powerConsumption*60, consumerEnergyConsumption, tick-1); //each action represents 1 minute
		suspendThread(waitTime);
	}
}

void Bucket::solarGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable solarTimetable;
	getTimetable(tableName, solarTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}

	setVecSize(totalSolarPower);
	setVecSize(totalSolarEnergy);

	double length = 1;
	double width = 1;
	double efficiency = 0.2;

	int lastTick = 0;
	while (tick<=maxTick) {
		double solarFlux = solarTimetable[tick];

		//P = efficiency * solar flux * area of solar panel
		double solarPower = efficiency * solarFlux * length * width;

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(solarPower, totalSolarPower, tick - 1);
		}

		agentUI.setPower("solar", index, solarPower);
		
		addEnergyToVector(solarPower * 60, totalSolarEnergy, tick-1); //each agent action represents 1 minute
		suspendThread(waitTime);
	}
}

//wind data as wind speed, converted using generator defined in this program
void Bucket::windGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable windTimetable;
	getTimetable(tableName, windTimetable); //datamap passed by reference

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}

	setVecSize(totalWindPower);
	setVecSize(totalWindEnergy);

	double efficiency = 0.25; //turbines are 20% to 40% efficient at converting wind into energy
	double airDensity = 1.225; //kg/m^3 according to the International Standard Atmosphere (ISA) values�15� C at sea level with dry air
	double bladeLength = 1.5; //metres
	double generatorArea = M_PI * bladeLength * bladeLength; //pi*r^2

	int lastTick = 0;
	while (tick<=maxTick) {
		double windSpeed = windTimetable[tick];

		//P = (efficieny * density of air * area of wind generator * windspeed^3)/2
		double windPower = (efficiency * airDensity * generatorArea * windSpeed * windSpeed * windSpeed)/2;

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(windPower, totalWindPower, tick-1);
		}

		agentUI.setPower("wind", index, windPower);

		addEnergyToVector(windPower * 60, totalWindEnergy, tick-1); //each agent action represents 1 minute
		suspendThread(waitTime);
	}
}

//regular batteries take power from the system with no regard to any external factors
void Bucket::regularBattery(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	//get battery data
	Battery battery;
	getBattery(tableName, battery);

	QObject::connect(&agentUI, &AgentUI::batteryChanged, &w, &MainWindow::updateBattery);

	double batteryEnergy = 0; //current charge

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}

	setVecSize(batteryPowerConsumption);
	setVecSize(batteryEnergyConsumption);

	int lastTick = 0;
	while (tick <= maxTick && batteryEnergy < battery["Capacity"]) { //stop drawing power when full
		double power = battery["Power"];
		double energy = power * 60; //each agent action represents 1 minute

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(power, batteryPowerConsumption, tick - 1);
		}

		batteryEnergy = std::min(batteryEnergy + energy, battery["Capacity"]); //charge battery

		agentUI.updateBattery(index, batteryEnergy, battery["Capacity"]);
		addEnergyToVector(energy, batteryEnergyConsumption, tick-1);
		suspendThread(waitTime);
	}
}

//smart batteries
void Bucket::smartBattery(std::string tableName, int consumptionIndex, int smartBatteryIndex, MainWindow& w, AgentUI& agentUI) {
	//get battery data
	Battery battery;
	getBattery(tableName, battery);

	QObject::connect(&agentUI, &AgentUI::batteryChanged, &w, &MainWindow::updateBattery);

	double batteryEnergy = 0; //current charge

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}

	setVecSize(batteryPowerConsumption);
	setVecSize(batteryEnergyConsumption);

	//initialise smart battery communication vessel
	totalMutex.lock();
	if(smartBatteryCommunication.size() == 0) {
		smartBatteryCommunication.resize(smartBatteryCount);
		for (int i = 0; i < smartBatteryCount; i++) {
			smartBatteryCommunication[i] = false;
		}
	}
	totalMutex.unlock();

	int lastTick = 0;
	while (tick <= maxTick && batteryEnergy < battery["Capacity"]) {
		double power = 0;

		//wait until previous battery is full
		if (smartBatteryIndex == 0) {
			power = battery["Power"];
		}
		else if (smartBatteryCommunication[smartBatteryIndex - 1]) {
			power = battery["Power"];
		}

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(power, batteryPowerConsumption, tick - 1);
		}

		double energy = power * 60; //each action represents 1 minute
		batteryEnergy = std::min(batteryEnergy + energy, battery["Capacity"]); //charge battery

		agentUI.updateBattery(consumptionIndex, batteryEnergy, battery["Capacity"]);
		addEnergyToVector(energy, batteryEnergyConsumption, tick-1);
		suspendThread(waitTime);
	}

	smartBatteryCommunication[smartBatteryIndex] = true;
}






void waitForUserInput() {
	while (maxTick==0) {
		suspendThread(100);
	}
}

//Main thread function that represents agents
void Bucket::megaThread(MainWindow &w, std::string tableName, std::string type) {

	countMutex.lock(); //need to lock code segment until thread is assigned index

	AgentUI agentUI;
	QObject::connect(&agentUI, &AgentUI::addAgentToUI, &w, &MainWindow::addWidget);
	QObject::connect(&agentUI, &AgentUI::addBatteryToUI, &w, &MainWindow::addBattery);

	//kick off agent process
	if (type == "wind") {
		int index = windCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		windGeneration(tableName, index, w, agentUI);
	}

	else if (type == "solar") {
		int index = solarCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		solarGeneration(tableName, index, w, agentUI);
	}

	else if (type == "consumer") {
		int index = consumerCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		powerConsumption(tableName, index, w, agentUI);
	}

	else if (type == "regular battery") {
		int index = consumerCount++;
		agentUI.newBattery(index);
		countMutex.unlock();
		regularBattery(tableName, index, w, agentUI);
	}

	else if (type == "smart battery") {
		int index = consumerCount++;
		int smartBatteryIndex = smartBatteryCount++;
		agentUI.newBattery(index);
		countMutex.unlock();
		smartBattery(tableName, index, smartBatteryIndex, w, agentUI);
	}

	else { //no case, unlock mutex and do no thread actions
		countMutex.unlock();
	}
}
////// GENERAL THREAD FUNCTIONS END //////

void AgentUI::newAgent(std::string name, std::string type, double power, int index) {
	emit addAgentToUI(QString::fromStdString(name), QString::fromStdString(type), power, index);
}

void AgentUI::newBattery(int index) {
	emit addBatteryToUI(index);
}

void AgentUI::setPower(std::string type, int index, double power) {
	emit powerChanged(QString::fromStdString(type), index, power);
}

void AgentUI::updateBattery(int index, double power, double capacity) {
	emit batteryChanged(index, power, capacity);
}