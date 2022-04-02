#include "pch.h"
#include "GetInfoLoadPictureClass.h"

GetInfoLoadPictureClass::GetInfoLoadPictureClass(const Logger& loggerService)
{
	logger = loggerService;
}

bool GetInfoLoadPictureClass::get(char* path, int& uid, double& x, double& y, double& a)
{
	std::string pathStr(path);

	giFrame = cv::imread(pathStr);

	if (giFrame.empty())
	{
		err = 201;
		return false;
	}

	getPaimonRefMat();

	cv::Mat paimonTemplate;

	cv::resize(giMatchResource.PaimonTemplate, paimonTemplate, giPaimonRef.size());

	cv::Mat tmp;
	//	giPaimonRef = giFrame(cv::Rect(0, 0, cvCeil(giFrame.cols / 20), cvCeil(giFrame.rows / 10)));
	//
	//#ifdef _DEBUG
	//	cv::namedWindow("test", cv::WINDOW_FREERATIO);
	//	cv::imshow("test", giPaimonRef);
	//#endif

	if (giPaimonRef.channels() != 4)
	{
		cv::cvtColor(paimonTemplate, paimonTemplate, cv::COLOR_RGBA2GRAY);
		cv::cvtColor(giPaimonRef, giPaimonRef, cv::COLOR_RGB2GRAY);
	}

	cv::matchTemplate(paimonTemplate, giPaimonRef, tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	cv::namedWindow("test2", cv::WINDOW_FREERATIO);
	cv::imshow("test2", tmp);
	std::cout << "Paimon Match: " << minVal << "," << maxVal << std::endl;
#endif
	if (maxVal < 0.34 || maxVal == 1)
	{
		err = 202;
		return false;
	}

	getMiniMapRefMat();

	cv::Mat img_scene(giMatchResource.MapTemplate);
	cv::Mat img_object(giMiniMapRef(cv::Rect(30, 30, giMiniMapRef.cols - 60, giMiniMapRef.rows - 60)));

	cv::cvtColor(img_scene, img_scene, cv::COLOR_RGBA2RGB);

	if (img_object.empty())
	{
		err = 203;
		return false;
	}

	getAvatarRefMat();

	if (giAvatarRef.empty())
	{
		err = 204;//未能取到小箭头区域
		return false;
	}

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	cv::Mat gray0;
	cv::Mat gray1;
	cv::Mat gray2;

	cv::threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	cv::threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	cv::threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);

	cv::Mat and12;
	cv::bitwise_and(gray1, gray2, and12);// , gray0);
	cv::resize(and12, and12, cv::Size(), 2, 2, 3);
	cv::Canny(and12, and12, 20, 3 * 20, 3);
	cv::circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);
	cv::Mat dilate_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(and12, and12, dilate_element);
	cv::Mat erode_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	cv::findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		err = 205;//提取小箭头特征误差过大
		//return false;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
			AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
		}

		AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);

		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
		}
		if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
			AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
		}
		if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
		{
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
			AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
		}

		AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
		KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];

		a = Line2Angle(KeyLine);
	}



	if (getUIDRefMat() == false)
	{
		err = 206;
		//return false;
	}
	else
	{
		std::vector<cv::Mat> channels;

		split(giUIDRef, channels);
		giUIDRef = channels[0];

		int _uid = 0;
		int _NumBit[9] = { 0 };

		int bitCount = 1;
		cv::Mat matchTmp;
		cv::Mat Roi;
		cv::Mat checkUID = giMatchResource.UID;

#ifdef _DEBUG
		//if (checkUID.rows > Roi.rows)
		//{
		//	cv::resize(checkUID, checkUID, cv::Size(), Roi.rows/ checkUID.rows);
		//}
#endif
		giUIDRef.copyTo(Roi);

		if (checkUID.rows > Roi.rows)
		{
			cv::resize(Roi, Roi, cv::Size(cvRound(1.0 * checkUID.rows / Roi.rows * Roi.cols), checkUID.rows), 0);
		}

		cv::matchTemplate(Roi, checkUID, matchTmp, cv::TM_CCOEFF_NORMED);

		minVal = 0;
		maxVal = 0;
		minLoc = cv::Point(0, 0);
		maxLoc = cv::Point(0, 0);
		//寻找最佳匹配位置
		cv::minMaxLoc(matchTmp, &minVal, &maxVal, &minLoc, &maxLoc);
		if (maxVal > 0.75)
		{
			int x = maxLoc.x + checkUID.cols + 7;
			int y = 0;
			double tmplis[10] = { 0 };
			int tmplisx[10] = { 0 };
			for (int p = 8; p >= 0; p--)
			{
				_NumBit[p] = 0;
				for (int i = 0; i < 10; i++)
				{
					cv::Rect r(x, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);//180-46/9->140/9->16->16*9=90+54=144
					if (x + r.width > giUIDRef.cols)
					{
						r = cv::Rect(giUIDRef.cols - giMatchResource.UIDnumber[i].cols - 2, y, giMatchResource.UIDnumber[i].cols + 2, giUIDRef.rows);
					}

					cv::Mat numCheckUID = giMatchResource.UIDnumber[i];
					Roi = giUIDRef(r);

					cv::matchTemplate(Roi, numCheckUID, matchTmp, cv::TM_CCOEFF_NORMED);

					double minVali, maxVali;
					cv::Point minLoci, maxLoci;
					//寻找最佳匹配位置
					cv::minMaxLoc(matchTmp, &minVali, &maxVali, &minLoci, &maxLoci);

					tmplis[i] = maxVali;
					tmplisx[i] = maxLoci.x + numCheckUID.cols - 1;
					if (maxVali > 0.91)
					{
						_NumBit[p] = i;
						x = x + maxLoci.x + numCheckUID.cols - 1;
						break;
					}
					if (i == 10 - 1)
					{
						_NumBit[p] = getMaxID(tmplis, 10);
						x = x + tmplisx[_NumBit[p]];
					}
				}
			}
		}

		_uid = 0;
		for (int i = 0; i < 9; i++)
		{
			_uid += _NumBit[i] * bitCount;
			bitCount = bitCount * 10;
		}
		if (_uid == 0)
		{
			err = 207;//未能在UID区域检测到有效UID
			//return false;
		}
		uid = _uid;
	}




	if (!is_init_end)
	{
		init();//初始化
	}
	if (img_object.empty())
	{
		err = 208;//原神小地图区域为空或者区域长宽小于60px
		return false;
	}
	bool retflag;
	bool retval = test.get_value1(img_scene, img_object, x, y, retflag);
	if (retflag) return retval;

	err = 0;
	return true;


}
