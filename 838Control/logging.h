#pragma once
#include <string>
#include <vector>
#include <fstream>

#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>

#include "lunodefs.h"


namespace LUNOBackend
{
	class logging
	{
	public:
		struct logEntry
		{
			boost::posix_time::ptime ts;
			unsigned int cat;
			std::string caller;
			std::string msg;
		};


		logging(std::string path);
		~logging();

		void addLog(boost::posix_time::ptime ts, unsigned int cat, std::string caller, std::string msg);
		void clearLog();
		void saveLog();

		std::vector<logEntry>* getLog();

	private:
		bool checkLogPath(std::string *path);
		std::string createLogFileName();

		std::vector<logEntry> m_logList;
		std::string m_logPath;
		std::ofstream m_logFile;

		boost::posix_time::time_facet *m_facet = NULL;
		boost::mutex io_mutex;
	};
}
