// Blank_VS2010.cpp : main project file.
#include "stdafx.h"
#include "Form1.h"
#include "Bucket.h"

using namespace Blank_VS2010;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	std::thread emptyThreads[20];
	std::thread fillThreads[20];

	// Create & kick off threads
	for (int i = 0; i < 10; i++) {
		fillThreads[i] = std::thread(fillBucket);
		emptyThreads[i] = std::thread(emptyBucket);

		fillThreads[i].detach();
		emptyThreads[i].detach();
	}

	// Set up timer thread last, kicks off whole process
	std::thread timeThread(timer);
	timeThread.detach();

	// Create the main window and run it

	Application::Run(gcnew Form1());

	return 0;
}
