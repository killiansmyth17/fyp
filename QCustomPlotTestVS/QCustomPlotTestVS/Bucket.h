#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>

/*namespace bkt {
	class Bucket;
}*/


class Bucket {
	public:
		//variables
		int tick;
		double water;

		//functions
		void timer();
		void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data);

		double getWater();

	private:
		
		void changeWater(double waterTick);
};
