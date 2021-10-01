#include <iostream>
#include <thread>
#include <mutex>

double water = 0;

//tick system variables
int threadCount = 2;
int threadCounter = 0;
int tick = 0;

std::mutex mutex;

void changeWater(double waterTick) {
	mutex.lock();

	water += waterTick;

	mutex.unlock();
}

void waitForTick(int currentTick) {
	mutex.lock();

	threadCounter++;

	if (threadCounter == threadCount) {
		std::cout << "Advanced to tick " << tick << std::endl;
		std::cout << water << " litres of water are in the bucket\n" << std::endl;
		tick++; //advancing tick outside of cout (i.e. no ++tick) because it immediately frees up threads and messes up print order otherwise
		threadCounter = 0;
	}

	mutex.unlock();

	while (currentTick == tick) {
		//do nothing
	}
}

void fillBucket(void) {
	double waterRate = 1.0;

	//while(true) {
	for(int i = 0; i<100; i++) {

		int currentTick = tick;
		double waterTick = 0;
		waterTick += waterRate;

		std::cout << "Filling" << std::endl;

		changeWater(waterTick);
		waitForTick(currentTick);
	}
}

void emptyBucket(void) {
	double waterRate = -0.5;

	//while(true) {
	for(int i = 0; i<100; i++) {

		int currentTick = tick;
		double waterTick = 0;
		waterTick += waterRate;

		std::cout << "Emptying" << std::endl;

		changeWater(waterTick);
		waitForTick(currentTick);
	}
}

int main()
{
	std::thread fillThread(fillBucket);
	std::thread emptyThread(emptyBucket);

	fillThread.join();
	emptyThread.join();

	return 1;
}