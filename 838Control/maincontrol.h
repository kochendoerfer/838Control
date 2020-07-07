#pragma once
#include "logging.h"
#include "SettingsVals.h"
#include "Basler.h"
#include "OPCUA_Client.h"

#include "AcqJoinedImg.h"
#include "AxisTransformation.h"

namespace LUNOBackend
{
	class maincontrol
	{
	public:
		maincontrol();
		maincontrol(std::string settingsFilename);
		~maincontrol();

		/// <summary>
		/// Initialise the backend
		/// </summary>
		/// <returns> true on success</returns>
		bool init();

		/// <summary>
		/// Ense
		/// </summary>
		bool stop();

		/// <summary>
		/// busyloop
		/// </summary>
		/// <returns></returns>
		void run();

		/// <summary>
		/// internal logging object
		/// </summary>
		LUNOBackend::logging *m_log;

		/// <summary>
		/// the Basler camerasensorobject
		/// </summary>
		LUNOBackend::Basler *m_baslerSensor;

		LUNOBackend::toolControlMTX *m_opcclient;

		LUNOBackend::SettingsVals m_settings;

		//interne Signale
		boost::signals2::signal<void(std::string)> sigInitStep;
		boost::signals2::signal<void(boost::posix_time::ptime, unsigned int, std::string, std::string)> sigLogMsg;

	private:
		std::string m_settingsFilename;
		AcqJoinedImg m_procRasterImage;
		LUNOBackend::AxisTransformation m_axisTransformation;

		bool m_running = true;

		void resetMarker();
		void calcTransformation();
		void detectMarker(double v1, double v2);

		void resetRaster();
		void FinalizeRasterImage();
		void addRasterImage(double v1, double v2);

		void cameraState(bool state);

	};
}
