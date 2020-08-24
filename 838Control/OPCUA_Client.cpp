#include "OPCUA_Client.h"
#include "lunodefs.h"
//#include <open62541/client_config_default.h>
//#include <open62541/client_highlevel.h>

namespace LUNOBackend
{
	toolControlMTX::toolControlMTX()
	{
		m_numAxis = 0;
		//m_MachineData = std::shared_ptr<LUNOBackend::MachineData>(new LUNOBackend::MachineData);

		m_clientReadHP = NULL;
		m_clientWrite = NULL;

		m_MTXPrefix = "MTX_SPS";

		UA_ClientConfig config = UA_ClientConfig_default;
		config.timeout = 5000;

		m_clientReadHP = UA_Client_new(config);
		m_clientWrite = UA_Client_new(config);

		//UA_ClientConfig *configHP = new UA_ClientConfig();
		//configHP = UA_Client_getConfig(m_clientReadHP);
		//configHP->timeout = 5000;

		//UA_ClientConfig_setDefault(UA_Client_getConfig(m_clientReadHP));
		//UA_ClientConfig_setDefault(configHP);
		//UA_ClientConfig_setDefault(UA_Client_getConfig(m_clientWrite));

		m_threadHP = NULL;

		m_running = false;

		//Request für Achsendaten anlegen
		UA_ReadRequest_init(&m_reqAxisData);
		static const int numReadVarsAxis = 4;

		int numVar = 0;
		m_reqAxisData.nodesToRead = (UA_ReadValueId*)UA_Array_new(numReadVarsAxis, &UA_TYPES[UA_TYPES_READVALUEID]);
		m_reqAxisData.nodesToReadSize = numReadVarsAxis;

		m_reqAxisData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqAxisData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(27, "NC.Chan.AxisPosAcs,01,1");
		numVar++;

		m_reqAxisData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqAxisData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(27, "NC.Chan.AxisPosAcs,01,2");
		numVar++;

		m_reqAxisData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqAxisData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(27, "NC.Chan.AxisPosAcs,01,3");
		numVar++;

		m_reqAxisData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqAxisData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(27, "NC.Chan.AxisPosAcs,01,4");
		numVar++;


		/*
		bool AcqImage; ///Bildaufnahme
		double PosX; ///Vorgabe X Position SLM
		double PosY; ///Vorgabe Y Position SLM
		bool ProcessMarkerStart; ///BV Markererkennung Start
		bool ProcessMarkerEnd; ///BV Markererkennung Ende alle Marken angefahren
		bool ProcesRasterStart;
		bool ProcesRasterEnd;

		*/
		//Request für Maschinendaten anlegen
		UA_ReadRequest_init(&m_reqMachineData);
		static const int numReadVarsData = 9;

		numVar = 0;
		m_reqMachineData.nodesToRead = (UA_ReadValueId*)UA_Array_new(numReadVarsData, &UA_TYPES[UA_TYPES_READVALUEID]);
		m_reqMachineData.nodesToReadSize = numReadVarsData;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerStart").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerImage").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerEnd").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterStart").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterImage").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterEnd").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_MarkerPosX").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_MarkerPosY").c_str());
		numVar++;

		m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING_ALLOC(2, (m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_GUIClose").c_str());
		numVar++;


		//m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		//m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING(27, "NC.Chan.ProgramPosition,01,FilePosition");
		//numVar++;

		//m_reqMachineData.nodesToRead[numVar].attributeId = UA_ATTRIBUTEID_VALUE;
		//m_reqMachineData.nodesToRead[numVar].nodeId = UA_NODEID_STRING(2, "");
		//numVar++;
	}


	toolControlMTX::~toolControlMTX()
	{
		if (m_clientReadHP) UA_Client_delete(m_clientReadHP);
		if (m_clientWrite) UA_Client_delete(m_clientWrite);
	}

	void toolControlMTX::init(LUNOBackend::SettingsVals *settings)
	{
		m_settings = settings;
	}

	LUNOBackend::AxisPosData *toolControlMTX::getAxisDataPointer()
	{
		return m_AxisData;
	}

	void toolControlMTX::start()
	{
		connect();

		m_running = true;
		m_threadHP = new boost::thread(boost::bind(&toolControlMTX::runHP, this));
	}

	void toolControlMTX::stop()
	{
		//m_running = false;

		//if (m_threadHP)	m_threadHP->join();



		bool rc = true;

		if (m_running)
		{
			m_running = false;
			try
			{
				m_threadHP->join();
			}
			catch(boost::thread_exception e)
			{
				std::string sss = e.what();
			}
			disconnect();
		}

		//return rc;

	}

	void toolControlMTX::connect()
	{
		UA_StatusCode retvalReadHP;
		UA_StatusCode retvalWrite;

		std::stringstream Server_URL;
		std::string m_ServerName;
		m_settings->getValString("MTX_IP", m_ServerName); //"192.168.1.65";
		int m_Port;// = 4840;
		m_settings->getValInt("OPCPort", m_Port);

		Server_URL << "opc.tcp://" << m_ServerName << ":" << std::to_string(m_Port);

		//OPC Verbindung aufbauen
		/*if (!m_ClientReadConnected) */
		retvalReadHP = UA_Client_connect(m_clientReadHP, Server_URL.str().c_str());
		retvalWrite = UA_Client_connect(m_clientWrite, Server_URL.str().c_str());

		if (retvalReadHP == UA_STATUSCODE_GOOD)
		{
			//get Num Axis
			UA_StatusCode retval;
			//UA_String value;
			UA_Variant *val = UA_Variant_new();
			int retVal = 0;

			retval = UA_Client_readValueAttribute(m_clientReadHP, UA_NODEID_STRING_ALLOC(27, "NC.Chan.AxisPosWcs,01,2"), val);

			retVal = val->arrayLength;
			UA_Variant_delete(val);

			m_numAxis = retVal;

			m_AxisData = new LUNOBackend::AxisPosData(m_numAxis);

			extractAxisData();
		}
		else
		{
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_FATAL_ERROR, "[MTX]", "MTX Connection failed");
		}
	}

	void toolControlMTX::disconnect()
	{
		UA_Client_disconnect(m_clientReadHP);
		UA_Client_disconnect(m_clientWrite);
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_MESSAGE, "[MTX]", "MTX Disconnected");
	}

	bool toolControlMTX::isConnected()
	{
		return m_running;
	}

	void toolControlMTX::statusMarkerDetection(int i)
	{
		int rc = -1;
		std::string itemname = m_MTXPrefix;
		if (i>0)
			itemname += ".Globale_Variablen.g_tOPC.gIn_bProcessingError";
		else
			itemname += ".Globale_Variablen.g_tOPC.gIn_bProcessingOK";

		rc = writeBool(2, itemname, true);
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Error finding Marker:");
	}

	void toolControlMTX::setTransformation(cv::Point2f translation, cv::Point2f rotation, cv::Point2f scale)
	{
		bool rc = false;
		rc = (writeFloat(2, m_MTXPrefix+ ".Globale_Variablen.g_tOPC.gIn_rSetTransX", translation.x)==0?true:false);
		rc = (writeFloat(2, m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_rSetTransY", translation.y) == 0 ? true : false);

		rc = (writeFloat(2, m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_rSetRot", rotation.x) == 0 ? true : false);

		rc = (writeFloat(2, m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_rSetScaleX", scale.x) == 0 ? true : false);
		rc = (writeFloat(2, m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_rSetScaleY", scale.y) == 0 ? true : false);

		rc = (writeBool(2, m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_bProcessMarkerDone", true) == 0 ? true : false);
	}

	void toolControlMTX::setCameraState(bool state)
	{
		std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_bKameraConnectionError";

		writeBool(2, itemname, state);
	}

	void toolControlMTX::runHP()
	{
		bool ProcessMarkerStartLast = false;
		bool ProcessMarkerImageLast = false;
		bool ProcessMarkerEndLast = false;

		bool ProcessRasterStartLast = false;
		bool ProcessRasterImageLast = false;
		bool ProcesRasterEndLast = false;

		bool CloseGUILast = false;

		try {
			while (m_running)
			{
				if (getConnectionState())
				{
					extractAxisData();
					extractMachineData();

					double MarkerPosX = 42.0;
					double MarkerPosY = 4.20;

					if (m_AxisData)
					{
						for (auto i = 0; i < m_AxisData->m_AxisData.size(); i++)
						{
							//if (m_AxisData->m_AxisData[i].getName().compare("X"))
							//	MarkerPosX = m_AxisData->m_AxisData[i].getValue();
							//
							//if (m_AxisData->m_AxisData[i].getName().compare("Y"))
							//	MarkerPosY = m_AxisData->m_AxisData[i].getValue();
							MarkerPosX = m_AxisData->m_AxisData[0].getValue();
							MarkerPosY = m_AxisData->m_AxisData[1].getValue();
						}
					}

					//Handle Processing Start Requests
					if (m_machineData.ProcessMarkerStart && !ProcessMarkerStartLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarker Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerStart";
						writeBool(2, itemname, false);

						sigFindMarkerStartRequest();
					}

					ProcessMarkerStartLast = m_machineData.ProcessMarkerStart;

					//Handle Processing Requests
					if (m_machineData.ProcessMarkerImage && !ProcessMarkerImageLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarker Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerImage";
						writeBool(2, itemname, false);

						sigFindMarkerRequest(MarkerPosX, MarkerPosY);
					}

					ProcessMarkerImageLast = m_machineData.ProcessMarkerImage;

					//alle Marken wurden angefahren
					if (m_machineData.ProcessMarkerEnd && !ProcessMarkerEndLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarkerEnd Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerEnd";
						writeBool(2, itemname, false);

						sigFindMarkerEndRequest();
					}
					ProcessMarkerEndLast = m_machineData.ProcessMarkerEnd;

					//Rasterbild Reset
					if (m_machineData.ProcesRasterStart && !ProcessRasterStartLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Clear Raster Image Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterStart";
						writeBool(2, itemname, false);

						sigRasterStartRequest();
					}
					ProcessRasterStartLast = m_machineData.ProcesRasterStart;

					//Handle Processing Requests
					if (m_machineData.ProcesRasterImage && !ProcessRasterImageLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Raster Image Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterImage";
						int rc = writeBool(2, itemname, false);
						itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_bProcessingOK";
						rc = writeBool(2, itemname, true);
						sigRasterRequest(MarkerPosX, MarkerPosY);
					}
					ProcessRasterImageLast = m_machineData.ProcesRasterImage;

					//Bauraum abgeraster->Bild verarbeiten + speichern Request
					if (m_machineData.ProcesRasterEnd && !ProcesRasterEndLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Process Raster ImageEnd Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterEnd";
						writeBool(2, itemname, false);
						itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_bProcessingOK";
						writeBool(2, itemname, false);

						sigRasterEndRequest();
					}
					ProcesRasterEndLast = m_machineData.ProcesRasterEnd;

					if (m_machineData.CloseGUI && !CloseGUILast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Close GUI Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_GUIClose";
						writeBool(2, itemname, false);

						sigCloseGUIRequest();
					}
					CloseGUILast = m_machineData.CloseGUI;
				}
				else
				{
					//no connectoin
					sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[MTX]", "No connection to tool trying reconnect");
					//NO MTX try to reconnect
					disconnect();
					boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
					connect();
				}

				boost::this_thread::sleep_for(boost::chrono::microseconds(THREAD_HP * 100));
			}
		}
		catch (boost::thread_interrupted &)
		{
			m_running = false;
		}

		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Module main loop ended");
	}

	/*void toolControlMTX::runHP()
	{
		bool ProcessMarkerStartLast = false;
		bool ProcessMarkerImageLast = false;
		bool ProcessMarkerEndLast = false;

		bool ProcessRasterStartLast = false;
		bool ProcessRasterImageLast = false;
		bool ProcesRasterEndLast = false;

		bool CloseGUILast = false;
		
		try {
			while (m_running)
			{
				if (getConnectionState())
				{
					extractAxisData();
					extractMachineData();

					double MarkerPosX = 42.0;
					double MarkerPosY = 4.20;

					if (m_AxisData)
					{
						for (auto i = 0; i < m_AxisData->m_AxisData.size(); i++)
						{
							if (m_AxisData->m_AxisData[i].getName().compare("X"))
								MarkerPosX = m_AxisData->m_AxisData[i].getValue();

							if (m_AxisData->m_AxisData[i].getName().compare("Y"))
								MarkerPosY = m_AxisData->m_AxisData[i].getValue();
						}
					}
					
					//Handle Processing Start Requests
					if (m_machineData.ProcessMarkerStart && !ProcessMarkerStartLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarker Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerStart";
						writeBool(2, itemname, false);

						sigFindMarkerStartRequest();
					}

					ProcessMarkerStartLast = m_machineData.ProcessMarkerStart;

					//Handle Processing Requests
					if (m_machineData.ProcessMarkerImage && !ProcessMarkerImageLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarker Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerImage";
						writeBool(2, itemname, false);

						sigFindMarkerRequest(MarkerPosX, MarkerPosY);
					}

					ProcessMarkerImageLast = m_machineData.ProcessMarkerImage;

					//alle Marken wurden angefahren
					if (m_machineData.ProcessMarkerEnd && !ProcessMarkerEndLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "FindMarkerEnd Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessMarkerEnd";
						writeBool(2, itemname, false);

						sigFindMarkerEndRequest();
					}
					ProcessMarkerEndLast = m_machineData.ProcessMarkerEnd;

					//Rasterbild Reset
					if (m_machineData.ProcesRasterStart && !ProcessRasterStartLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Clear Raster Image Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterStart";
						writeBool(2, itemname, false);

						sigRasterStartRequest();
					}
					ProcessRasterStartLast = m_machineData.ProcesRasterStart;

					//Handle Processing Requests
					if (m_machineData.ProcesRasterImage && !ProcessRasterImageLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Raster Image Request");
						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterImage";
						writeBool(2, itemname, false);

						sigRasterRequest(MarkerPosX, MarkerPosY);
					}
					ProcessRasterImageLast = m_machineData.ProcesRasterImage;

					//Bauraum abgeraster->Bild verarbeiten + speichern Request
					if (m_machineData.ProcesRasterEnd && !ProcesRasterEndLast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Process Raster ImageEnd Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_bProcessRasterEnd";
						writeBool(2, itemname, false);
						itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gIn_bProcessingOK";
						writeBool(2, itemname, false);

						sigRasterEndRequest();
					}
					ProcesRasterEndLast = m_machineData.ProcesRasterEnd;

					if (m_machineData.CloseGUI && !CloseGUILast)
					{
						sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Close GUI Request");

						std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.gOut_GUIClose";
						writeBool(2, itemname, false);

						sigCloseGUIRequest();
					}
					CloseGUILast = m_machineData.CloseGUI;
				}
				else
				{
					//no connectoin
					sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[MTX]", "No connection to tool trying reconnect");
					//NO MTX try to reconnect
					disconnect();
					boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
					connect();
				}

				boost::this_thread::sleep_for(boost::chrono::microseconds(THREAD_HP * 100));
			}
		}
		catch (boost::thread_interrupted &)
		{
			m_running = false;
		}

		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Module main loop ended");
	}*/

	//Helper
	int toolControlMTX::writeBool(int nodeID, std::string itemname, bool val)
	{
		int rc = -1;

		boost::mutex::scoped_lock lock(m_mtxWrite);
		{
			UA_WriteRequest wReq;
			UA_WriteResponse wResp;
			UA_WriteRequest_init(&wReq);
			wReq.nodesToWrite = UA_WriteValue_new();
			wReq.nodesToWriteSize = 1;

			wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
			wReq.nodesToWrite[0].value.hasValue = true;
			wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the integer on deletion */
			wReq.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_BOOLEAN];
			wReq.nodesToWrite[0].value.value.data = (void *)&val;
			wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nodeID, (const char *)(itemname.c_str()));

			wResp = UA_Client_Service_write(m_clientWrite, wReq);

			if (wResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			{
				rc = 0;
			}
			else
			{
				rc = wResp.responseHeader.serviceResult;
			}

			UA_WriteRequest_deleteMembers(&wReq);
			UA_WriteResponse_deleteMembers(&wResp);
		}

		return rc;
	}

	int toolControlMTX::writeString(int nodeID, std::string itemname, std::string val)
	{
		int rc = -1;

		boost::mutex::scoped_lock lock(m_mtxWrite);
		{
			UA_String a;
			a = UA_String_fromChars(val.c_str());
			//		wReq.nodesToWrite[0].value.value.data = (void *)&a;


			UA_WriteRequest wReq;
			UA_WriteResponse wResp;
			UA_WriteRequest_init(&wReq);
			wReq.nodesToWrite = UA_WriteValue_new();
			wReq.nodesToWriteSize = 1;

			wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
			wReq.nodesToWrite[0].value.hasValue = true;
			wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the integer on deletion */
			wReq.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_STRING];
			wReq.nodesToWrite[0].value.value.data = (void *)&a;
			wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nodeID, (const char *)(itemname.c_str()));

			wResp = UA_Client_Service_write(m_clientWrite, wReq);

			if (wResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			{
				rc = 0;
			}

			UA_WriteRequest_deleteMembers(&wReq);
			UA_WriteResponse_deleteMembers(&wResp);
		}

		return rc;
	}

	int toolControlMTX::writeFloat(int nodeID, std::string itemname, float val)
	{
		int rc = -1;

		float valf = (float)val;

		boost::mutex::scoped_lock  lock(m_mtxWrite);
		{
			UA_WriteRequest wReq;
			UA_WriteResponse wResp;
			UA_WriteRequest_init(&wReq);
			wReq.nodesToWrite = UA_WriteValue_new();
			wReq.nodesToWriteSize = 1;

			wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
			wReq.nodesToWrite[0].value.hasValue = true;
			wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the integer on deletion */
			wReq.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_FLOAT];
			wReq.nodesToWrite[0].value.value.data = (void *)&valf;
			wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nodeID, (const char *)(itemname.c_str()));

			wResp = UA_Client_Service_write(m_clientWrite, wReq);

			if (wResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			{
				rc = 0;
			}

			UA_WriteRequest_deleteMembers(&wReq);
			UA_WriteResponse_deleteMembers(&wResp);
		}

		return rc;
	}

	int toolControlMTX::writeDouble(int nodeID, std::string itemname, double val)
	{
		int rc = -1;

		float valf = (float)val;

		boost::mutex::scoped_lock  lock(m_mtxWrite);
		{
			UA_WriteRequest wReq;
			UA_WriteResponse wResp;
			UA_WriteRequest_init(&wReq);
			wReq.nodesToWrite = UA_WriteValue_new();
			wReq.nodesToWriteSize = 1;

			wReq.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
			wReq.nodesToWrite[0].value.hasValue = true;
			wReq.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE; /* do not free the integer on deletion */
			wReq.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_DOUBLE];
			wReq.nodesToWrite[0].value.value.data = (void *)&valf;
			wReq.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(nodeID, (const char *)(itemname.c_str()));

			wResp = UA_Client_Service_write(m_clientWrite, wReq);

			if (wResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
			{
				rc = 0;
			}

			UA_WriteRequest_deleteMembers(&wReq);
			UA_WriteResponse_deleteMembers(&wResp);
		}

		return rc;
	}

	int toolControlMTX::extractAxisData()
	{
		int rc = -1;

		if (/*m_ClientReadConnected*/true)
		{
			boost::mutex::scoped_lock lock(m_mtxRead);
			{
				UA_ReadResponse resp;

				resp = UA_Client_Service_read(m_clientReadHP, m_reqAxisData);

				if (resp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
				{
					m_numAxis = resp.results[0].value.arrayLength;//Wenn das nicht funktioniert liegte es hier am resize und der neuen Arraygröße!!!

//					m_AxisData->resize(m_numAxis);

					for (int i = 0; i < m_numAxis; i++)
					{
						//if (resp.results[0].value.type != 0)
						//{
						//	//(m_axisdata.get()[i]).axisposition = ((ua_double*)resp.results[0].value.data)[i];
						//	m_axisposdata.setvalue(i, ((ua_double*)resp.results[0].value.data)[i]);
						//}

						std::string name;
						double value = 0.0;
						if (resp.results[1].value.type != 0)
						{
							std::string n(reinterpret_cast<char const*>((((UA_String*)resp.results[1].value.data)[i]).data), (((UA_String*)resp.results[1].value.data)[i]).length);
							name = n;
						}

						if (resp.results[2].value.type != 0)
						{
							value = ((UA_Double*)resp.results[0].value.data)[i];
						}

						if (resp.results[3].value.type != 0)
						{
							int unit = ((UA_UInt32*)resp.results[3].value.data)[i];
							LUNOBackend::AXIS_TYPE at = unit == 1 ? LUNOBackend::AT_NORM : (unit == 2 ? LUNOBackend::AT_ROT : LUNOBackend::AT_UNKOWN);

							//Error case
							if (at == LUNOBackend::AT_UNKOWN)
								return -1;//Meh

							if (m_AxisData)
								(*m_AxisData)[i].setData(name, value, unit, at);
						}


					}

					rc = 0;
				}
				else
				{
					rc = -1;
				}

				UA_ReadResponse_deleteMembers(&resp);
			}

			//if (m_AxisData && m_AxisData->size()>0)
			//	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_MESSAGE, "[MTX]", "AxisPos:" + (*m_AxisData)[0].getName() + " " + std::to_string((*m_AxisData)[0].getValue()));
		}

		return rc;
	}

	int toolControlMTX::extractMachineData()
	{
		int rc = -1;

		if (/*m_ClientReadConnected*/true)
		{
			boost::mutex::scoped_lock lock(m_mtxRead);
			{
				UA_ReadResponse resp;

				resp = UA_Client_Service_read(m_clientReadHP, m_reqMachineData);

				if (resp.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
				{
					if (resp.results != 0)
					{
						if (resp.results[0].value.type != 0) m_machineData.ProcessMarkerStart = (bool)*(UA_Boolean*)resp.results[0].value.data;
						if (resp.results[1].value.type != 0) m_machineData.ProcessMarkerImage = (bool)*(UA_Boolean*)resp.results[1].value.data;
						if (resp.results[2].value.type != 0) m_machineData.ProcessMarkerEnd = (bool)*(UA_Boolean*)resp.results[2].value.data;
						if (resp.results[3].value.type != 0) m_machineData.ProcesRasterStart = (bool)*(UA_Boolean*)resp.results[3].value.data;
						if (resp.results[4].value.type != 0) m_machineData.ProcesRasterImage = (bool)*(UA_Boolean*)resp.results[4].value.data;
						if (resp.results[5].value.type != 0) m_machineData.ProcesRasterEnd = (bool)*(UA_Boolean*)resp.results[5].value.data;
						if (resp.results[6].value.type != 0) m_machineData.PosX = (double)*(UA_Float*)resp.results[6].value.data;
						if (resp.results[7].value.type != 0) m_machineData.PosY = (double)*(UA_Float*)resp.results[7].value.data;
						if (resp.results[8].value.type != 0) m_machineData.CloseGUI = (bool)*(UA_Boolean*)resp.results[8].value.data;

						rc = 0;
					}
				}
				else
				{
					rc = -1;
				}

				UA_ReadResponse_deleteMembers(&resp);
			}

		}

		return rc;
	}

	void toolControlMTX::keepAlive()
	{
		int rc = -1;

		std::string itemname = m_MTXPrefix + ".Globale_Variablen.g_tOPC.bExternBV";

		rc = writeBool(2, itemname, true);
		if (rc != 0)
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[MTX]", "Keep Alive RC:" + std::to_string(rc));
	}

	bool toolControlMTX::getConnectionState()
	{
		bool rc = false;

		UA_ClientState state = UA_Client_getState(m_clientReadHP);

		if (state == UA_CLIENTSTATE_SESSION)
		{
			rc = true;
			keepAlive();
		}
		else
		{
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_ERROR, "[MTX]", "MTX Connection State:" + std::to_string(state));
		}

		return rc;
	}
}

