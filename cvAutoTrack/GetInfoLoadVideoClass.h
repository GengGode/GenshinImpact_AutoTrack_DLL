#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"
#include "Kalmanfilter.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "newclass2.h"

class GetInfoLoadVideoClass
{
	Logger logger;
public:
	GetInfoLoadVideoClass() = delete;
	GetInfoLoadVideoClass(const Logger& loggerService);


	bool get(char* path, char* pathOutFile);

private:

	newclass2 test;

	bool is_init_end = false;

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	cv::Mat giFrame;
	cv::Mat giPaimonRef;
	cv::Mat giMiniMapRef;
	cv::Mat giUIDRef;
	cv::Mat giAvatarRef;
private:
	bool init() { return false; }
	bool getPaimonRefMat() { return false; }
	bool getMiniMapRefMat() { return false; }
	bool getUIDRefMat() { return false; }
	bool getAvatarRefMat() { return false; }
};


