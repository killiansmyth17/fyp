// Blank_VS2010.cpp : main project file.
#include <vector>
#include <unordered_map>
#include <string> //needed for map as nested character pointers suck

#include "stdafx.h"
#include "Form1.h"
#include "Bucket.h"
#include "sqlite3.h"

using namespace Blank_VS2010;

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
			std::cout << index << ": " << argv[headers[index]] << std::endl;
		}
	}

	//make thread
	threads.push_back(std::thread(megaThread, headers, argv));

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

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create & kick off threads
	int rc = createThreads();
	if (rc) return 1;

	// Set up timer thread last, kicks off whole process
	std::thread timeThread(timer);
	timeThread.detach();

	// Create the main window and run it

	Application::Run(gcnew Form1());

	return 0;
}
