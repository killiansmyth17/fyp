#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional> //for callbacks using class member

#include "sqlite3.h"
//#include "stdafx.h"
#include "Bucket.h"
#include "MainWindow.h"
#include "QCustomPlotTestVS.h"
#include <QObject>

bool wait = true;

int tick = 0; //increments 10 times every second, let 10 ticks represent a minute (1 tick = 6 seconds)
int ticksPerAction = 10; //one action every 10 ticks, multiply watts by 60 to get joules

double joules = 0; //joules in battery (kJ for now)
double batteryCapacity = 50000;

//graphing values:
int windCount = 0;
int solarCount = 0;
int consumerCount = 0;
std::vector<double> latestWindPower;
std::vector<double> latestSolarPower;
std::vector<double> latestPowerConsumption;

std::mutex joulesMutex;
std::mutex countMutex;

//create alias for 2D Vector
using Vector1D = std::vector<std::string>;
using Vector2D = std::vector<Vector1D>;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;

////// GENERATOR SPECS BEGIN //////
std::unordered_map<std::string, double> windTurbine{ //Enercon E-126 EP3 3.5MW Turbine
	{"ratedPower", 3.5}, //kiloWatts, tied to k => turbine specs divided by 1000, not the real turbine
	{"ratedWindSpeed", 14},
	{"cutOut", 30},
	{"cutIn", 2},
	{"k", 0.01785714} //constant, update every time you update ratedPower (ratedPower/(ratedWindSpeed*ratedWindSpeed))
};

//std::unordered_map<std::string, double> solarPanel{

//};
////// GENERATOR SPECS END //////



////// MUTEX FUNCTIONS BEGIN //////
void Bucket::changeJoules(double joulesTick) {
	joulesMutex.lock();

	joules = std::min(joules+joulesTick, batteryCapacity);
	std::cout << joules << " joules of energy in the battery\n" << std::endl;

	joulesMutex.unlock();
}
////// MUTEX FUNCTIONS END //////



////// AGENT ACTION FUNCTIONS BEGIN //////
void Bucket::chargeBattery(int intervals) {
	changeJoules(intervals * 1);
}

void Bucket::drainBattery(int intervals) {
	changeJoules(intervals * -1);
}
////// AGENT ACTION FUNCTIONS END //////



////// GENERAL THREAD FUNCTIONS BEGIN //////
void Bucket::suspendThread(int milliseconds) {
	std::chrono::milliseconds timespan(milliseconds);
	std::this_thread::sleep_for(timespan);
}

void Bucket::timer(void) {
	while (true) {
		suspendThread(100);
		tick++;
	}
}

//hacky way to convert ticks to 24 hour clock as int for database querying
int Bucket::getTime() {
	int time = tick / 10; //first divide by 10 to get minutes (1 tick = 0.1 min)
	time = time % 1440; //24 hours * 60 = 1440 minutes -> 24 hour wrap
	int minutes = time % 60; //then mod 60 to find minutes and subtract
	time /= 60; //then div 60 to find hours
	time *= 100; //then multiply hours by 100 to open up two rightmost digits for minutes
	minutes -= (minutes % 15); //round minutes to nearest 15 minute floor increment
	time += minutes; //then add minutes
	return time;
}

//Check if enough time has passed for action to happen
int Bucket::checkInterval(std::function<void(int)> callback, double amount, int lastAction) {
	int tickdiff = tick - lastAction;

	if (tickdiff > ticksPerAction) {
		double seconds = ticksPerAction * 6; //1 tick represents 6 seconds

		int intervals = tickdiff / ticksPerAction;
		callback(intervals*amount*seconds);
		return lastAction += ticksPerAction;
	}

	return lastAction;
}

static int handleData(void* data, int argc, char** argv, char** colName) {
	Vector2D* records = static_cast<Vector2D*>(data);
	try {
		records->emplace_back(argv, argv + argc);
	}
	catch (...) {
		return 1;
	}

	return 0;
}

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
	rc = sqlite3_exec(db, query.c_str(), handleData, &data, &zErrMsg);
	if (rc != SQLITE_OK) {
		 std::cerr << "SQL error: " << zErrMsg << std::endl;
		 sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection

	for (int i = 0; i < data.size(); i++) {
		int index = std::stoi(data[i][0]);
		datamap[index] = std::stod(data[i][1]);
	}

	return 0;
}

