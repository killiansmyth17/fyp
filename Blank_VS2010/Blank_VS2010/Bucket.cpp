#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "stdafx.h"
#include "Bucket.h"

double water = 0;

int tick = 0;
//std::mutex mutex;

void changeWater(double waterTick) {
	//mutex.lock();

	water += waterTick;
	std::cout << water << " litres of water in the bucket\n" << std::endl;

	//mutex.unlock();
}

void suspendThread(int milliseconds) {
	std::chrono::milliseconds timespan(milliseconds);
	std::this_thread::sleep_for(timespan);
}

void fillBucket(void) {
	double waterRate = 1.0;
	int waterInterval = 30; //fill every 30 ticks * 100 milliseconds
	int lastAction = 0;

	while (true) {
		suspendThread(20);
		if ((tick - lastAction) > waterInterval) {
			lastAction += waterInterval;

			changeWater(waterRate);
		}
	}
}

void emptyBucket(void) {
	double waterRate = -0.5;
	int waterInterval = 50; //empty every 50 ticks * 100 milliseconds
	int lastAction = 0;

	while (true) {
		suspendThread(20);
		if ((tick - lastAction) > waterInterval) {
			lastAction += waterInterval;

			changeWater(waterRate);
		}
	}
}

void timer(void) {
	while (true) {
		suspendThread(100);
		tick++;
	}
}