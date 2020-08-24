#pragma once
#include "open62541.h"
//#include "open62541/"
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include "axisPosData.h"
#include "SettingsVals.h"
#include <opencv2/core/core.hpp>


namespace LUNOBackend
{
	struct MachineData
	{
		double PosX; ///Vorgabe X Position SLM
		double PosY; ///Vorgabe Y Position SLM
		bool ProcessMarkerStart; ///BV Markererkennung Start
		bool ProcessMarkerImage; ///BV Markererkennung neues Bild
		bool ProcessMarkerEnd; ///BV Markererkennung Ende alle Marken angefahren
		bool ProcesRasterStart;
		bool ProcesRasterImage; ///Bildaufnahme
		bool ProcesRasterEnd;
		bool CloseGUI;
	};

	class toolControlMTX
	{
	public:
		toolControlMTX();
		~toolControlMTX();

		void init(LUNOBackend::SettingsVals *settings);

		LUNOBackend::AxisPosData *getAxisDataPointer();

		void start();
		void stop();

		void connect();

		void disconnect();

		bool isConnected();

		int getAxisCount() { return m_numAxis; };

		void statusMarkerDetection(int i);
		void setTransformation(cv::Point2f translation, cv::Point2f rotation, cv::Point2f scale);
		void setCameraState(bool state);

	/// <summary>
	/// signal um Klassenfehler zu loggen
	/// z.B. Connectin Lost, Aufreuf einer Methode,...
	/// </summary>
	/// <param name="">Zeitstempel</param>
	/// <param name="">Message Klasse (definiert in LUNOdefs.h</param>
	/// <param name="">Name des Module/Klasse</param>
	/// <param name="">Meldungstext</param>
		boost::signals2::signal<void(boost::posix_time::ptime, unsigned int, std::string, std::string)> sigLogMsg;

		boost::signals2::signal<void()> sigFindMarkerStartRequest;
		boost::signals2::signal<void()> sigRasterStartRequest;

		boost::signals2::signal<void(double, double)> sigFindMarkerRequest;
		boost::signals2::signal<void(double, double)> sigRasterRequest;

		boost::signals2::signal<void()> sigFindMarkerEndRequest;
		boost::signals2::signal<void()> sigRasterEndRequest;

		boost::signals2::signal<void()> sigCloseGUIRequest;

	private:
		void runHP();

		int writeBool(int nodeID, std::string itemname, bool val);

		int writeString(int nodeID, std::string itemname, std::string val);

		int writeFloat(int nodeID, std::string itemname, float val);

		int writeDouble(int nodeID, std::string itemname, double val);

		int extractAxisData();

		int extractMachineData();

		void keepAlive();

		bool getConnectionState();

		bool m_running;

		unsigned short m_numAxis;

		LUNOBackend::SettingsVals *m_settings = NULL;

		UA_Client *m_clientReadHP; //High Priority Reading
		UA_Client *m_clientWrite;

		UA_ReadRequest m_reqAxisData;
		UA_ReadRequest m_reqMachineData;

		boost::thread *m_threadHP;

		boost::mutex m_mtxRead;
		boost::mutex m_mtxWrite;

		LUNOBackend::AxisPosData *m_AxisData=NULL;

		MachineData m_machineData;

		std::string m_MTXPrefix;
	};
}
