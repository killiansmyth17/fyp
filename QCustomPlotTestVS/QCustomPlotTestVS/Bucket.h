#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>


class Bucket {
	public:
		//variables
		int tick;
		double water;

		//getters
		double getWater();

		//functions
		void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data);
		void timer(void);

	private:
		void changeWater(double waterTick);
		void fillBucket(int intervals);
		void emptyBucket(int intervals);
		void suspendThread(int milliseconds);
		int checkInterval(std::function<void(int)> callback, int tickRate, int lastAction);
};
