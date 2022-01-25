#include "pch.h"
#include "GetStarClass.h"

GetStarClass::GetStarClass(const Logger& loggerService)
{
	logger = loggerService;

	//wStar.append(this, &AutoTrackCore::getGengshinImpactWnd, 121);
	//wStar.append(this, &AutoTrackCore::getGengshinImpactRect, 122);
	//wStar.append(this, &AutoTrackCore::getGengshinImpactScreen, 123);
	//wStar.append(this, &AutoTrackCore::getMiniMapRefMat, 124);
}

bool GetStarClass::get(double& x, double& y, bool& isEnd)
{
	static bool isNotSee = false;
	static vector<cv::Point2d> pos;
	static int seeId = 0;

	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	double scale = 1.3;

	if (isNotSee)
	{
		if (isOnCity)
		{
			scale = 0.8667;
		}
		x = pos[seeId].x * scale;
		y = pos[seeId].y * scale;
		seeId++;
		if (seeId == pos.size())
		{
			isEnd = true;
			isNotSee = false;
		}
		else
		{
			isEnd = false;
			isNotSee = true;
		}
		return true;
	}
	else
	{
		pos.clear();
		seeId = 0;

		if (wStar.run() == false)
		{
			return false;
		}

		getPaimonRefMat();

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


		if (isStarVisible == true)
		{
			if (isOnCity)
			{
				scale = 0.8667;
			}
			x = pos[seeId].x * scale;
			y = pos[seeId].y * scale;
			seeId++;
			if (seeId == pos.size())
			{
				isEnd = true;
				isNotSee = false;
			}
			else
			{
				isEnd = false;
				isNotSee = true;
			}
			return true;
		}
		err = 601;
		return false;
	}

	return false;
}
