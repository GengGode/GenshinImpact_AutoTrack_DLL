#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "GetPositionClass.h"
#include "GetDirectionClass.h"
class GetStarJsonClass
{
	Logger logger;
public:
	GetStarJsonClass() = delete;
	GetStarJsonClass(const Logger& loggerService);

	bool get(char* jsonBuff);
private:

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	FlowWork wStar;
	cv::Mat giMiniMapRef;
	cv::Mat giStarRef;

	bool isOnCity = false;
	bool isStarVisible = false;

private:
	bool getPaimonRefMat() { return false; }
	bool getMiniMapRefMat() { return false; }
};

