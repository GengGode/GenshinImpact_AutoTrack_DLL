#include "pch.h"
#include "AutoTrackCore.h"
#include "ErrorCode.h"

AutoTrackCore::AutoTrackCore()
{
	logger = std::make_shared<MatchLogger>();

	_transform = new GetTransformClass(logger);
	_position = new GetPositionClass(logger);
	_direction = new GetDirectionClass(logger);
	_rotation = new  GetRotationClass(logger);
	_star = new  GetStarClass(logger);
	_starJson = new  GetStarJsonClass(logger);
	_uid = new  GetUIDClass(logger);
	_infoLoadPicture = new  GetInfoLoadPictureClass(logger);
	_infoLoadVideo = new GetInfoLoadVideoClass(logger);

	MapWorldOffset.x = MapWorldAbsOrigin_X - WorldCenter_X;
	MapWorldOffset.y = MapWorldAbsOrigin_Y - WorldCenter_Y;
	MapWorldScale = WorldScale;

}

AutoTrackCore::~AutoTrackCore(void)
{
}

bool AutoTrackCore::asyncStartService()
{
	isRunService = true;
	return isRunService;
}

bool AutoTrackCore::asyncStopService()
{
	isRunService = false;
	return isRunService;
}

bool AutoTrackCore::asyncSetHandle(long long int handle)
{
	return false;
}

bool AutoTrackCore::asyncSetWorldCenter(double x, double y)
{
	return false;
}

bool AutoTrackCore::asyncSetWorldScale(double scale)
{
	return false;
}

bool AutoTrackCore::asyncGetTransform(double& x, double& y, double& a)
{
	x = TransformData.x;
	y = TransformData.y;
	a = TransformData.a;
	return TransformData.result;
}

bool AutoTrackCore::asyncGetPosition(double& x, double& y)
{
	x = PositionData.x;
	y = PositionData.y;
	return PositionData.result;
}

bool AutoTrackCore::asyncGetDirection(double& a)
{
	a = DirectionData.a;
	return DirectionData.result;
}

bool AutoTrackCore::asyncGetRotation(double& a)
{
	a = RotationData.a2;
	return RotationData.result;
}

bool AutoTrackCore::asyncGetStar(double& x, double& y, bool& isEnd)
{
	x = StarData.x;
	y = StarData.y;
	isEnd = StarData.isEnd;
	return StarData.result;
}

bool AutoTrackCore::asyncGetStarJson(char* jsonBuff)
{
	jsonBuff = StarJsonData.jsonBuff; //char* 
	return StarJsonData.result;
}

bool AutoTrackCore::asyncGetUID(int& uid)
{
	uid = UIDData.uid;
	return UIDData.result;
}

bool AutoTrackCore::asyncGetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
	path = InfoLoadPictureData.path; //char*
	uid = InfoLoadPictureData.uid;
	x = InfoLoadPictureData.x;
	y = InfoLoadPictureData.y;
	a = InfoLoadPictureData.a;
	return InfoLoadPictureData.result;
}

bool AutoTrackCore::asyncGetInfoLoadVideo(char* path, char* pathOutFile)
{
	path = InfoLoadVideoData.path; //char*
	pathOutFile = InfoLoadVideoData.pathOutFile; //char*
	return InfoLoadVideoData.result;
}

bool AutoTrackCore::init()
{
	return false;//_u.init();
}

bool AutoTrackCore::uninit()
{
	return _u.uninit();
}

bool AutoTrackCore::SetHandle(long long int handle)
{
	if (handle == 0)
	{
		is_Auto_getHandle = true;
		return true;
	}
	else
	{
		is_Auto_getHandle = false;
		giHandle = (HWND)handle;
	}

	return IsWindow(giHandle);
}

bool AutoTrackCore::SetWorldCenter(double x, double y)
{
	UserWorldOrigin_X = x;
	UserWorldOrigin_Y = y;
	return true;
}

bool AutoTrackCore::SetWorldScale(double scale)
{
	UserWorldScale = scale;
	return true;
}


bool AutoTrackCore::GetTransform(double& x, double& y, double& a)
{
	return _transform->get(x,y,a);
}

bool AutoTrackCore::GetPosition(double& x, double& y)
{
	return _position->get(x,y);
}


bool AutoTrackCore::GetDirection(double& a)
{
	return _direction->get(a);
}

bool AutoTrackCore::GetRotation(double& a)
{
	return _rotation->get(a);
}

bool AutoTrackCore::GetStar(double& x, double& y, bool& isEnd)
{
	return _star->get(x, y, isEnd);
}

bool AutoTrackCore::GetStarJson(char* jsonBuff)
{
	return _starJson->get(jsonBuff);
}

bool AutoTrackCore::GetUID(int& uid)
{
	return _uid->get(uid);
}

bool AutoTrackCore::GetInfoLoadPicture(char* path, int& uid, double& x, double& y, double& a)
{
	return _infoLoadPicture->get(path,uid,x,y,a);
}

bool AutoTrackCore::GetInfoLoadVideo(char* path, char* pathOutFile)
{
	return _infoLoadVideo->get(path,pathOutFile);
}

int AutoTrackCore::GetLastError()
{
#ifdef _DEBUG
	std::cout << err;
#endif
	return err;
}
const char* AutoTrackCore::GetLastErrorStr()
{
	return nullptr;
}

