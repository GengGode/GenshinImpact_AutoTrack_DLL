#include "pch.h"
#include "GetUIDClass.h"

GetUIDClass::GetUIDClass(const Logger& loggerService)
{
	logger = loggerService;

	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactWnd, 101);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactRect, 102);
	//wForAfter.append(this, &AutoTrackCore::getGengshinImpactScreen, 103);
}

bool GetUIDClass::get(int& uid)
{
	if (wForAfter.run() == false)
	{
		err = 300;
		return false;
	}
	if (getUIDRefMat() == false)
	{
		return false;
	}

	std::vector<cv::Mat> channels;

	split(giUIDRef, channels);
	giUIDRef = channels[3];

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

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//Ñ°ÕÒ×î¼ÑÆ¥ÅäÎ»ÖÃ
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
				//Ñ°ÕÒ×î¼ÑÆ¥ÅäÎ»ÖÃ
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
		err = 8;//Î´ÄÜÔÚUIDÇøÓò¼ì²âµ½ÓÐÐ§UID
		return false;
	}
	uid = _uid;
	err = 0;
	return true;
}
