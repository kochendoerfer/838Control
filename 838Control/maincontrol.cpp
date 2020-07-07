#include <boost/filesystem.hpp>

#include "LUNOdefs.h"
#include "maincontrol.h"
#include "ImgProc.h"

#ifdef LUNODEBUG
#endif

LUNOBackend::maincontrol::maincontrol()
{
	m_log = NULL;
	m_baslerSensor = NULL;

	m_log = new LUNOBackend::logging("log");
	sigLogMsg.connect(boost::bind(&logging::addLog, m_log, _1, _2, _3, _4));
}

LUNOBackend::maincontrol::maincontrol(std::string settingsFilename) : m_settingsFilename(settingsFilename)
{
	bool rcINI = false;
	bool rcLOG = false;
	m_log = NULL;
	m_baslerSensor = NULL;

	rcINI = m_settings.init(m_settingsFilename); 

	std::string logpath = "C:\\temp";
	if (rcINI)
		rcLOG = m_settings.getValString("SystemLogPath", logpath);

	m_log = new LUNOBackend::logging(logpath);

	sigLogMsg.connect(boost::bind(&logging::addLog, m_log, _1, _2, _3, _4));

	if (!rcINI||m_settings.m_vals.size()==0)
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_FATAL_ERROR, "[maincontrol]", "No Settings.def, PANIK");
}

LUNOBackend::maincontrol::~maincontrol()
{
	if (m_baslerSensor)
	{
		m_baslerSensor->stop();
		delete m_baslerSensor;
		m_baslerSensor = NULL;
	}

	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Maincontrol]", "Bye, bye. Thank you for making a simple program very happy.");

	//logging must be the last destroyed object
	if (m_log)
	{
		//m_log->saveLog();
		delete m_log;
		m_log = NULL;
	}
}

bool LUNOBackend::maincontrol::init()
{
	bool rc = true;
	int waitTime = 100;

	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Maincontrol]", "Initialising System");

	sigInitStep("Initialising Camera ...");
	m_baslerSensor = new Basler();
	m_baslerSensor->sigLogMsg.connect(sigLogMsg);
	m_baslerSensor->sigCamConnectionState.connect(boost::bind(&maincontrol::cameraState, this, _1));
	m_baslerSensor->init(&m_settings);
	m_baslerSensor->start();
	boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
	sigInitStep("Initialising Camera Done");

	sigInitStep("Initialising PLC Connection ...");
	m_opcclient = new LUNOBackend::toolControlMTX();
	m_opcclient->sigLogMsg.connect(sigLogMsg);
	m_opcclient->init(&m_settings);
	m_opcclient->start();
	m_opcclient->sigFindMarkerStartRequest.connect(boost::bind(&maincontrol::resetMarker, this));
	m_opcclient->sigFindMarkerEndRequest.connect(boost::bind(&maincontrol::calcTransformation, this));
	m_opcclient->sigFindMarkerRequest.connect(boost::bind(&maincontrol::detectMarker, this,_1,_2));

	m_opcclient->sigRasterStartRequest.connect(boost::bind(&maincontrol::resetRaster, this));
	m_opcclient->sigRasterEndRequest.connect(boost::bind(&maincontrol::FinalizeRasterImage, this));
	m_opcclient->sigRasterRequest.connect(boost::bind(&maincontrol::addRasterImage, this, _1, _2));
	m_opcclient->sigCloseGUIRequest.connect(boost::bind(&maincontrol::stop, this));

	boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
	sigInitStep("Initialising PLC Connection Done");

	sigInitStep("Initialising Tool Done");
	return rc;
}

bool LUNOBackend::maincontrol::stop()
{
	m_baslerSensor->stop();
	m_opcclient->stop();

	m_running = false;

	return true;
}

void LUNOBackend::maincontrol::run()
{
	cv::Mat img(100,100,CV_8UC3);
	while (m_running)
	{
		bool actViewer = true;
		//m_settings.getValBool("ImageViewer", actViewer);

		if (m_baslerSensor)
		{
			if (actViewer)
			{
				try
				{
					//cv::copyTo(*m_baslerSensor->getLiveImage(), img);
					m_baslerSensor->getLiveImage().copyTo(img);

					if (img.rows > 0 && img.cols > 0)
					{
						cv::line(img, cv::Point(img.cols / 2, 0), cv::Point(img.cols / 2, img.rows), cv::Scalar(0, 0, 255));
						cv::line(img, cv::Point(0, img.rows/2), cv::Point(img.cols, img.rows/2), cv::Scalar(0, 0, 255));
						cv::imshow("dd", img);
					}
					cv::waitKey(1);
				}
				catch (cv::Exception e)
				{
					std::string sss;
					sss = e.what();
				}
			}
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
	}
}

void LUNOBackend::maincontrol::resetMarker()
{
	m_axisTransformation.clear();
}

void LUNOBackend::maincontrol::calcTransformation()
{
	cv::Point2d translation;
	cv::Point2d rotation;
	cv::Point2d scale;

	bool rc = false;
	rc = m_axisTransformation.process(&translation,&rotation,&scale);

	if (m_opcclient)
	{
		m_opcclient->setTransformation(translation, rotation, scale);
	}

	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[maincontrol]", "do something");
}

void LUNOBackend::maincontrol::detectMarker(double v1,double v2)
{
	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[maincontrol]", "Detect MarkerPos");

	ImgProc findMark;
	findMark.init(&m_settings);
	findMark.sigLogMsg.connect(sigLogMsg);
	if (m_opcclient)
	{
		findMark.sigAlgStatus.connect([this](int i) { m_opcclient->statusMarkerDetection(i); });
	}
	cv::Point2d markpos;
	cv::Point2d axispos;

	axispos.x = v1;
	axispos.y = v2;

	markpos = findMark.getCenterOfMarker(m_baslerSensor->getValue());//TODO BHO die Berechnung dauert scheinbar >20s evtl in einen Thread verlagern
	m_axisTransformation.addPoint(axispos, markpos);

	findMark.sigLogMsg.disconnect_all_slots();

	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[maincontrol]", "Mark at:"+ std::to_string(markpos.x) + ","+ std::to_string(markpos.y));
}

void LUNOBackend::maincontrol::resetRaster()
{
	m_procRasterImage.clear();
}

void LUNOBackend::maincontrol::FinalizeRasterImage()
{
	std::string filename = "";
	m_settings.getValString("RasterImageFilename", filename);

	m_procRasterImage.saveImage(filename);
}

void LUNOBackend::maincontrol::addRasterImage(double v1, double v2)
{
	m_procRasterImage.addImage(*m_baslerSensor->getValue(), v1, v2);
}

void LUNOBackend::maincontrol::cameraState(bool state)
{
	if (m_opcclient)
		m_opcclient->setCameraState(!state);
}