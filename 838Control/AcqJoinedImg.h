#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/signals2/signal.hpp>
#include "SettingsVals.h"

class AcqJoinedImg
{
public:
	AcqJoinedImg();
	~AcqJoinedImg();

	void init(LUNOBackend::SettingsVals * settings);
	void clear();
	void addImage(cv::Mat &src, int posX, int posY);
	void saveImage(std::string name);

	boost::signals2::signal<void()> sigImgAdded;
	boost::signals2::signal<void(std::string)> sigLogMsg;

private:

	std::string m_path;
	cv::Mat m_joinedImg;
	//size of area to be rastered in mm
	int m_sizeX = 0;
	int m_sizeY = 0;
	double m_mmPerPixel = 0.0048;
	LUNOBackend::SettingsVals *m_settings = NULL;
};

