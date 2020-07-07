#pragma once
#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <opencv2/core.hpp>
#include <pylon/PylonIncludes.h>

#include "SettingsVals.h"

namespace LUNOBackend
{
	class Basler
	{
	public:
		Basler();
		~Basler();


		bool init(std::string path, std::string filename); //Einstellungen z.B. IP Adresse,...
		void init(LUNOBackend::SettingsVals *settings); //Einstellungen z.B. IP Adresse,...
		bool start(); //Starten der com loop
		bool stop(); //Stoppen der com loop

		/// <summary>
		/// set exposure time [ms]
		/// camera may change the exposure time, so after setting exposure time read out exposure time tho verify settings
		/// </summary>
		/// <param name="exptime">expusure time [ms]</param>
		/// <returns>true on suscess</returns>
		bool setExposureTime(double exptime);

		/// <summary>
		/// set framerate [Hz]
		/// camera may change the framerate, so after setting framerate read out framerate to verify settings
		/// </summary>
		/// <param name="framerate">framerate [Hz]</param>
		/// <returns>true on suscess</returns>
		bool setFramerate(double framerate);

		/// <summary>
		/// set camera ROI (not implemented yet)
		/// </summary>
		/// <param name="X1">ROI Pos1 X</param>
		/// <param name="Y1">ROI Pos1 Y</param>
		/// <param name="X2">ROI Pos2 X</param>
		/// <param name="Y2">ROI Pos2 Y</param>
		/// <returns>true on success</returns>
		bool setROI(unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2);

		/// <summary>
		/// returns the connection State of the camera false: no camera connected
		/// </summary>
		bool getConnectionState() { return m_isConnected; };

		/// <summary>
		/// get current framerate
		/// </summary>
		/// <returns>framerate [Hz]</returns>
		double getFramerate();

		/// <summary>
		/// get current exposure time (not implemented yet)
		/// </summary>
		/// <returns>exposure time [ms]</returns>
		double getExposureTime();

		/// <summary>
		/// det image
		/// </summary>
		/// <returns>pointer to last image</returns>
		cv::Mat* getValue();

		cv::Mat getLiveImage();

		/// <summary>
		/// saves current Image to PNG
		/// </summary>
		void exportdata();

		/// <summary>
		/// signal um Klassenfehler zu loggen
		/// z.B. Connectin Lost, Aufreuf einer Methode,...
		/// </summary>
		/// <param name="">Zeitstempel</param>
		/// <param name="">Message Klasse (definiert in LUNOdefs.h</param>
		/// <param name="">Name des Module/Klasse</param>
		/// <param name="">Meldungstext</param>
		boost::signals2::signal<void(boost::posix_time::ptime, unsigned int, std::string, std::string)> sigLogMsg;
		boost::signals2::signal<void(bool)> sigCamConnectionState;

	private:
		bool connect();
		bool disconnect();
		void run();
		void getError();

		LUNOBackend::SettingsVals *m_settings = NULL;

		boost::thread *m_threadStatus = NULL; //Thread Object

		bool m_running = false; //busy loop control Var

		Pylon::CInstantCamera m_camera;
		GenApi::INodeMap *m_nodemap;

		bool m_isConnected = false;

		INT m_memId;
		char *m_imageMem;

		unsigned int m_ROIX = 0;
		unsigned int m_ROIY = 0;
		unsigned int m_ROIW = 0;
		unsigned int m_ROIH = 0;

		cv::Mat *m_img;

		std::string m_path;
		std::string m_filename;

	};
}
