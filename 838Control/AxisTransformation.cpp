#include "AxisTransformation.h"



LUNOBackend::AxisTransformation::AxisTransformation()
{
}


LUNOBackend::AxisTransformation::~AxisTransformation()
{
}

void LUNOBackend::AxisTransformation::init(LUNOBackend::SettingsVals *settings)
{
	m_settings = settings;
}

void LUNOBackend::AxisTransformation::clear()
{
	m_refPoints.clear();
}

void LUNOBackend::AxisTransformation::addPoint(cv::Point2d axispnt, cv::Point2d markerpnt)
{
	DataPoints points;
	points.refpoint = axispnt;
	points.actpoint = markerpnt;

	m_refPoints.push_back(points);
}

bool LUNOBackend::AxisTransformation::process(cv::Point2d *translation, cv::Point2d *rotation, cv::Point2d *scale)
{
	bool rc = true;
	translation->x = 42;
	translation->y = 42;

	rotation->x = 4.2;
	rotation->y = 4.2;

	scale->x = 0.42;
	scale->y = 0.42;

	return rc;
}
