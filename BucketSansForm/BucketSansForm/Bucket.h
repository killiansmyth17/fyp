#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>

//namespace bkt {
	//variables
	extern int tick;
	extern double water;

	//functions
	void timer();
	void megaThread(std::unordered_map<std::string, int> headers, std::vector<std::string> data);
//}