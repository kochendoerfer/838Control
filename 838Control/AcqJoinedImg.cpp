#include "AcqJoinedImg.h"



AcqJoinedImg::AcqJoinedImg()
{
}


AcqJoinedImg::~AcqJoinedImg()
{
}

void AcqJoinedImg::init(LUNOBackend::SettingsVals *settings)
{
	//get parameters from settingsclass
	m_settings = settings;
	m_settings->getValDouble("ImgSizeX", m_sizeX);
	m_settings->getValDouble("ImgSizeY", m_sizeY);
	m_settings->getValDouble("PixelSize", m_mmPerPixel);
	//create image in required size
	if (m_sizeX > 0 && m_sizeY > 0 && m_mmPerPixel > 0)
		m_joinedImg = cv::Mat(m_sizeX / m_mmPerPixel, m_sizeY / m_mmPerPixel, CV_8UC1);
	else
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", "Imagesize X/Y and Pixelsize must be larger than zero.");
}

void AcqJoinedImg::clear()
{
//TODO implementieren:
	if (!m_joinedImg.empty())
		m_joinedImg.release();
	m_firstImg = true;
	//get parameters from settingsclass
	m_settings->getValDouble("ImgSizeX", m_sizeX);
	m_settings->getValDouble("ImgSizeY", m_sizeY);
	m_settings->getValDouble("PixelSize", m_mmPerPixel);
	//create image in required size
	if (m_sizeX > 0 && m_sizeY > 0 && m_mmPerPixel > 0)
		m_joinedImg = cv::Mat(m_sizeX / m_mmPerPixel, m_sizeY / m_mmPerPixel, CV_8UC1);
}

void AcqJoinedImg::addImage(cv::Mat & src, double posX, double posY)
{
	if (m_firstImg)
	{
		m_StartAxisXmm = posX;
		m_StartAxisYmm = posY;
		m_firstImg = false;
	}
	//check src img content and type
	if (!src.empty() && src.type() == CV_8UC1)
	{
		//check bounds of img
		int idxX = m_joinedImg.cols - m_squareImgSize - abs(m_StartAxisYmm - posY) / m_mmPerPixel;
		int idxY = abs(m_StartAxisXmm - posX) / m_mmPerPixel;
		idxX = int((idxX + m_squareImgSize / 2) / m_squareImgSize) * m_squareImgSize;
		idxY = int((idxY + m_squareImgSize / 2) / m_squareImgSize) * m_squareImgSize;
		std::string startpos = "x-start: " + std::to_string(m_StartAxisXmm) + " y-start: " + std::to_string(m_StartAxisYmm);
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", startpos);
		std::string msgpos = "x-pos: " + std::to_string(posX) + " y-pos: " + std::to_string(posY);
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", msgpos);
		std::string idxs = "Index X: " + std::to_string(idxX) + " Index Y: " + std::to_string(idxY);
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", idxs);
		if (idxX + m_squareImgSize <= m_joinedImg.cols &&
			idxX >= 0 &&
			idxY + m_squareImgSize <= m_joinedImg.rows &&
			idxY >= 0)
		{
			cv::Mat rect = src(cv::Rect(139, 11, m_squareImgSize, m_squareImgSize));
			rect.copyTo(m_joinedImg(cv::Rect(idxX, idxY, m_squareImgSize, m_squareImgSize)));
			sigImgAdded();
		}
		else
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", "Image Acquisition reached Limits of joined Image");
	}
	else
		sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", "Image could not be added, no contents or wrong type");
}

void AcqJoinedImg::saveImage(std::string path)
{
	if (!m_joinedImg.empty())
	{
		//cv::Mat cropped(m_joinedImg(cv::Rect(0, 2000, 36000, 34000)));
		//cv::Mat rotated;
		//cv::rotate(cropped, rotated, cv::ROTATE_90_COUNTERCLOCKWISE);
		if (!cv::imwrite(path, m_joinedImg))
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", "Image could not be saved.");
		else
			sigLogMsg(boost::posix_time::microsec_clock::universal_time(), LUNO_LOG_TYPE_INTERNAL_MESSAGE, "[BV]", "Image saved to: " + path);
	}
	m_firstImg = true;
}
