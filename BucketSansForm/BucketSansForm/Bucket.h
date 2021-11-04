#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>


//variables
extern int tick;
extern double water;

class Bucket {

public:

	//functions
	void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data);
	void timer(void);

	//getters
	//static double getWater();

private:
	void changeWater(double waterTick);
	void fillBucket(int intervals);
	void emptyBucket(int intervals);
	void suspendThread(int milliseconds);
	int checkInterval(std::function<void(int)> callback, int tickRate, int lastAction);
};
