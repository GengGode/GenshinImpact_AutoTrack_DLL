#include "pch.h"
#include "GetStarJsonClass.h"

GetStarJsonClass::GetStarJsonClass(const Logger& loggerService)
{
	logger = loggerService;

	//wStar.append(this, &AutoTrackCore::getGengshinImpactWnd, 121);
	//wStar.append(this, &AutoTrackCore::getGengshinImpactRect, 122);
	//wStar.append(this, &AutoTrackCore::getGengshinImpactScreen, 123);
	//wStar.append(this, &AutoTrackCore::getMiniMapRefMat, 124);
}

bool GetStarJsonClass::get(char* jsonBuff)
{
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	vector<cv::Point2d> pos;
	double scale = 1.3;


	if (wStar.run() == false)
	{
		return false;
	}

	getPaimonRefMat();

	//一个bug 未开游戏而先开应用，开游戏时触发
	cv::cvtColor(giMiniMapRef(cv::Rect(36, 36, giMiniMapRef.cols - 72, giMiniMapRef.rows - 72)),
		giStarRef, cv::COLOR_RGBA2GRAY);


	matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
	if (maxVal < 0.66)
	{
		isStarVisible = false;
	}
	else
	{
		isLoopMatch = true;
		isStarVisible = true;
		pos.push_back(cv::Point2d(maxLoc) -
			cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
			cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		giStarRef(cv::Rect(maxLoc.x, maxLoc.y, giMatchResource.StarTemplate.cols, giMatchResource.StarTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(giMatchResource.StarTemplate, giStarRef, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.push_back(cv::Point2d(maxLoc) -
				cv::Point2d(giStarRef.cols / 2, giStarRef.rows / 2) +
				cv::Point2d(giMatchResource.StarTemplate.cols / 2, giMatchResource.StarTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
	}

	if (isOnCity)
	{
		scale = 0.8667;
	}

	if (isStarVisible == true)
	{

		sprintf_s(jsonBuff, 1024, "{\"n\": %d ,\"list\":[", static_cast<int>(pos.size()));//[123,12],[123,53]]}")
		for (int i = 0; i < pos.size(); i++)
		{
			char buff[99];
			if (i == 0)
			{
				sprintf_s(buff, 99, "[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			else
			{
				sprintf_s(buff, 99, ",[ %lf , %lf ]", pos[i].x * scale, pos[i].y * scale);
			}
			strncat_s(jsonBuff, 1024, buff, 99);
		}
		strncat_s(jsonBuff, 1024, "]}", 3);
		return true;
	}
	sprintf_s(jsonBuff, 99, "{\"n\": 0 ,\"list\":[]}");
	return true;
	//err = 501;
	//return false;
}
