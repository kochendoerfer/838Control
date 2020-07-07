#pragma once

#include <string>
#include <vector>

namespace LUNOBackend
{
	enum AXIS_TYPE
	{
		AT_NORM = 0,
		AT_ROT,
		AT_UNKOWN,
	};

	class AxisPosData
	{
	public:
		class AxisDataPoint
		{//Datenkapselung
		public:
			AxisDataPoint() : axisName("NA"), unit(-1), value(1234), at(AT_NORM) {}//Default konstruktor

			//GETTER
			std::string getName() { return axisName; }
			double getValue() { return value; }
			uint32_t getUnit() { return unit; }
			AXIS_TYPE getAxisType() { return at; }

			//SETTER
			void setName(std::string name) { axisName = name; }
			void setValue(double val) { value = val; }
			void setUnit(uint32_t u) { unit = u; }
			void setAxisType(AXIS_TYPE axistype) { at = axistype; }
			void setData(std::string name, double val, uint32_t u, AXIS_TYPE axistype)
			{
				axisName = name;
				value = val;
				unit = u;
				at = axistype;
			}

			double value;

		private:
			std::string axisName;
			uint32_t unit;
			AXIS_TYPE at;
		};

		//construct by size, default is 0
		AxisPosData(size_t size = 0) : m_AxisData(size) {}

		//construct by copy, copies array
		AxisPosData(const AxisPosData &tocopy) : m_AxisData(tocopy.m_AxisData) {}

		~AxisPosData() {}

		//void init(int numAxis);
		//void setName(int ID, std::string name);
		//void setUnit(int ID, int type);
		//void setValue(int ID, double value);
		//void setNumber(int ID, uint32_t number);
		//void setGridPos(int ID, int row, int col);

		//std::string getName(int ID);
		//int getUnit(int ID);
		//int getNumAxis();
		//void getGridPos(int ID, int &row, int &col);
		//double getValue(int ID);
		//double getValue(std::string name, bool ignorecase = true);

		//void removeAxisPoint(int ID);
		//void removeAxisPoint(std::string name);
		void resize(size_t size) { m_AxisData.resize(size); }
		size_t size() { return m_AxisData.size(); }
		void removeAxisPoint(int i) { m_AxisData.erase(m_AxisData.begin() + i); }

		size_t numAxis() { return m_AxisData.size(); }
		size_t numNorm() { return numAxisType(AT_NORM); }
		size_t numRot() { return numAxisType(AT_ROT); }


		AxisDataPoint& operator[](size_t i)
		{
			return m_AxisData[i];
		}

		//AxisDataPoint& operator()(size_t i, AXIS_TYPE at);

		//int m_numAxis;
		std::vector<AxisDataPoint> m_AxisData;

	private:
		//helper function
		size_t numAxisType(AXIS_TYPE at);
	};
}
