#include "Basler.h"

#include <iostream>

#include "LUNOdefs.h"
#include <boost/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

LUNOBackend::Basler::Basler()
{
	Pylon::PylonInitialize();
	m_running = false;
	m_img = NULL;
	m_img = new cv::Mat();
}


LUNOBackend::Basler::~Basler()
{
	if (m_img)
	{
		delete m_img;
		m_img = NULL;
	}
	try
	{
		Pylon::PylonTerminate();
	}
	catch (Pylon::GenericException &e)
	{
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", e.what());
	}
}

bool LUNOBackend::Basler::init(std::string path, std::string filename)
{
	bool rc = true;
	 m_path = path;
	 m_filename =filename;

	 if (boost::filesystem::exists(path))
	 {
		 //add path delimiter if not exist
		 if (m_path.back() != '/' && m_path.back() != '\\')
		 {
			 m_path += "/";
		 }
	 }
	 else
	 {
		 rc = false;
		 sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[vision]", "Init failed! Path does not exist");
	 }

	return rc;
}

void LUNOBackend::Basler::init(LUNOBackend::SettingsVals * settings)
{
	m_settings = settings;
}

bool LUNOBackend::Basler::start()
{
	bool rc = true;

	connect();
	if (!m_running)
	{
		m_running = true;

		m_threadStatus = new boost::thread(boost::bind(&LUNOBackend::Basler::run, this));
	}

	return rc;
}

bool LUNOBackend::Basler::stop()
{
	bool rc = true;

	if (m_running)
	{
		m_running = false;
		m_threadStatus->join();

		disconnect();
	}


	return rc;
}

bool LUNOBackend::Basler::setExposureTime(double exptime)
{
	bool rc = true;
	exptime *= 1000; //Timebase of Basler is µs, we are using ms
	Pylon::CFloatParameter expTime(m_nodemap, "ExposureTimeAbs");

	if (exptime < expTime.GetMin())exptime = expTime.GetMin();
	if (exptime > expTime.GetMax())exptime = expTime.GetMax();

	expTime.SetValue(exptime);

	return rc;
}

bool LUNOBackend::Basler::setFramerate(double framerate)
{
	bool rc = true;
	double *newFPS =0;

	//if (is_SetFrameRate(m_camHandle, framerate, newFPS) != IS_SUCCESS)
	//{
	//	rc = false;
	//	getError();
	//}

	return rc;
}

bool LUNOBackend::Basler::setROI(unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2)
{
	return false;
}

double LUNOBackend::Basler::getFramerate()
{
	double newFPS = 0;

	//if (is_GetFramesPerSecond(m_camHandle, &newFPS) != IS_SUCCESS)
	//{
	//	getError();
	//}

	return newFPS;
}

double LUNOBackend::Basler::getExposureTime()
{
	Pylon::CFloatParameter expTime(m_nodemap, "ExposureTimeAbs");
	double exptimeTST = (double)expTime.GetValue();
	return exptimeTST/1000;
}

cv::Mat* LUNOBackend::Basler::getValue()
{

	return m_img;
}

cv::Mat LUNOBackend::Basler::getLiveImage()
{
	cv::Mat img;
	cv::cvtColor(*m_img, img,cv::COLOR_GRAY2BGR);
	return img;
}

void LUNOBackend::Basler::exportdata()
{
	cv::imwrite(m_path + m_filename, *m_img);
}

