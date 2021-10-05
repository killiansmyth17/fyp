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

	// Create the main window and run it
	Application::Run(gcnew Form1());

	bucketMain();

	return 0;
}
