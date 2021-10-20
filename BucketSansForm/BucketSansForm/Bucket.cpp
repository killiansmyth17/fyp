#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>

#include "stdafx.h"
#include "Bucket.h"

double water = 0;

int tick = 0;
std::mutex waterMutex;



///////////////////////////////////
////// MUTEX FUNCTIONS BEGIN //////
///////////////////////////////////
void changeWater(double waterTick) {
	waterMutex.lock();

	water += waterTick;
	std::cout << water << " litres of water in the bucket\n" << std::endl;

	waterMutex.unlock();
}
/////////////////////////////////
////// MUTEX FUNCTIONS END //////
/////////////////////////////////



//////////////////////////////////////////
////// AGENT ACTION FUNCTIONS BEGIN //////
//////////////////////////////////////////
void fillBucket(int intervals) {
	changeWater(intervals * 1);
}

void emptyBucket(int intervals) {
	changeWater(intervals * -1);
}
////////////////////////////////////////
////// AGENT ACTION FUNCTIONS END //////
////////////////////////////////////////



////////////////////////////////////////////
////// GENERAL THREAD FUNCTIONS BEGIN //////
////////////////////////////////////////////
void suspendThread(int milliseconds) {
	std::chrono::milliseconds timespan(milliseconds);
	std::this_thread::sleep_for(timespan);
}

void timer(void) {
	while (true) {
		suspendThread(100);
		tick++;
	}
}

//Check if enough time has passed for action to happen
int checkInterval(void (*callback)(int), int tickRate, int lastAction) {
	int tickdiff = tick - lastAction;

	if (tickdiff > tickRate) {
		int intervals = tickdiff / tickRate;
		callback(intervals);
		return lastAction += tickRate;
	}

	return lastAction;
}

//Main thread function that represents agents
void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data) {
	//set up agent variables
	int fillBucketRate = stoi(data[headers["FILL_BUCKET"]]);
	int emptyBucketRate = stoi(data[headers["EMPTY_BUCKET"]]);

	//set up time tracking variables
	int fillBucketTick = 0;
	int emptyBucketTick = 0;

	//do agent stuff
	while (true) {
		suspendThread(10); //suspend thread for performance
		if (fillBucketRate) fillBucketTick = checkInterval(fillBucket, fillBucketRate, fillBucketTick);
		if (emptyBucketRate) emptyBucketTick = checkInterval(emptyBucket,emptyBucketRate, emptyBucketTick);
	}
}
//////////////////////////////////////////
////// GENERAL THREAD FUNCTIONS END //////
//////////////////////////////////////////