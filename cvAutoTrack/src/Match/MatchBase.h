#pragma once
#include "../Thread/ThreadBase.h"

template <class T>
class MatchBase 
{
public:
	~MatchBase() = delete;
	virtual void match() = 0;
	virtual T asyncMatch() = 0;
};

//#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/xfeatures2d/nonfree.hpp>
//template <class T>
//class SurfMatch :public MatchBase,public ThreadBase<T>
//{
//	std::atomic<T> value;
//public:
//	SurfMatch();
//	
//	void match()
//	{
//		//匹配过程
//		//更新输出值
//	}
//	T matchAsync()
//	{
//		return value;
//	}
//
//};
//
//template<class T>
//inline SurfMatch<T>::SurfMatch()
//{
//	setFunctionPtr(match);
//}
