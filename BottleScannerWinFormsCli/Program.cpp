#using <System.dll>
#using <System.Windows.Forms.dll>

#include "MainForm.h"
#include <Windows.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace BottleScannerWinFormsCli;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);
  Application::Run(gcnew MainForm());
  return 0;
}