//power consumption data stored as watts
void Bucket::powerConsumption(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable powerTimetable;
	getTimetable(tableName, powerTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	int lastAction = 0;

	while (true) {
		double powerConsumption = powerTimetable[getTime()]/1000; //in kiloWatts
		agentUI.setPower("consumer", index, powerConsumption);

		latestPowerConsumption[index] = powerConsumption;

		lastAction = checkInterval(std::bind(&Bucket::chargeBattery, this, std::placeholders::_1), powerConsumption*-60, lastAction);
		suspendThread(100); //sleep for performance
	}
}

void Bucket::solarGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable solarTimetable;
	getTimetable(tableName, solarTimetable);

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	int lastAction = 0;

	while (true) {
		double solarGeneration = solarTimetable[getTime()];

		double solarPower = 0; //TODO
		agentUI.setPower("solar", index, solarPower);
	}
}

//wind data as wind speed, converted using generator defined in this program
void Bucket::windGeneration(std::string tableName, int index, MainWindow& w, AgentUI& agentUI) {
	Timetable windTimetable;
	getTimetable(tableName, windTimetable); //datamap passed by reference

	QObject::connect(&agentUI, &AgentUI::powerChanged, &w, &MainWindow::changePower);

	int lastAction = 0;
	
	while (true) {

		double windSpeed = windTimetable[getTime()];

		//calculate power generation from wind speed (generator has cut out and cut in ratings, as well as rated speed, all need to be taken into account)
		double windPower;
		if (windSpeed > windTurbine["cutOut"] || windSpeed < windTurbine["cutIn"]) { //if turbine can't turn
			windPower = 0;
		} else if (windSpeed > windTurbine["ratedWindSpeed"]) { // power gen caps at ratedPower when ratedWindSpeed is reached
			windPower = windTurbine["ratedPower"];
		} else {
			windPower = windSpeed * windSpeed * windTurbine["k"]; // P = kv^2
		}

		agentUI.setPower("wind", index, windPower);

		latestWindPower[index] = windPower; //update graphing value with latest data

		lastAction = checkInterval(std::bind(&Bucket::chargeBattery, this, std::placeholders::_1), windPower*60, lastAction);
		suspendThread(100); //sleep for performance
	}
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



//increments thread count and expands vector by 1
void incrementCount(std::string type) {
	if (strCompare(type, "wind")) {
		windCount++;
		latestWindPower.push_back(0);
	}

	if (strCompare(type, "consumer")) {
		consumerCount++;
		latestPowerConsumption.push_back(0);
	}
}

//Main thread function that represents agents
void Bucket::megaThread(MainWindow &w, std::unordered_map<std::string, int> headers, std::vector<std::string> data) {

	//get the agent type
	std::string tableName = data_string(headers, data, "Name");
	std::string type = data_string(headers, data, "Type");

	//set up time tracking variables
	//int chargeBatteryTick = 0;
	//int drainBatteryTick = 0;

	//using namespace std::placeholders; //for _1

	countMutex.lock(); //need to lock code segment until thread is assigned index
	incrementCount(type);

	AgentUI agentUI;
	QObject::connect(&agentUI, &AgentUI::addAgentToUI, &w, &MainWindow::addWidget);

	//kick off agent process
	if (strCompare(type, "wind")) {
		int index = windCount-1;
		countMutex.unlock();
		agentUI.newAgent(tableName, type, 0, index);
		windGeneration(tableName, index, w, agentUI);
	}

	else if (strCompare(type, "solar")) {
		int index = solarCount-1;
		countMutex.unlock();
		agentUI.newAgent(tableName, type, 0, index);
		solarGeneration(tableName, index, w, agentUI);
	}

	else if (strCompare(type, "consumer")) {
		int index = consumerCount-1;
		countMutex.unlock();
		agentUI.newAgent(tableName, type, 0, index);
		powerConsumption(tableName, index, w, agentUI);
	}

	else { //no case, unlock mutex and do no thread actions
		countMutex.unlock();
	}
}
////// GENERAL THREAD FUNCTIONS END //////

AgentUI::~AgentUI()
{
}

void AgentUI::newAgent(std::string name, std::string type, double power, int index) {
	emit addAgentToUI(QString::fromStdString(name), QString::fromStdString(type), power, index);
}

void AgentUI::setPower(std::string type, int index, double power) {
	emit powerChanged(QString::fromStdString(type), index, power);
}