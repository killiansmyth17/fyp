// Blank_VS2010.cpp : main project file.
#include <vector>
#include <unordered_map>
#include <string> //needed for map as nested character pointers suck

//#include "stdafx.h"
#include "Bucket.h"
#include "sqlite3.h"

#include "QCustomPlotTestVS.h"
#include <QtWidgets/QApplication>

std::unordered_map<std::string, int> headers;
std::vector<std::thread> threads;

//callback function for sqlite query
static int processAgent(void* NotUsed, int argc, char** argv, char** colName) {

	int i;
	//populate headers map ONCE for robust indexing of data with column headers
	if (!(headers["Count"])) {
		for (i = 0; i < argc; i++) {
			std::string index(colName[i]);
			headers[index] = i;
		}
	}

	//convert argv from nested char pointers to vector of strings for robustness
	std::vector<std::string> data;
	for (i = 0; i < argc; i++) {
		data.push_back(argv[i]);
	}

	//make thread
	threads.push_back(std::thread(&Bucket::megaThread, Bucket(), headers, data));

	return 0;
}

int createThreads() {
	//Create database connection
	sqlite3* db; //database connection
	int rc; //return code 
	char* zErrMsg = 0;

	rc = sqlite3_open("Agents.db", &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return 1;
	}

	const char* sql = "SELECT * FROM AGENTS";
	rc = sqlite3_exec(db, sql, processAgent, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection
	return 0;
}

int main(int argc, char *argv[])
{
	// Create & kick off threads
	int rc = createThreads();
	if (rc) return 1;

	// Set up timer thread last, kicks off whole process
	std::thread timeThread(&Bucket::timer, Bucket());
	timeThread.detach();

    QApplication a(argc, argv);
    QCustomPlotTestVS w;
    w.show();
    return a.exec();
}
