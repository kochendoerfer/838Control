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
	LUNOBackend::maincontrol m_maincontrol("settings.def"); 
    std::cout << "Hello World!\n"; 

	m_maincontrol.init();
	m_maincontrol.sigLogMsg.connect(log);
	m_maincontrol.run();
	m_maincontrol.stop();
}
