#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional> //for callbacks using class member

//#include "stdafx.h"
#include "Bucket.h"
#include "QCustomPlotTestVS.h"


int tick = 0; //increments 10 times every second
double joules = 0;
std::mutex joulesMutex;



////// GETTERS BEGIN //////
//double Bucket::getWater() {
//	return water;
//}
////// GETTERS END //////



////// MUTEX FUNCTIONS BEGIN //////
void Bucket::changeJoules(double joulesTick) {
	joulesMutex.lock();

	joules += joulesTick;
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

//Check if enough time has passed for action to happen
int Bucket::checkInterval(std::function<void(int)> callback, int tickRate, int lastAction) {
	int tickdiff = tick - lastAction;

	if (tickdiff > tickRate) {
		int intervals = tickdiff / tickRate;
		callback(intervals);
		return lastAction += tickRate;
	}

	return lastAction;
}

//Main thread function that represents agents
void Bucket::megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data) {
	//set up agent variables
	int chargeBatteryRate = stoi(data[headers["FILL_BUCKET"]]);
	int drainBatteryRate = stoi(data[headers["EMPTY_BUCKET"]]);

	//set up time tracking variables
	int chargeBatteryTick = 0;
	int drainBatteryTick = 0;

	using namespace std::placeholders; //for _1
	//do agent stuff
	while (true) {
		suspendThread(10); //suspend thread for performance
		
		if (chargeBatteryRate) chargeBatteryTick = checkInterval(std::bind(&Bucket::chargeBattery, this, _1), chargeBatteryRate, chargeBatteryTick);
		if (drainBatteryRate) drainBatteryTick = checkInterval(std::bind(&Bucket::drainBattery, this, _1), drainBatteryRate, drainBatteryTick);
	}
}
////// GENERAL THREAD FUNCTIONS END //////
