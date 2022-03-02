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
#include "QCustomPlotTestVS.h"
#include <QObject>

bool once = false;

int tick = 0; //increments 10 times every second, let 10 ticks represent a minute (1 tick = 6 seconds)
int waitTime = 20; //wait 20 milliseconds per agent action

double joules = 0; //joules in battery
double batteryCapacity = 50000;

//graphing values:
int windCount = 0;
int solarCount = 0;
int consumerCount = 0;
//int batteryCount = 0;

std::mutex totalMutex;
std::vector<double> totalWindPower;
std::vector<double> totalSolarPower;
std::vector<double> totalPowerConsumption;

std::mutex joulesMutex;
std::mutex countMutex;

//create alias for 2D Vector
using Vector1D = std::vector<std::string>;
using Vector2D = std::vector<Vector1D>;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;
using Battery = std::unordered_map<std::string, double>;

////// GENERATOR SPECS BEGIN //////
/*std::unordered_map<std::string, double> windTurbine{ //Enercon E-126 EP3 3.5MW Turbine
	{"ratedPower", 3.5}, //kiloWatts, tied to k => turbine specs divided by 1000, not the real turbine
	{"ratedWindSpeed", 14},
	{"cutOut", 30},
	{"cutIn", 2},
	{"k", 0.01785714} //constant, update every time you update ratedPower (ratedPower/(ratedWindSpeed*ratedWindSpeed))
};*/

std::unordered_map<std::string, double> windTurbine{
	{"bladeLength", 1} //metres
};



void Bucket::chargeBattery(double thisJoules) {
	joulesMutex.lock();
	joules = std::min(joules + thisJoules, batteryCapacity); //cap out battery
	joulesMutex.unlock();
}



////// GENERAL THREAD FUNCTIONS BEGIN //////
void suspendThread(int milliseconds) {
	std::chrono::milliseconds timespan(milliseconds);
	std::this_thread::sleep_for(timespan);
}

