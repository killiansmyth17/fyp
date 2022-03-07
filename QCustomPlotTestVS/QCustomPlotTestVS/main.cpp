// Blank_VS2010.cpp : main project file.
#include <vector>
#include <unordered_map>
#include <string> //needed for map as nested character pointers suck

#include "Bucket.h"
#include "sqlite3.h"

#include "QCustomPlotTestVS.h"
#include "MainWindow.h"
#include <QtWidgets/QApplication>

std::unordered_map<std::string, int> headers;
std::vector<std::thread> threads;

//callback function for sqlite query, processes SQL data line by line (one line passed into this function at a time)
static int processAgent(void* arg, int argc, char** argv, char** colName) {

	//populate headers map ONCE for robust indexing of data with column headers (first line will be headers)
	if (!(headers["Count"])) {
		for (int i = 0; i < argc; i++) {
			std::string index(colName[i]);
			headers[index] = i;
		}
	}

	//convert argv from nested char pointers to vector of strings for robustness
	std::vector<std::string> data;
	for (int i = 0; i < argc; i++) {
		data.push_back(argv[i]);
	}

	//make thread
	MainWindow* w = (MainWindow *)arg;
	threads.push_back(std::thread(&Bucket::megaThread, Bucket(), std::ref(*w), headers, data));

	return 0;
}

int createThreads(MainWindow &w) {
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

	const char* sql = "SELECT * FROM \"Agents\"";
	rc = sqlite3_exec(db, sql, processAgent, &w, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db); //close db connection
	return 0;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	// Create & kick off threads
	int rc = createThreads(w);
	if (rc) return 1;

	// Set up timer thread last, kicks off whole process
	std::thread timeThread(&Bucket::timer, Bucket(), std::ref(w));
	timeThread.detach();

	w.show();

    return a.exec();
}
