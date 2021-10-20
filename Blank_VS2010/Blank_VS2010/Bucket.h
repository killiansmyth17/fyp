#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <string>
#include "stdafx.h"
#include "Bucket.h"

//namespace bkt {
	//variables
	extern int tick;
	extern double water;

	//functions
	void timer();
	void megaThread(std::unordered_map<std::string, int> headers, char** data);
//}