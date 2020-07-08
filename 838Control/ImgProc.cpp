#include "ImgProc.h"

#include "LUNOdefs.h"

ImgProc::ImgProc()
{
}

ImgProc::~ImgProc()
{
}

void ImgProc::init(LUNOBackend::SettingsVals * settings)
{
	m_settings = settings;
	//init variables using settings class
	m_settings->getValInt("AlgorithmSelect", m_AlgorithSelect);
	m_settings->getValBool("DebugMode", m_debugMode);
	//for algorithm using morphology
	m_settings->getValInt("DFTKernelSize", m_DTFKernelSize);
	m_settings->getValInt("ImgBnThreshold", m_threshold);
	m_settings->getValInt("ErodeVerticalSizeX", m_erodeVerticalXSize);
	m_settings->getValInt("ErodeVerticalSizeY", m_erodeVerticalYSize);
	m_settings->getValInt("ErodeHorizontalSizeX", m_erodeVerticalXSize);
	m_settings->getValInt("ErodeHorizontalSizeY", m_erodeVerticalYSize);
	m_settings->getValInt("DilateVerticalSizeX", m_dilateVerticalXSize);
	m_settings->getValInt("DilateHorizontalSizeY", m_dilateHorizontalYSize);

	//for algorithm using hough lines
	m_settings->getValInt("AngleTolerance", m_angleTolerance);
	m_settings->getValDouble("AcceptedOffset", m_AcceptedOffset);
	m_settings->getValInt("HoughThresh", m_houghThresh);
	m_settings->getValInt("HoughLength", m_houghLineLength);
	m_settings->getValInt("HoghGapLength", m_houghGapSize);
}

cv::Point ImgProc::getCenterOfMarker(const cv::Mat *src)
{
	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[detectMarker]", "Step 0");
	///validate src image
	if (validateImg(*src))
	{
		//find marker with correct algorithm
		cv::Point pm;
		switch (m_AlgorithSelect)
		{
		case 1:
			pm = AlgoMorphRect(src);
			break;
		case 2:
			pm = AlgoHough(src);
			break;
		default:
			break;
		}
		if (validateResult(pm, src->cols, src->rows, 0.2))
		{
			//emit found position
			sigPosFound(pm);
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[detectMarker]", "Finished");
			sigAlgStatus(0); //MarkerFound
			return pm;
		}
		else
		{
			sigPosFound(cv::Point(-1, -1));
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[detectMarker]", "Could not find Marker");
			sigAlgStatus(1); //MarkerNOTFound

			return cv::Point(-1, -1);
		}
	}
	sigPosFound(cv::Point(-1, -1));
	sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[detectMarker]", "Invalid Image");
	sigAlgStatus(2); //Invalid Image
	return cv::Point(-1, -1);
}

cv::Point ImgProc::AlgoMorphRect(const cv::Mat * src)
{
	///generate filter kernel
	cv::Mat kernel = cv::Mat(src->size(), CV_32F, cv::Scalar(0));
	synthesizeCrossFilter(kernel, cv::Point(int(kernel.cols / 2), int(kernel.rows / 2)), MIN(kernel.rows, kernel.cols), m_DTFKernelSize);
	fftShift(kernel, kernel);
	///filtering
	cv::Mat filtered, newSrc;
	src->convertTo(newSrc, CV_32F);
	filter2DFreq(newSrc, filtered, kernel);
	filtered.convertTo(filtered, CV_8U);
	cv::normalize(filtered, filtered, 0, 255, cv::NORM_MINMAX);
	//morphological operations
	cv::Mat thresh, morphX, morphY;
	//threshold first
	getThresh(filtered, thresh, m_threshold, cv::THRESH_BINARY);
	thresh = (255 - thresh);
	//vertical rect
	erode(thresh, morphY, m_erodeVerticalXSize, m_erodeVerticalYSize, cv::MORPH_CROSS);
	dilate(morphY, morphY, m_dilateVerticalXSize, morphY.rows, cv::MORPH_RECT);
	//horizontal rect
	erode(thresh, morphX, m_erodeHorizontalXSize, m_erodeHorizontalYSize, cv::MORPH_CROSS);
	dilate(morphX, morphX, morphX.cols, m_dilateHorizontalYSize, cv::MORPH_RECT);

	//find rectangles
	std::vector<std::vector<cv::Point>> rectX, rectY;
	findRect(morphX, rectX);
	findRect(morphY, rectY);
	cv::Point pm;
	if (rectX.size() == 1 && rectY.size() == 1)
		pm = cv::Point((rectY[0][2].x + rectY[0][0].x) / 2, (rectX[0][0].y + rectX[0][1].y) / 2);
	return pm;
}

