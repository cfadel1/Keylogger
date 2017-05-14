#ifndef HELPER_H
#define HELPER_H
#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

namespace Helper
{
	template <class T>
	std::string ToString(const T &);

	struct DateTime
	{
		int D, m, y, H, M, S;
		
		//local time on which the machine is running
		DateTime()
		{
			time_t ms = time(&ms);
			struct tm info;
			localtime_s(&info, &ms);

			//Day, Month (Jan = 0 so +1), year, Min, hour, sec
			D = info.tm_mday;
			m = info.tm_mon + 1;
			y = 1900 + info.tm_year;
			M = info.tm_min;
			H = info.tm_hour;
			S = info.tm_sec;
		}

		//DateTime constructors
		DateTime(int D, int m, int y, int H, int M, int S)
			: D(D), m(m), y(y), H(H), M(M), S(S) {}

		DateTime(int D, int m, int y)
			: D(D), m(m), y(y), H(0), M(0), S(0) {}

		//Gets date as DD/mm/YYYY
		std::string GetDateString() const
		{
			std::string day = (D < 10 ? "0" : std::string()) + ToString(D);
			std::string month = (m < 10 ? "0" : std::string()) + ToString(m);
			std::string year = ToString(y);

			return day + "/" + month + "/" + year;
		}

		//Gets time as HH:MM:SS
		std::string GetTimeString() const
		{
			std::string hour = (H < 10 ? "0" : std::string()) + ToString(H);
			std::string minutes = (M < 10 ? "0" : std::string()) + ToString(M);
			std::string seconds = (S < 10 ? "0" : std::string()) + ToString(S);

			return hour + ":" + minutes + ":" + seconds;
		}

		std::string GetDateTimeString(const std::string &sep = ":") const
		{
			return GetDateString() + "  " + GetTimeString();
		}
	};

	template <class T>

	std::string ToString(const T &e)
	{
		ostringstream s;
		s << e;

		return s.str();
	}

	//Creates a log file for the Keylogger
	void WriteAppLog(const std::string &s)
	{
		std::ofstream file("AppLog.txt", std::ios::app);

		file << "[" << DateTime().GetDateTimeString() << "]" << std::endl << s << std::endl;
		file.close();
	}
}
#endif 