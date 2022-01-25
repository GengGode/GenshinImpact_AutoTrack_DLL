#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

class GetDirectionClass
{
	Logger logger;
public:
	GetDirectionClass() = delete;
	GetDirectionClass(const Logger&loggerService);

	bool get(double& a);

private:
	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	FlowWork wAvatar;

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giAvatarRef;
private:
	bool getPaimonRefMat() {}
	bool getMiniMapRefMat() {}
	bool getAvatarRefMat() {}
	
};

