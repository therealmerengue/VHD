#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(/*array<String^>^ arg*/)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	VHD::MainForm mf;
	Application::Run(%mf);
}

