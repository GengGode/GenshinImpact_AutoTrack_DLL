#pragma once

#include <opencv.hpp>
#include <xfeatures2d.hpp>

enum Sync
{
	FromLocal,
	FromAppliction
};

class KeyPointSerialization
{
	cv::FileStorage _fs;
	std::string _filePath;
public:
	KeyPointSerialization()=delete;
	KeyPointSerialization(std::string path);
	KeyPointSerialization(std::vector<cv::KeyPoint> KeyPoint,std::string path);
	~KeyPointSerialization();
public:
	std::vector<cv::KeyPoint> keyPoint;

	void sync(Sync type=Sync::FromLocal);
private:

};

KeyPointSerialization::KeyPointSerialization(std::string path):_filePath(path)
{
	_fs = cv::FileStorage(path, cv::FileStorage::READ);
	_fs["KeyPoint"] >> keyPoint;

}

inline KeyPointSerialization::KeyPointSerialization(std::vector<cv::KeyPoint> KeyPoint, std::string path)
{
	keyPoint = KeyPoint;
	_filePath = path;
	_fs = cv::FileStorage(path, cv::FileStorage::WRITE);

}

KeyPointSerialization::~KeyPointSerialization()
{
	_fs << "KeyPoint" << keyPoint;
}

inline void KeyPointSerialization::sync(Sync type)
{
	switch (type)
	{
		case Sync::FromLocal:
		{
			_fs = cv::FileStorage(_filePath, cv::FileStorage::READ);
			_fs["KeyPoint"] >> keyPoint;
			break;
		}

		case Sync::FromAppliction:
		{
			_fs = cv::FileStorage(_filePath, cv::FileStorage::WRITE);
			break;
		}
		default:
		{
			_fs = cv::FileStorage(_filePath, cv::FileStorage::READ);
			_fs["KeyPoint"] >> keyPoint;
		}
	}
}

#define cvKeyPoint KeyPointSerialization

