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
class GetStarClass
{
	Logger logger;
public:
	GetStarClass() = delete;
	GetStarClass(const Logger& loggerService);

	bool get(double& x, double& y, bool& isEnd);
private:

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	FlowWork wStar;
	cv::Mat giMiniMapRef;
	cv::Mat giStarRef;

	bool isOnCity = false;
	bool isStarVisible = false;

private:
	bool getPaimonRefMat() {}
	bool getMiniMapRefMat() {}
};
