// 838Control.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include "maincontrol.h"
#include "ImgProc.h"

void log(boost::posix_time::ptime ts, unsigned int cat, std::string caller, std::string msg)
{
	std::cout << msg << std::endl;
}


int main()
{


	/*remove later*/
	ImgProc findMark;
	cv::Mat src = cv::imread("H:/WorkDir/AufnahmenAconity/S1_M1_750us.bmp", cv::IMREAD_GRAYSCALE);

	findMark.getCenterOfMarker(&src);

	LUNOBackend::maincontrol m_maincontrol("settings.def"); 
    std::cout << "Hello World!\n"; 

	m_maincontrol.init();
	m_maincontrol.sigLogMsg.connect(log);
	if (m_maincontrol.m_baslerSensor->getConnectionState())
		m_maincontrol.m_baslerSensor->setExposureTime(1000);
	m_maincontrol.run();
	m_maincontrol.stop();
}
