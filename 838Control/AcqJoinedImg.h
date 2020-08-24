#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/signals2/signal.hpp>
#include "SettingsVals.h"
#include "LUNOdefs.h"
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

class AcqJoinedImg
{
public:
	AcqJoinedImg();
	~AcqJoinedImg();

	void init(LUNOBackend::SettingsVals * settings);
	void clear();
	void addImage(cv::Mat &src, double posX, double posY);
	void saveImage(std::string path);

	boost::signals2::signal<void()> sigImgAdded;
	boost::signals2::signal<void(boost::posix_time::ptime, unsigned int, std::string, std::string)> sigLogMsg;

private:

	std::string m_path;
	cv::Mat m_joinedImg;
	//size of area to be rastered in mm
	double m_sizeX = 0;
	double m_sizeY = 0;
	double m_mmPerPixel = 0.0048;
	bool m_firstImg = true;
	double m_StartAxisXmm = 0;
	double m_StartAxisYmm = 0;
	int m_squareImgSize = 1000;
	LUNOBackend::SettingsVals *m_settings = NULL;
};

