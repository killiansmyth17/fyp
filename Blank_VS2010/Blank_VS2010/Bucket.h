#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

//namespace bkt {
	//variables
	extern int tick;
	extern double water;

	//functions
	void timer();
	void emptyBucket();
	void fillBucket();
//}