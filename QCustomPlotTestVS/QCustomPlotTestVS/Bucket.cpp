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


int tick = 0;
double water = 0;
std::mutex waterMutex;



////// GETTERS BEGIN //////
double Bucket::getWater() {
	return water;
}
////// GETTERS END //////



////// MUTEX FUNCTIONS BEGIN //////
void Bucket::changeWater(double waterTick) {
	waterMutex.lock();

	water += waterTick;
	std::cout << water << " litres of water in the bucket\n" << std::endl;

	waterMutex.unlock();
}
////// MUTEX FUNCTIONS END //////



////// AGENT ACTION FUNCTIONS BEGIN //////
void Bucket::fillBucket(int intervals) {
	changeWater(intervals * 1);
}

void Bucket::emptyBucket(int intervals) {
	changeWater(intervals * -1);
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
	int fillBucketRate = stoi(data[headers["FILL_BUCKET"]]);
	int emptyBucketRate = stoi(data[headers["EMPTY_BUCKET"]]);

	//set up time tracking variables
	int fillBucketTick = 0;
	int emptyBucketTick = 0;

	using namespace std::placeholders; //for _1
	//do agent stuff
	while (true) {
		suspendThread(10); //suspend thread for performance
		
		if (fillBucketRate) fillBucketTick = checkInterval(std::bind(&Bucket::fillBucket, this, _1), fillBucketRate, fillBucketTick);
		if (emptyBucketRate) emptyBucketTick = checkInterval(std::bind(&Bucket::emptyBucket, this, _1), emptyBucketRate, emptyBucketTick);
	}
}
////// GENERAL THREAD FUNCTIONS END //////
