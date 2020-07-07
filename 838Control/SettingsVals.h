#pragma once

#include <vector>

namespace LUNOBackend
{
	typedef struct
	{
		std::string key;			// this is the in brackets defined key [key]
		std::string type;			// i need it: STRING, INT, UINT, SHORT, USHORT, DOUBLE, FLOAT, BOOL ("0", or "1")
									// ENUM (WERT1=1, WERT2=2, ...) -> ends up in dropdown menu, stored as UINT
		std::string descr;		    // this is the string to be shown for input in GUI "descr = [1.2345]"
		std::string value;
		std::string unit;
		std::string group;
		std::string toolTip;
		std::string imagePath;
		std::string help;
		std::string defaultVal;
		std::string displayHint;	// some hints for displaying the input mask - may be empty
		std::string minStr;
		std::string maxStr;
		std::string indexStr;

		int arraySize1;
		int arraySize2;

		double min;					// default: -std::limit<double>::max{}
		double max;					// default: std::limit<double>::max{}
		int index1;					// default: -1
		int index2;					// default: -1

		std::vector<std::string> arrayID; // from parse

		int accessLevel; // 0=everyone, 1 = only logged in users, 2 = setup/maintainence, 3 = guru
		bool visible; // true = show in setup, false = hidden
	} type_Vals;
	

	typedef struct
	{
		std::string function;					// result of parse() -> example: min  functions: min, max, array[], array[][], enum 
		std::vector<std::string> arguments;		// result of parse() -> example _var1, +99999.999  (_ is token for that this is a variable or _min or _max)
												// ARGUMENT OF AN ARRAY MAY ALSO A MIN/MAX FUNCTION!!!
												// arguments of enums are the values (combined with enumID

		std::vector<std::string> arrayID;		// result of parse() -> if something like X|Y|Z occures in display hint arrayID.at(0)="X" ....
												// if array is given with WITH the variables will occure in arrayID
												// if array is given with SUCC the arrayID is ignored (obsolete)
		
		std::vector<int> enumID;				// result of parse() -> is calculated while parsing enum

		double result;							// result of evaluate() -> is calculated by evaluating of min, max
		int arraySize1;							// result of evaluate() -> is calculated by evaluating of array[] and array[][]
		int arraySize2;							// result of evaluate() -> is calculated by evaluating of array[][]
	}type_parse;

	class SettingsVals
	{
	public:
		/// <summary>
		/// Default Constructor
		/// </summary>
		SettingsVals();

		/// <summary>
		/// constructor of Settings
		/// </summary>
		/// <param name="fileName">fileName: the complete path to the Settings Definition file</param>
		/// <param name="iniFile">iniFile: the path to the standard initialization path</param>
		SettingsVals(std::string fileName, std::string iniFile);

		SettingsVals &operator=(const SettingsVals &k);

		/// <summary>
		/// Initialise Class
		/// </summary>
		/// <param name="fileName">fileName: the complete path to the Settings Definition file</param>
		/// <param name="iniFile">iniFile: the path to the standard initialization path</param>
		/// <returns>true in success</returns>
		bool init(std::string fileName, std::string iniFile="");

		/// <summary>
		/// read definitions of values from fileName
		/// </summary>
		/// <param name="fileName">fileName: the complete path to the Settings Definition file</param>
		/// <returns>true in success</returns>
		bool readSettings(std::string fileName);

		/// <summary>
		/// read the actual value from file iniFile
		/// if iniFile is empty the m_iniFile is used
		/// </summary>
		/// <param name="parse">
		/// parse 1 = generate all variables without array or with fixed size array
		/// parse 2 = read all
		/// </param>
		/// <param name="iniFile">iniFile: the path to the standard initialization path</param>
		/// <returns>true in success</returns>
		bool readIni(int parse=2, std::string iniFile = "");
		void generateArrays();

		/// <summary>
		/// write all actual values into file iniFile
		/// if iniFile is empty the m_iniFile is used
		/// </summary>
		/// <param name="iniFile">iniFile: the path to the initialization path</param>
		/// <returns>true in success</returns>
		bool writeIni(std::string iniFile = "");


		/// get actual Value by Name and index. if index is <0 no array is used
		bool getValString(std::string key, std::string &val, int index1 = -1, int index2 = -1);
		bool getValInt(std::string key, int &val, int index1 = -1, int index2 = -1);
		bool getValUInt(std::string key, unsigned int &val, int index1 = -1, int index2 = -1);
		bool getValShort(std::string key, short &val, int index1 = -1, int index2 = -1);
		bool getValUShort(std::string key, unsigned short &val, int index1 = -1, int index2 = -1);
		bool getValFloat(std::string key, float &val, int index1 = -1, int index2 = -1);
		bool getValDouble(std::string key, double &val, int index1 = -1, int index2 = -1);
		bool getValBool(std::string key, bool &val, int index1 = -1, int index2 = -1);

		bool getParsedDisplayHint(std::string key, type_parse &retVal);
		bool getParsedEnum(std::string key, type_parse &retVal);

		bool setValString(std::string key, std::string val);

