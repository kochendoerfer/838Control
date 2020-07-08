#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "SettingsVals.h"

class ImgProc
{
public:
	ImgProc();
	~ImgProc();

	void init(LUNOBackend::SettingsVals *settings);
	cv::Point getCenterOfMarker(const cv::Mat *src);

	boost::signals2::signal<void(cv::Point)> sigPosFound;
	boost::signals2::signal<void(boost::posix_time::ptime, unsigned int, std::string, std::string)> sigLogMsg;
	boost::signals2::signal<void(int)> sigAlgStatus;

private:
	//variables that need to be initialised using setting class
	int m_AlgorithSelect = 0;
	bool m_debugMode = false;
	//morphology
	int m_DTFKernelSize = 10;
	int m_threshold = 90;
	int m_erodeVerticalXSize = 2;
	int m_erodeVerticalYSize = 19;
	int m_dilateVerticalXSize = 3;
	int m_erodeHorizontalXSize = 17;
	int m_erodeHorizontalYSize = 3;
	int m_dilateHorizontalYSize = 4;
	//hough lines
	int m_angleTolerance = 0;
	double m_AcceptedOffset = 0.15;
	int m_houghThresh = 50;
	int m_houghLineLength = 70;
	int m_houghGapSize = 70;
	LUNOBackend::SettingsVals *m_settings = NULL;
	

	cv::Point AlgoMorphRect(const cv::Mat * src);
	cv::Point AlgoHough(const cv::Mat * src);
	void getThresh(cv::Mat &src, cv::Mat &dst, int threshold, int threshFlag);
	void erode(const cv::Mat &src, cv::Mat &dst, int sizeX, int sizeY, int structType);
	void dilate(const cv::Mat &src, cv::Mat &dst, int sizeX, int sizeY, int structType);
	void morph(const cv::Mat &src, cv::Mat &dst, int operation, int sizeX, int sizeY, int structType);
	void fftShift(cv::Mat &src, cv::Mat &dst);
	void getPowerSpectrum(cv::Mat &src, cv::Mat &magI);
	void synthesizeRadianFilter(cv::Mat &inout, cv::Point center, int radius);
	void synthesizeCrossFilter(cv::Mat &inout, cv::Point center, int size, int thickness);
	void filter2DFreq(const cv::Mat &src, cv::Mat &dst, const cv::Mat &kernel);
	void findRect(cv::Mat &src, std::vector<std::vector<cv::Point>> &rectangles);
	bool validateImg(const cv::Mat &src);
	bool validateResult(cv::Point res, int ImgSizeX, int ImgSizeY, float tolerance);
};