cv::Point ImgProc::AlgoHough(const cv::Mat * src)
{
	cv::Mat clahe, adaptive, houghCol, thresh, srcCopy;
	src->copyTo(srcCopy);
	//static threshold
	cv::threshold(srcCopy, thresh, 200, 255, cv::THRESH_BINARY);
	thresh.copyTo(adaptive);
	////adaptive threshold
	//doCLAHE(src, clahe, 25);
	//adaptiveThresh(clahe, adaptive, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 51, 10);
	//adaptive = 255 - adaptive;
	//proc.erode(adaptive, adaptive, 2, 2, cv::MORPH_RECT);
	cv::cvtColor(adaptive, houghCol, cv::COLOR_GRAY2BGR);
	cv::cvtColor(srcCopy, srcCopy, cv::COLOR_GRAY2BGR);
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(adaptive, lines, 1, CV_PI / 180, m_houghThresh, m_houghLineLength, m_houghGapSize);

	//reject invalid lines
	//lines have to be either horizontal or vertical
	int x = srcCopy.cols;
	int y = srcCopy.rows;
	auto it = lines.begin();
	while (it != lines.end())
	{
		if (abs(it->val[0] - it->val[2]) > m_angleTolerance
			&& abs(it->val[1] - it->val[3]) > m_angleTolerance)
			it = lines.erase(it);
		else
			it++;
	}

	//sort lines in horizontal and vertical lines
	//get average of lines
	std::vector<cv::Vec4i>hlines, vlines;
	int yAvg = 0;
	int yCount = 0;
	int xAvg = 0;
	int xCount = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		//horizontal lines
		if (abs(lines[i][0] - lines[i][2]) > abs(lines[i][1] - lines[i][3]))
		{
			if (lines[i][1] < y / 2 + y * m_AcceptedOffset
				&& lines[i][1] > y / 2 - y * m_AcceptedOffset)
			{
				hlines.push_back(lines.at(i));
				yAvg += (lines[i][1] + lines[i][3]) / 2;
				yCount++;
			}
		}
		//vertical lines
		else if (abs(lines[i][0] - lines[i][2]) < abs(lines[i][1] - lines[i][3]))
		{
			if (lines[i][0] < x / 2 + x * m_AcceptedOffset
				&& lines[i][0] > x / 2 - x * m_AcceptedOffset)
			{
				vlines.push_back(lines.at(i));
				xAvg += (lines[i][0] + lines[i][2]) / 2;
				xCount++;
			}
		}
	}
	xAvg /= xCount;
	yAvg /= yCount;
	return cv::Point(xAvg, yAvg);
}

void ImgProc::getThresh(cv::Mat &src, cv::Mat &dst, int threshold, int threshFlag)
{
	if ((threshFlag & cv::THRESH_OTSU) == cv::THRESH_OTSU)
	{
		threshold = 0;
	}
	cv::threshold(src, dst, threshold, 255, threshFlag);
}

void ImgProc::erode(const cv::Mat &src, cv::Mat &dst, int sizeX, int sizeY, int structType)
{
	cv::Mat element = cv::getStructuringElement(structType,
		cv::Size(2 * sizeX + 1, 2 * sizeY + 1),
		cv::Point(sizeX, sizeY));

	cv::erode(src, dst, element);
}

void ImgProc::dilate(const cv::Mat &src, cv::Mat &dst, int sizeX, int sizeY, int structType)
{
	cv::Mat element = cv::getStructuringElement(structType,
		cv::Size(2 * sizeX + 1, 2 * sizeY + 1),
		cv::Point(sizeX, sizeY));

	cv::dilate(src, dst, element);
}

