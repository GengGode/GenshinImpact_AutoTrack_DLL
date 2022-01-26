#pragma once
#include "src/Logger/LoggingFacility.h"

#include "LoadGiMatchResource.h"
#include "FlowWork.h"
#include "ErrorCode.h"

class GetUIDClass
{
	Logger logger;
public:
	GetUIDClass() = delete;
	GetUIDClass(const Logger& loggerService);

	bool get(int& uid);

private:
	FlowWork wForAfter;

	LoadGiMatchResource giMatchResource;
	ErrorCode& err = ErrorCode::getInstance();

	cv::Mat giUIDRef;

private:
	bool getUIDRefMat() { return false; }
};

