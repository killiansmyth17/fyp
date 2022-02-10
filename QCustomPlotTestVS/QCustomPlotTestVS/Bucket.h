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
extern bool wait;
extern int tick;
extern double joules;
extern std::vector<double> latestWindPower;
extern std::vector<double> latestPowerConsumption;

//alias for timetable datatype
using Timetable = std::unordered_map<int, double>;

class Bucket {

	public:

		//functions
		void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data);
		void timer(void);

	private:
		int getTime();
		int getTimetable(std::string tableName, Timetable &datamap);
		void powerConsumption(std::string tableName, int index);
		void windGeneration(std::string tableName, int index);
		void solarGeneration(std::string tableName, int index);
		void changeJoules(double joulesTick);
		void chargeBattery(int intervals);
		void drainBattery(int intervals);
		void suspendThread(int milliseconds);
		int checkInterval(std::function<void(int)> callback, double amount, int lastAction);
};
