#include "pch.h"
#include "GetDirectionClass.h"

GetDirectionClass::GetDirectionClass(const Logger& loggerService)
{
	logger = loggerService;

	//wAvatar.append(this, &AutoTrackCore::getGengshinImpactWnd, 113);
	//wAvatar.append(this, &AutoTrackCore::getGengshinImpactRect, 114);
	//wAvatar.append(this, &AutoTrackCore::getGengshinImpactScreen, 115);
	//wAvatar.append(this, &AutoTrackCore::getAvatarRefMat, 116);
}

bool GetDirectionClass::get(double& a)
{
	if (wAvatar.run() == false)
	{
		return false;
	}

	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;

	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));

	cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);

	if (maxVal < 0.36 || maxVal == 1)
	{
		err = 6;//未能匹配到派蒙
		return false;
	}

	getMiniMapRefMat();

	if (giMiniMapRef.empty())
	{
		err = 5;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}

	getAvatarRefMat();

	if (giAvatarRef.empty())
	{
		err = 11;//未能取到小箭头区域
		return false;
	}

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12, gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		err = 9;//提取小箭头特征误差过大
		return false;
	}

	for (int i = 0; i < 3; i++)
	{
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
		AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
	}

	AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
	AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	}
	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
	}
	if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
	}

	AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
	KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

	a = Line2Angle(KeyLine);

	err = 0;
	return true;
}
