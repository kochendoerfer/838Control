#include "logging.h"
//#include "Settings.h"

#include <boost/filesystem.hpp>

using namespace LUNOBackend;

logging::logging(std::string path) : m_logPath(path)
{
	m_facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");

	bool rc = checkLogPath(&m_logPath);

	if (rc)
	{
		if (m_logPath.back() != '/' && m_logPath.back() != '\\')
		{
			m_logPath += "/";
		}

		m_logPath += createLogFileName();
		m_logFile.open(m_logPath);
	}
}

logging::~logging()
{
	m_logFile.close();
}


void LUNOBackend::logging::addLog(boost::posix_time::ptime ts, unsigned int cat, std::string caller, std::string msg)
{
	boost::mutex::scoped_lock lock(io_mutex);
	try {
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");
		logEntry data;

		data.ts = ts;
		data.caller = caller;
		data.cat = cat;
		data.msg = msg;

		m_logList.push_back(data);

		if (m_logList.size() > 500)
		{
			m_logList.erase(m_logList.begin(), m_logList.begin() + (m_logList.size() - 500));
		}

		std::stringstream ss;

		ss.imbue(std::locale(ss.getloc(), facet));
		ss << ts;

		m_logFile << ss.str() << "\t";
		m_logFile << std::to_string(cat) << "\t";
		m_logFile << caller << "\t";
		m_logFile << msg << "\n";

		m_logFile.flush();
	}
	catch (std::exception& e)
	{
		std::string errstr = e.what();
		std::cerr << errstr << std::endl;
	}
}

void LUNOBackend::logging::clearLog()
{
	m_logList.clear();
}

void LUNOBackend::logging::saveLog()
{
	//boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");

	bool rc = checkLogPath(&m_logPath);

	std::ofstream myfile;

	if (rc)
	{
		std::stringstream ss;

		ss.imbue(std::locale(ss.getloc(), m_facet));

		m_logPath += createLogFileName();
		myfile.open(m_logPath);

		for (auto i = 0; i < m_logList.size(); i++)
		{
			ss << m_logList[i].ts;

			myfile << ss.str() << "\t";
			myfile << std::to_string(m_logList[i].cat) << "\t";
			myfile << m_logList[i].caller << "\t";
			myfile << m_logList[i].msg << "\n";

			ss.str(std::string());
		}

		myfile.close();
	}
}

std::vector<LUNOBackend::logging::logEntry>* LUNOBackend::logging::getLog()
{
	return &m_logList;
}

bool LUNOBackend::logging::checkLogPath(std::string *path)
{
	bool rc = true;

	if (path->size() > 0)
	{
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");

		boost::filesystem::path p(*path);

		if (!boost::filesystem::exists(p))
		{
			rc = boost::filesystem::create_directories(p);
		}
	}
	else
		rc = false;

	return rc;
}

std::string LUNOBackend::logging::createLogFileName()
{
	//boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f");

	std::stringstream ss;

	ss.imbue(std::locale(ss.getloc(), m_facet));

	std::string t_str = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

	std::string filename = t_str + ".log";

	return filename;
}