void ImgProc::morph(const cv::Mat &src, cv::Mat &dst, int operation, int sizeX, int sizeY, int structType)
{
	cv::Mat element = cv::getStructuringElement(structType,
		cv::Size(2 * sizeX + 1, 2 * sizeY + 1),
		cv::Point(sizeX, sizeY));
	cv::morphologyEx(src, dst, operation, element);
}

void ImgProc::fftShift(cv::Mat &src, cv::Mat &dst)
{
	dst = src.clone();
	int cx = dst.cols / 2;
	int cy = dst.rows / 2;
	cv::Mat q0(dst, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(dst, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(dst, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(dst, cv::Rect(cx, cy, cx, cy));
	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void ImgProc::getPowerSpectrum(cv::Mat &I, cv::Mat &magI)
{
	cv::Mat padded;
	int m = cv::getOptimalDFTSize(I.rows);
	int n = cv::getOptimalDFTSize(I.cols);
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);

	cv::dft(complexI, complexI);


	split(complexI, planes);
	magnitude(planes[0], planes[1], planes[0]);
	magI = planes[0];
	cv::pow(magI, 2, magI);
	magI += cv::Scalar::all(1);
	cv::log(magI, magI);

	magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));
}

void ImgProc::synthesizeRadianFilter(cv::Mat& inout, cv::Point center, int radius)
{
	cv::circle(inout, center, radius, 1, -1, 8);
}

void ImgProc::synthesizeCrossFilter(cv::Mat &inout, cv::Point center, int size, int thickness)
{
	cv::drawMarker(inout, center, 1, cv::MARKER_CROSS, size, thickness);
}

void ImgProc::filter2DFreq(const cv::Mat &src, cv::Mat &dst, const cv::Mat &kernel)
{
	cv::Mat planes[2] = { cv::Mat_<float>(src.clone()), cv::Mat::zeros(src.size(), CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI, cv::DFT_SCALE);
	cv::Mat planesH[2] = { cv::Mat_<float>(kernel.clone()), cv::Mat::zeros(kernel.size(), CV_32F) };
	cv::Mat complexH;
	cv::merge(planesH, 2, complexH);
	cv::Mat complexIH;
	cv::mulSpectrums(complexI, complexH, complexIH, 0);
	cv::idft(complexIH, complexIH);
	cv::split(complexIH, planes);
	dst = planes[0];
}

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

void ImgProc::findRect(cv::Mat &src, std::vector<std::vector<cv::Point>> &rectangles)
{
	std::vector < std::vector<cv::Point> > contours;
	cv::findContours(src, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	std::vector<cv::Point> approx;
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true)*0.002, true);
		if (approx.size() == 4 &&
			fabs(cv::contourArea(approx)) > 1000 &&
			cv::isContourConvex(approx))
		{
			double maxCosine = 0;
			for (size_t j = 2; j < 5; j++)
			{
				double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}
			if (maxCosine < 0.3)
				rectangles.push_back(approx);
		}
	}
}

bool ImgProc::validateImg(const cv::Mat &src)
{
	//is  image not empty and of correct type
	if (!src.empty() && src.type() == CV_8UC1)
	{
		cv::Scalar mean, stddev;
		cv::meanStdDev(src, mean, stddev, cv::Mat());
		//does image have a high enough contrast
		if (stddev.val[0] * 2 > 100)
			return true;
	}
	return true;
}

bool ImgProc::validateResult(cv::Point res, int ImgSizeX, int ImgSizeY, float tolerance)
{
	int xMin = (ImgSizeX / 2) - ImgSizeX * tolerance;
	int xMax = (ImgSizeX / 2) + ImgSizeX * tolerance;
	int yMin = (ImgSizeY / 2) - ImgSizeY * tolerance;
	int yMax = (ImgSizeY / 2) + ImgSizeY * tolerance;
	if (xMin <= res.x && res.x <= xMax &&
		yMin <= res.y && res.y <= yMax)
	{
		return true;
	}
	return false;
}