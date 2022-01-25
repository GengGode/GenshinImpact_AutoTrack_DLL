// cvAT_dllTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#pragma execution_character_set("utf-8")

#include <iostream>
#include <cvAutoTrack.h>

#include <Windows.h>

//#define TestVideo
#ifdef TestVideo
#include <vector>
#endif

int main()
{
	//system("chcp 65001");
	std::ios::sync_with_stdio(false);

	// 静态方法调用
	// 初始化
	//init();
	// 准备变量
	double x = 0;
	double y = 0;
	double a = 0;
	double x2 = 0;
	double y2 = 0;
	double a2 = 0;
	double aa2 = 0;
	int uid = 0;
	// 调用循环

#ifdef TestVideo
	
	std::vector<std::vector<double>> his;
	char path[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Picture/001.png" };
	char pathV[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.mp4" };
	
	char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.json" };
	
	//char pathTxt[256] = { "C:/Users/GengG/source/repos/cvAutoTrack/cvAutoTrack/Video/000.txt" };
	/*GetInfoLoadVideo(pathV, pathTxt);
	std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";*/
#endif

	//Sleep(1000);

	//if (init())
	//{

	//	Sleep(2000);

	//}
	//uninit();

	//Sleep(1000);


	FILE *fptr = NULL;
	fopen_s(&fptr,"./Output.txt","w+");

	while (1)
	{
#ifdef TestVideo
		//SetWorldScale(0.666667);
		if (GetInfoLoadPicture(path, uid, x2, y2, a2))
		{
			std::cout << "Now Coor and Angle: "<<" " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		//SetWorldScale(1.0);
		if (GetInfoLoadPicture(path, uid, x2, y2, a2))
		{
			std::cout << "Now Coor and Angle: " << " " << uid << " " << " " << x2 << " " << y2 << " " << a2 << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
#endif
		char buff[1024] = { 0 };
#ifdef _DEBUG
		if (GetStarJson(buff))
		{
			//坐标需要映射 p * 1.33 + AvatarPos
			std::cout << buff << "\n";
		}
#endif
#ifndef TestVideo



		// 调用获取方法
		if (GetTransform(x, y, a))
		{
			std::cout << "坐标和角度   : " << " " << x << " " << y << " " << a << "\n";
			fprintf(fptr, "%lf %lf %lf", x, y, a);
			fflush(fptr);
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		if (GetPosition(x2, y2))
		{
			std::cout << "坐标         : " << " " << x2 << " " << y2 << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		if (GetDirection(a2))
		{
			std::cout << "角度         : " << " " << a2 << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		if (GetRotation(aa2))
		{
			std::cout << "视角朝向     : " << " " << aa2 << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		if (GetUID(uid))
		{
			std::cout << "当前UID      : " << " " << uid << " " << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		if (GetStarJson(buff))
		{
			//坐标需要映射 p + AvatarPos
			std::cout << "当前神瞳Json : " << buff << "\n";
		}
		else
		{
			std::cout << "错误码       : " << " " << GetLastErr() << " " << "\n";
		}
		bool a;
		GetStar(x2,y2,a);

		Sleep(30);
		system("cls");
		//std::cout << "\r\r\r\r\r";
#endif
	}
	fclose(fptr);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
