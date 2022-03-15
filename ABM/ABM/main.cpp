// Blank_VS2010.cpp : main project file.
#include <vector>
#include <string> //needed for map as nested character pointers suck

#include "Bucket.h"
#include "sqlite3.h"

#include "Graph.h"
#include "MainWindow.h"
#include <QtWidgets/QApplication>

std::vector<std::thread> threads;

//callback function for sqlite query, processes SQL data row by row (one row passed into this function at a time)
static int processAgent(void* arg, int argc, char** argv, char** colName) {	
	
	std::string tableName;
	std::string type;

	//check column name so order in db doesn't matter
	if (!strcmp(colName[0], "Name")) {
		tableName = argv[0];
		type = argv[1];
	}
	else {
		tableName = argv[1];
		type = argv[0];
	}

	//make thread
	MainWindow* w = (MainWindow *)arg; //cast arg back to MainWindow pointer
	threads.push_back(std::thread(&Bucket::megaThread, Bucket(), std::ref(*w), tableName, type));

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

	std::string sql = "SELECT Name, Type FROM \"Agents\"";
	rc = sqlite3_exec(db, sql.c_str(), processAgent, &w, &zErrMsg);
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
	threads.push_back(std::thread(&Bucket::timer, Bucket(), std::ref(w)));

	w.show();

    int ret = a.exec(); //Qt Application main event loop

	tick = maxTick+1; //forcefully mark simulation as complete (forces threads to stop looping)

	//confirm all threads have terminated
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	return ret;
}
