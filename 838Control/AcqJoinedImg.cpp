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
	m_settings->getValInt("ImgSizeX", m_sizeX);
	m_settings->getValInt("ImgSizeY", m_sizeY);
	m_settings->getValDouble("PixelSize", m_mmPerPixel);
	//create image in required size
	if (m_sizeX < 0 && m_sizeY < 0 && m_mmPerPixel < 0)
		m_joinedImg = cv::Mat(m_sizeX / m_mmPerPixel, m_sizeY / m_mmPerPixel, CV_8UC1);
	else
		sigLogMsg("Imagesize X/Y and Pixelsize must be larger than zero.");
}

void AcqJoinedImg::clear()
{
//TODO implementieren:
	if (!m_joinedImg.empty())
		m_joinedImg.release();
}

void AcqJoinedImg::addImage(cv::Mat & src, int posX, int posY)
{
	//check src img content and type
	if (!src.empty() && src.type() == CV_8UC1)
	{
		//check bounds of img
		if (posX + src.cols <= m_joinedImg.cols &&
			posY + src.rows <= m_joinedImg.rows)
		{
			src.copyTo(m_joinedImg(cv::Rect(posX, posY, src.cols, src.rows)));
			sigImgAdded();
		}
		else
			sigLogMsg("Image Acquisition reached Limits of joined Image");
	}
	else
		sigLogMsg("Image could not be added, no contents or wrong type");
}

void AcqJoinedImg::saveImage(std::string name)
{
	if (!m_joinedImg.empty())
		if(!cv::imwrite(m_path + name, m_joinedImg));
			sigLogMsg("Image could not be saved.");
}
