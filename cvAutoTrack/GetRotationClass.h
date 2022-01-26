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
class GetRotationClass
{
	Logger logger;
public:
	GetRotationClass() = delete;
	GetRotationClass(const Logger& loggerService);


	bool get(double& a);
private:

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	FlowWork wForAfter;

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;
	cv::Mat giStarRef;
private:
	bool getPaimonRefMat() { return false; }
	bool getMiniMapRefMat() { return false; }
};

