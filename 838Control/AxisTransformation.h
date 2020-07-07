#pragma once
#include <opencv2/core/core.hpp>

#include "LUNOdefs.h"
#include "SettingsVals.h"

namespace LUNOBackend
{
	struct DataPoints
	{
		cv::Point2d refpoint;
		cv::Point2d actpoint;
	};

	class AxisTransformation
	{
	public:
		AxisTransformation();
		~AxisTransformation();

		void init(LUNOBackend::SettingsVals *settings);
		
		/// <summary>
		/// Remove all Positions
		/// </summary>
		void clear();
		void addPoint(cv::Point2d axispnt, cv::Point2d markerpnt);
		bool process(cv::Point2d *translation,cv::Point2d *rotation,cv::Point2d *scale);

	private:
		LUNOBackend::SettingsVals *m_settings;

		std::vector<DataPoints>  m_refPoints;
	};
}