		type_parse parse(std::string in);
		void evaluate(type_parse &in);

		std::vector<std::string> getAllGroups();

		/// collection of all data used
		std::vector<type_Vals> m_vals;


	private:

		std::string m_iniFile;


		/// helpers
		std::string deleteLeadingSpaces(std::string in);
		std::string deleteComment(std::string in);
		std::string deleteAllSpaces(std::string in);
		std::string deleteAllChars(std::string in, std::string characters);
		bool split(std::string in, char seperator, std::string &left, std::string &right);
		std::string replaceDecimalComma(std::string in);
		std::string getIndex(std::string in, int &index1, int &index2);

		bool isConstant(std::string in);
		double evalMinMax(std::string in);

		bool put(type_Vals &val, std::string member, std::string value);
		bool get(type_Vals val, std::string member, std::string &value);
		bool get(type_Vals val, std::string member, double &value);
		bool get(type_Vals val, std::string member, int &value);
		bool get(type_Vals val, std::string member, bool &value);

		bool find(type_Vals &val, std::string key, int index1, int index2, int &position);
		bool find(type_Vals &val, std::string key, int index1 = -1, int index2 = -1);

		bool generateArray(int pos);

		/*
		public:
			/// <summary>
			/// AnlagenType 1: Demo, 2: TLC7020, 3: Messe-System
			/// wird dann endgültig definiert, wenn OPC-Schnittstelle dokumentiert
			/// </summary>
			static unsigned short toolID;

			/// Number of NC-Axis if not readable via OPC
			static unsigned short nc_numAxis;

			/// Encoder 0:Demo, 1:DEVA
			static unsigned short enc_encoderModel;

			/// Encoder Type 0:incremental, 1:absolute
			static unsigned short enc_type;

			/// Number of Enc-Axis
			static unsigned short enc_numAxis;

			/// Conversion factor Encoder Data to SI Units x-inc = 1mm
			static std::vector<double> enc_scaleAxis;

			/// Magical Numbers of DEVA cards (must be read out once when setting up)
			static long enc_linMagicNumber;
			static long enc_rotMagicNumber;

			/// obsolete?
			static double enc_scaleAxisBaseValue;

			/// Calibrationdata of Scanhead (X, Y, Z, A, B, C) vector between intersection of rotating axis and zero point of scanner
			static double OffsetScanhead[6];

			/// Calibrationdata of Scanhead (X, Y, Z, A, B, C) vector between TCP and zero point of scanner
			static double OffsetScanheadTCP2LaserTCP[6];

			/// Offset powder nozzle to surface (Z)
			static float OffsetPowderNozzle;

			/// Offset Drehachse A <-> Maschinen Nullpunkt
			static double OffsetAxisA[6];

			/// Offset Drehachse B <-> Maschinen Nullpunkt
			static double OffsetAxisB[6];

			/// Offset Drehachse C <-> Maschinen Nullpunkt
			static double OffsetAxisC[6];

			/// Axis Position for manual SensorCalibration
			static double CalibManPosition[4][6];

			/// Type of Axis, 0: linear 1: rotation
			static unsigned short *enc_axisType; // was ist das hier?????
			static unsigned short enc_axisTypeBaseValue;

			/// Maping between NC-Axis ID and Encoder Port ID
			struct mapOpcEncData { short IDEnc; short IDopc; };
			static std::vector<mapOpcEncData> m_mapOPCEnc; //Zuordnung Encoder ID zu MTX-Achs ID

													/// Type ID of Linescanner, 0: MicroEpsilon 1:Keyence
			static unsigned short scanner_linescanType;

			static double scanner_frequence;

			static std::string addr_OPC;
			static std::string addr_scanner;

			static unsigned short addr_portOPC;
			static unsigned short addr_portScanner;

			static std::string nc_progPathLocal;
			static std::string nc_progPathRemote;

			static std::string nc_scanFile;///Dateiname für das NC-Scan Programm
			static std::string nc_processFile;///Dateiname für das NC-Schweiß Programm

			static int traori; //temporärflag für TLC

			static std::string externalProg;

			///Max Vorschub Achsen
			static double nc_axisMaxVel;

			//-------------Mesh-----
			static int meshAlg;
			static int meshquality;
			static int scanQuality;

			static double filterReducePointsParam1[3];
			static double filterOutlierParam1[3];
			static int filterOutlierParam2[3];
			static double filterNormalParam1[3];
			static double filterReduceMwshParam[3];

			//table
			static double tbl_xmin;
			static double tbl_xmax;
			static double tbl_ymin;
			static double tbl_ymax;
			static double tbl_zmin;
			static double tbl_zmax;
			static double tbl_thickness;

			static int tbl_camdist;

			static unsigned short ui_MenuFrameRate;

			static unsigned short userlevel;

			static std::string mwToolXMLFile;//XML File mit Maschinenkonfiguration

			static std::string rci_path;//Pfad zum Trump-Komunikationsorder
			static std::string rci_machineID;//Maschinenbezeichnung
			static std::string rci_localPath;//Pfad auf der TLC zur NC-Datei
			static std::string UserPath;
		};
		*/
	};
}