bool LUNOBackend::Basler::connect()
{
	int rc = true;

	int numCams = 0;
	int cvtype = CV_8UC1;

	Pylon::DeviceInfoList plist;
	Pylon::CTlFactory::GetInstance().EnumerateDevices(plist);

	if (plist.size() > 0)
	{
		try
		{
			m_camera.Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		}
		catch (Pylon::RuntimeException e)
		{
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", e.what());
		}

		if (m_camera.IsPylonDeviceAttached())
		{
			try
			{
				numCams = 1;
				m_camera.Open();
				m_nodemap = &m_camera.GetNodeMap();


				Pylon::CIntegerParameter offsetX(m_nodemap, "OffsetX");
				Pylon::CIntegerParameter offsetY(m_nodemap, "OffsetY");
				Pylon::CIntegerParameter width(m_nodemap, "Width");
				Pylon::CIntegerParameter height(m_nodemap, "Height");

				Pylon::CEnumParameter pixelFormat(m_nodemap, "PixelFormat");

				double expTime;
				if (m_settings->getValDouble("ExposureTime", expTime))
					setExposureTime(expTime);

				// Remember the current pixel format.
				std::string oldPixelFormat = (std::string) pixelFormat.GetValue();

				sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Basler]", "Camera PixelFormat" + oldPixelFormat);

				// Set the pixel format to Mono8 if available.
				if (pixelFormat.CanSetValue("Mono8"))
				{
					pixelFormat.SetValue("Mono8");
					cvtype = CV_8UC1;
				}
				// ggf ander Bildtype implementieren





				m_camera.StartGrabbing();

				m_isConnected = true;
				sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Basler]", "Camera connection established");
				//create img Object
				m_img->create(height.GetValue(), width.GetValue(), cvtype);
			}
			catch (Pylon::RuntimeException e)
			{
				sigLogMsg(boost::posix_time::microsec_clock::universal_time(),LUNO_LOG_TYPE_ERROR,"[Basler]",e.what());
			}
		}
		else
		{
			rc = false;
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "Camera connection NOT established!");
		}
	}
	else
	{
		rc = false;
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "There is no camera!");
	}

	return rc;
}

bool LUNOBackend::Basler::disconnect()
{
	int rc = true;

	m_camera.StopGrabbing();

	int err = 0;
	m_img->resize(0); //TODO BHO ????

	//if ((err = is_ExitCamera(m_camHandle)) != IS_SUCCESS && m_isConnected)
	//{
	//	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "Error closing Camera" + std::to_string(err));
	//}

	m_camera.Close();
	m_isConnected = false;

	return rc;
}

void LUNOBackend::Basler::run()
{
	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Basler]", "Starting module main loop");

	int err = 0;
	Pylon::CGrabResultPtr ptrGrabResult;

	while (m_running)
	{
		m_camera.IsCameraDeviceRemoved();

		if (!m_camera.IsCameraDeviceRemoved() && m_isConnected)
		{
			try
			{
				sigCamConnectionState(true);

				m_camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

				// Image grabbed successfully?
				if (ptrGrabResult->GrabSucceeded())
				{
					// Access the image data.
					uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
					//std::cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << std::endl;
				
					if (m_img)
						m_img->data = pImageBuffer;

				}
				else
				{
					//cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
				}


				//if ((err = is_FreezeVideo(m_camHandle, IS_DONT_WAIT)) != IS_SUCCESS)
				//{
				//	if (err != IS_CAPTURE_RUNNING)
				//		getError();
				//}
				//else
				//{
				//	if (m_img)
				//		m_img->data = (uchar*)m_imageMem;

				//}
			}
			catch (const Pylon::GenericException &e)
			{
				sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", e.GetDescription());
				//TODO check warum esception ausgelöst, ggf disconnect + connect
			}

			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		}
		else
		{
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "No connection to sensor trying reconnect");
			sigCamConnectionState(false);
			//NO camera try to reconnect
			disconnect();
			boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
			connect();
		}
	}

	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[Basler]", "Module main loop ended");
}

void LUNOBackend::Basler::getError()
{
	//int lastError = 0;
	//char *lastErrorString;


	//if (is_GetError(m_camHandle, &lastError, &lastErrorString)== IS_SUCCESS)
	//	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "Last Error #" + std::to_string(lastError) + ":" + std::string(lastErrorString));
	//else
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[Basler]", "Something went wrong getting the error message");

}
