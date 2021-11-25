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


int tick = 0; //increments 10 times every second
int ticksPerAction = 10; //one action every 10 ticks, divide 

double joules = 0; //joules in battery (megajoules for now)
double batteryCapacity = 500;

std::mutex joulesMutex;

//create alias for 2D Vector
using Vector1D = std::vector<std::string>;
using Vector2D = std::vector<Vector1D>;

////// GETTERS BEGIN //////
//double Bucket::getWater() {
//	return water;
//}
////// GETTERS END //////



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
		//if (tick % 10 == 0) {
			//QCustomPlotTestVS test;
			//test.plotData(tick);
		//}
		tick++;
	}
}

//roundabout way to convert decimal minutes to 24 hour clock
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
int Bucket::checkInterval(std::function<void(int)> callback, int amount, int tickRate, int lastAction) {
	int tickdiff = tick - lastAction;

	if (tickdiff > tickRate) {
		int intervals = tickdiff / tickRate;
		callback(intervals*amount);
		return lastAction += tickRate;
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

int Bucket::getTimetable(std::string tableName, std::unordered_map<int, int> &datamap) {

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
	const char* query = "SELECT * FROM Agents Wind Watts";
	rc = sqlite3_exec(db, query, handleData, &data, &zErrMsg);
	if (rc != SQLITE_OK) {
		 std::cerr << "SQL error: " << zErrMsg << std::endl;
		 sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection

	for (int i = 1; i < data.size(); i++) {
		int index = std::stoi(data[i][0]);
		datamap[index] = std::stoi(data[i][1]);
	}

	return 0;
}

void Bucket::windGeneration() {
	//get timetable
	std::unordered_map<int, int> datamap;
	getTimetable("Wind Generation Watts", datamap); //datamap passed by reference

	int chargeBatteryTick = 0;
	while (true) {
		suspendThread(100); //sleep for performance

		int chargeBatteryRate = 10; //number of ticks to wait before applying change to battery
		chargeBatteryTick = checkInterval(std::bind(&Bucket::chargeBattery, this, std::placeholders::_1), datamap[getTime()], chargeBatteryRate, chargeBatteryTick);
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

	//get the shit
	std::string type = data_string(headers, data, "type");

	//set up time tracking variables
	int chargeBatteryTick = 0;
	int drainBatteryTick = 0;

	using namespace std::placeholders; //for _1

	//kick off agent process
	if (strCompare(type, "wind")) windGeneration();
}
////// GENERAL THREAD FUNCTIONS END //////