void Bucket::timer(MainWindow& w) {
	AgentUI agentUI;
	QObject::connect(&agentUI, &AgentUI::timeChanged, &w, &MainWindow::updateProgressBar);
	while (tick < maxTick+1) { //simulate for [user input] ticks (+1 to account for incrementing to 1 first, agents must wait for tick==1 before starting)
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

//get power data for agent, timetable passed by reference
int Bucket::getTimetable(std::string tableName, Timetable &datamap) {

	//Create database connection
	sqlite3* db; //database connection
	int rc; //return code 
	char* zErrMsg = 0;

	rc = sqlite3_open("Agents.db", &db);
	if (rc) {
		 std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		 sqlite3_close(db);
	}

	Vector2D data;
	std::string query ("SELECT * FROM \""+tableName+"\"");
	rc = sqlite3_exec(db, query.c_str(), handleData, &data, &zErrMsg); //execute query with callback function to handle data passed as pointer
	if (rc != SQLITE_OK) {
		 std::cerr << "SQL error: " << zErrMsg << std::endl;
		 sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection

	//map time as index to data (power) as double -> power consumption at time x = y
	for (int i = 1; i < data.size(); i++) { //skip 0 index because no need for headers
		int index = std::stoi(data[i][0]);
		datamap[index] = std::stod(data[i][1]);
	}

	return 0;
}

//get capacity and charging rate data for battery
int Bucket::getBattery(std::string tableName, Battery& datamap) {
	//Create database connection
	sqlite3* db; //database connection
	int rc; //return code 
	char* zErrMsg = 0;

	rc = sqlite3_open("Agents.db", &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
	}

	Vector2D data;
	std::string query("SELECT * FROM \"" + tableName + "\"");
	rc = sqlite3_exec(db, query.c_str(), handleData, &data, &zErrMsg); //execute query with callback function to handle data passed as pointer
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection

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

//Add power for one agent to total per tick for plotting purposes, in a function because of mutex lock
void Bucket::addPowerToVector(double power, std::vector<double> &totalVector, int index) {
	totalMutex.lock();
	totalVector[index] += power;
	totalMutex.unlock();
}

//power consumption data stored as watts
void Bucket::powerConsumption(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable powerTimetable;
	getTimetable(tableName, powerTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}
	
	setVecSize(totalPowerConsumption);

	int lastTick = 0;
	while (tick<maxTick) {
		double powerConsumption = powerTimetable[tick];
		agentUI.setPower("consumer", index, powerConsumption);

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(powerConsumption, totalPowerConsumption, tick-1);
		}

		chargeBattery(powerConsumption*-60); //each action represents 1 minute
		suspendThread(waitTime);
	}
}

void Bucket::solarGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable solarTimetable;
	getTimetable(tableName, solarTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	setVecSize(totalSolarPower);

	int lastTick = 0;
	while (true) {
		double solarGeneration = solarTimetable[tick];

		double solarPower = 0; //TODO
		agentUI.setPower("solar", index, solarPower);
	}
}

//wind data as wind speed, converted using generator defined in this program
void Bucket::windGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable windTimetable;
	getTimetable(tableName, windTimetable); //datamap passed by reference

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	setVecSize(totalWindPower);

	double efficiency = 1; //no clue
	double airDensity = 1.225; //kg/m^3 according to the International Standard Atmosphere (ISA) values—15° C at sea level with dry air
	double bladeLength = windTurbine["bladeLength"]; //metres
	double generatorArea = M_PI * bladeLength * bladeLength; //pi*r^2

	int lastTick = 0;
	while (tick<maxTick) {
		double windSpeed = windTimetable[tick];

		//P = (efficieny * density of air * area of wind generator * windspeed^3)/3
		double windPower = efficiency * airDensity * generatorArea * windSpeed * windSpeed * windSpeed;

		//calculate power generation from wind speed (generator has cut out and cut in ratings, as well as rated speed, all need to be taken into account)
		/*
		if (windSpeed > windTurbine["cutOut"] || windSpeed < windTurbine["cutIn"]) { //if turbine can't turn
			windPower = 0;
		} else if (windSpeed > windTurbine["ratedWindSpeed"]) { // power gen caps at ratedPower when ratedWindSpeed is reached
			windPower = windTurbine["ratedPower"];
		} else {
			windPower = windSpeed * windSpeed * windTurbine["k"]; // P = kv^2
		}*/

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(windPower, totalWindPower, tick-1);
		}

		agentUI.setPower("wind", index, windPower);

		chargeBattery(windPower * 60); //each agent action represents 1 minute
		suspendThread(waitTime);
	}
}

//regular batteries take power from the system with no regard to any external factors
void Bucket::regularBattery(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	//get battery data
	Battery battery;
	getBattery(tableName, battery);

	QObject::connect(&agentUI, &AgentUI::batteryChanged, &w, &MainWindow::updateBattery);

	double batteryPower = 0; //current charge

	while (tick < 1) { //wait for tick to increment once before commencing
		suspendThread(20);
	}

	setVecSize(totalPowerConsumption);

	int lastTick = 0;
	while (tick < maxTick) {
		double power = battery["Power"] * 60; //each action represents 1 minute

		if (tick > lastTick) { //once per agent per tick
			lastTick = tick;
			addPowerToVector(power, totalPowerConsumption, tick - 1);
		}

		batteryPower = std::min(batteryPower + power, battery["Capacity"]);
		batteryPower += power; //charge battery

		agentUI.updateBattery(index, batteryPower, battery["Capacity"]);
		chargeBattery(power * -1); 
		suspendThread(waitTime);
	}
}

//smart batteries
void Bucket::smartBattery(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	//get battery data
	Battery battery;
	getBattery(tableName, battery);
}



int data_stoi(std::unordered_map<std::string, int> headers, std::vector<std::string> data, std::string query) {
	return stoi(data[headers[query]]);
}

//get data from query using column name for the current row
std::string data_string(std::unordered_map<std::string, int> headers, std::vector<std::string> data, std::string query) {
	return data[headers[query]]; //headers stores index of each header, data obtained using this index
}

boolean strCompare(std::string str, std::string comp) {
	return str.compare(comp) == 0;
}

void waitForUserInput() {
	while (maxTick==0) {
		suspendThread(100);
	}
}

//Main thread function that represents agents
void Bucket::megaThread(MainWindow &w, std::unordered_map<std::string, int> headers, std::vector<std::string> data) {

	//get the agent type
	std::string tableName = data_string(headers, data, "Name");
	std::string type = data_string(headers, data, "Type");

	countMutex.lock(); //need to lock code segment until thread is assigned index

	AgentUI agentUI;
	QObject::connect(&agentUI, &AgentUI::addAgentToUI, &w, &MainWindow::addWidget);
	QObject::connect(&agentUI, &AgentUI::addBatteryToUI, &w, &MainWindow::addBattery);

	//kick off agent process
	if (strCompare(type, "wind")) {
		int index = windCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		windGeneration(tableName, index, w, agentUI);
	}

	else if (strCompare(type, "solar")) {
		int index = solarCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		solarGeneration(tableName, index, w, agentUI);
	}

	else if (strCompare(type, "consumer")) {
		int index = consumerCount++;
		agentUI.newAgent(tableName, type, 0, index);
		countMutex.unlock();
		powerConsumption(tableName, index, w, agentUI);
	}

	else if (strCompare(type, "regular battery") || strCompare(type, "smart battery")) {
		int index = consumerCount++;
		agentUI.newBattery(index);
		countMutex.unlock();

		if (strCompare(type, "regular battery")) {
			regularBattery(tableName, index, w, agentUI);
		}
		else {
			smartBattery(tableName, index, w, agentUI);
		}
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