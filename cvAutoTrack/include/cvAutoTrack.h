#pragma once

#ifdef CVAUTOTRACK_EXPORTS
#define CVAUTOTRACK_PORT __declspec(dllexport)
#else
#define CVAUTOTRACK_PORT __declspec(dllimport)
#endif

#define CVAUTOTRACK_CALL  
//__stdcall
#define CVAUTOTRACK_API CVAUTOTRACK_PORT CVAUTOTRACK_CALL

//#ifdef CVAUTOTRACK_EXPORTS
//#define CVAUTOTRACK_API __declspec(dllexport)
//#else
//#define CVAUTOTRACK_API __declspec(dllimport)
//#endif

//#define CVAUTOTRACK_API 

// 这是已导出的函数。

extern "C" CVAUTOTRACK_API bool startServe();
extern "C" CVAUTOTRACK_API bool stopServe();

extern "C" CVAUTOTRACK_API bool init();
extern "C" CVAUTOTRACK_API bool uninit();
extern "C" CVAUTOTRACK_API bool SetHandle(long long int handle);
extern "C" CVAUTOTRACK_API bool SetWorldCenter(double x, double y);
extern "C" CVAUTOTRACK_API bool SetWorldScale(double scale);


extern "C" CVAUTOTRACK_API bool GetTransform(double& x, double& y, double& a);
extern "C" CVAUTOTRACK_API bool GetPosition(double& x, double& y);
extern "C" CVAUTOTRACK_API bool GetDirection(double& a);
extern "C" CVAUTOTRACK_API bool GetRotation(double& a);
extern "C" CVAUTOTRACK_API bool GetStar(double &x, double &y, bool &isEnd);
extern "C" CVAUTOTRACK_API bool GetStarJson(char *jsonBuff);
extern "C" CVAUTOTRACK_API bool GetUID(int &uid);

extern "C" CVAUTOTRACK_API bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
extern "C" CVAUTOTRACK_API bool GetInfoLoadVideo(char* path, char* pathOutFile);

extern "C" CVAUTOTRACK_API int GetLastErr();

extern "C" CVAUTOTRACK_API const char* GetLastErrStr();

