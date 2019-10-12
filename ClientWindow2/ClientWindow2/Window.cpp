#include "Window.h"

using namespace System;
using namespace System::Collections::Generic;

[STAThreadAttribute]

int main(array<String^> ^Args)
{
	Windows::Forms::Application::EnableVisualStyles();
	
	ClientWindow2::Window^ mainWindow = gcnew ClientWindow2::Window();
	Windows::Forms::Application::Run(mainWindow);

	return 0;
}