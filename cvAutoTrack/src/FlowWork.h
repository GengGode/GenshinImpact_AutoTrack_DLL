#pragma once
#include <vector>
#include "ErrorCode.h"
using namespace std;

class AutoTrackCore;

class FlowWork
{
	vector<bool(AutoTrackCore::*)(void)> funPtrList;
	vector<int> funErrCodeList;
	AutoTrackCore* AT;
	ErrorCode& err=ErrorCode::getInstance();
public:
	FlowWork();
	~FlowWork();

	void append(AutoTrackCore* at,bool(AutoTrackCore::* funPtr)(void),int errCode);

	bool run();
};

