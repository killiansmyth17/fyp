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
#include "QCustomPlotTestVS.h"


int tick = 0; //increments 10 times every second, let 10 ticks represent a minute (1 tick = 6 seconds)
int ticksPerAction = 10; //one action every 10 ticks, multiply watts by 60 to get joules

double joules = 0; //joules in battery (kJ for now)
double batteryCapacity = 50000;

//graphing values:
double latestWindPower = 0;

std::mutex joulesMutex;

//create alias for 2D Vector
using Vector1D = std::vector<std::string>;
using Vector2D = std::vector<Vector1D>;

////// GENERATOR SPECS BEGIN //////
std::unordered_map<std::string, double> windTurbine{
	{"ratedPower", 3.5}, //kiloWatts, update every time you update k
	{"ratedWindSpeed", 14},
	{"cutOut", 30},
	{"cutIn", 2},
	{"k", 0.017854714} //constant, update every time you update ratedPower (ratedPower/(ratedWindSpeed*ratedWindSpeed))
};
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

//hacky way to convert ticks to 24 hour clock as int
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

int Bucket::getTimetable(std::string tableName, std::unordered_map<int, double> &datamap) {

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

void Bucket::windGeneration() {
	//get timetable
	std::unordered_map<int, double> windTimetable;
	getTimetable("Wind Speeds", windTimetable); //datamap passed by reference

	int lastAction = 0;
	
	while (true) {
		suspendThread(100); //sleep for performance

		double windSpeed = windTimetable[getTime()];

		//calculate power generation from wind speed
		double windPower;
		if (windSpeed > windTurbine["cutOut"] || windSpeed < windTurbine["cutIn"]) { //if turbine can't turn
			windPower = 0;
		} else if (windSpeed > windTurbine["ratedWindSpeed"]) { // power gen caps at ratedPower when ratedWindSpeed is reached
			windPower = windTurbine["ratedPower"];
		} else {
			windPower = windSpeed * windSpeed * windTurbine["k"]; // P = kv^2
		}

		latestWindPower = windPower; //update graphing value with latest data

		lastAction = checkInterval(std::bind(&Bucket::chargeBattery, this, std::placeholders::_1), windPower, lastAction);
	}
}

//SQL data handling begin
int data_stoi(std::unordered_map<std::string, int> headers, std::vector<std::string> data, std::string query) {
	return stoi(data[headers[query]]);
}

std::string data_string(std::unordered_map<std::string, int> headers, std::vector<std::string> data, std::string query) {
	return data[headers[query]];
}

boolean strCompare(std::string str, std::string comp) {
	return str.compare(comp) == 0;
}
//SQL data handling end

//Main thread function that represents agents
void Bucket::megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data) {

	//get the agent type
	std::string type = data_string(headers, data, "Type");

	//set up time tracking variables
	int chargeBatteryTick = 0;
	int drainBatteryTick = 0;

	using namespace std::placeholders; //for _1

	//kick off agent process
	if (strCompare(type, "wind")) windGeneration();
}
////// GENERAL THREAD FUNCTIONS END //////
