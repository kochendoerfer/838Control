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
	//rotation y and scale are not needed, set it to 0
	rotation->y = 0;
	rotation->x = 0;
	bool rc = true;

	//easy approach
	//translation->x = (abs(m_refPoints.at(0).actpoint.x - m_refPoints.at(0).refpoint.x)
	//				+ abs(m_refPoints.at(1).actpoint.x - m_refPoints.at(1).refpoint.x)
	//				+ abs(m_refPoints.at(2).actpoint.x - m_refPoints.at(2).refpoint.x) / 3);
	//translation->y = (abs(m_refPoints.at(0).actpoint.y - m_refPoints.at(0).refpoint.y)
	//				+ abs(m_refPoints.at(1).actpoint.y - m_refPoints.at(1).refpoint.y)
	//				+ abs(m_refPoints.at(2).actpoint.y - m_refPoints.at(2).refpoint.y) / 3);

	//more complex approach
	double normXnext = abs(m_refPoints.at(0).actpoint.x - m_refPoints.at(0).refpoint.x)
				+ abs(m_refPoints.at(1).actpoint.x - m_refPoints.at(1).refpoint.x)
				+ abs(m_refPoints.at(2).actpoint.x - m_refPoints.at(2).refpoint.x) / 3;	
	double normYnext = abs(m_refPoints.at(0).actpoint.y - m_refPoints.at(0).refpoint.y)
				+ abs(m_refPoints.at(1).actpoint.y - m_refPoints.at(1).refpoint.y)
				+ abs(m_refPoints.at(2).actpoint.y - m_refPoints.at(2).refpoint.y) / 3;
	double normXcurrent = DBL_MAX;
	double normYcurrent = DBL_MAX;
	int degree = 0.1;
	cv::Mat pt1, pt2, pt3;
	while (sqrt((pow(normXcurrent, 2) + pow(normYcurrent, 2))) > sqrt(pow(normXnext, 2) + pow(normYnext, 2)))
	{
		//set next base
		normXcurrent = normXnext;
		normYcurrent = normYnext;
		//rotate point around origin
		double pt1d[4] = { m_refPoints.at(0).actpoint.x, m_refPoints.at(0).actpoint.y, 0, 1 };
		double pt2d[4] = { m_refPoints.at(1).actpoint.x, m_refPoints.at(1).actpoint.y, 0, 1 };
		double pt3d[4] = { m_refPoints.at(2).actpoint.x, m_refPoints.at(2).actpoint.y, 0, 1 };
		pt1 = cv::Mat(4, 1, CV_64F);
		pt1.data = (uchar*)&pt1d;
		pt2 = cv::Mat(4, 1, CV_64F);
		pt2.data = (uchar*)&pt2d;
		pt3 = cv::Mat(4, 1, CV_64F);
		pt3.data = (uchar*)&pt3d;
		double ddeg = degree * CV_PI / 180;
		double rotd[16] = { cos(ddeg), -sin(ddeg), 0, 0,
							sin(ddeg), cos(ddeg), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };
		cv::Mat rot = cv::Mat(4, 4, CV_64F);
		rot.data = (uchar*)&rotd;
		pt1 = rot * pt1;
		pt2 = rot * pt2;
		pt3 = rot * pt3;
		
		normXnext = abs(pt1.at<double>(1,0) - m_refPoints.at(0).refpoint.x)
			+ abs(pt2.at<double>(1,0) - m_refPoints.at(1).refpoint.x)
			+ abs(pt3.at<double>(1,0) - m_refPoints.at(2).refpoint.x) / 3;
		normYnext = abs(pt1.at<double>(2,0) - m_refPoints.at(0).refpoint.y)
			+ abs(pt2.at<double>(2,0) - m_refPoints.at(1).refpoint.y)
			+ abs(pt3.at<double>(2.0) - m_refPoints.at(2).refpoint.y) / 3;
		degree++;
	}

	rotation->x = degree;
	
	//harder translation
	translation->x = (abs(m_refPoints.at(0).actpoint.x - m_refPoints.at(0).refpoint.x)
					+ abs(m_refPoints.at(1).actpoint.x - m_refPoints.at(1).refpoint.x)
					+ abs(m_refPoints.at(2).actpoint.x - m_refPoints.at(2).refpoint.x) / 3);
	translation->y = (abs(m_refPoints.at(0).actpoint.y - m_refPoints.at(0).refpoint.y)
					+ abs(m_refPoints.at(1).actpoint.y - m_refPoints.at(1).refpoint.y)
					+ abs(m_refPoints.at(2).actpoint.y - m_refPoints.at(2).refpoint.y) / 3);
	return rc;
}
