#pragma once
#include "Core/AutoTrackCore.h"

class AutoTrackManagement {
	AutoTrackCore ATC;
public:
	AutoTrackManagement(void);
	~AutoTrackManagement(void);
	
	/// <summary>
	/// �����־û����񣬺�̨���������㣬�Լ�ʱ��ȡ����״̬����
	/// </summary>
	/// <returns></returns>
	bool startServe();
	/// <summary>
	/// ֹͣ�־û����񣬵������û�ȡ����״̬����
	/// </summary>
	/// <returns></returns>
	bool stopServe();

private:
	/// <summary>
	/// �־û�����
	/// </summary>
	void server();

public:
	/// <summary>
	/// ��ʼ�����ִ���Դ
	/// </summary>
	/// <returns>�Ƿ�ɹ�</returns>
	bool init();
	bool uninit();
	bool SetHandle(long long int handle);
	bool SetWorldCenter(double x, double y);
	bool SetWorldScale(double scale);
	
	/// <summary>
	/// ��ȡ��ɫ��ǰ����ͽǶ�
	/// </summary>
	/// <param name="x">��ɫ������������ϵ�µ�X����</param>
	/// <param name="y">��ɫ������������ϵ�µ�Y����</param>
	/// <param name="a">��ɫ������ĽǶ�</param>
	/// <returns>�Ƿ�ɹ���ȡ</returns>
	bool GetTransform(double &x, double&y, double&a);

	/// <summary>
	/// ��ȡ��ɫ��ǰ����
	/// </summary>
	/// <param name="x">��ɫ������������ϵ�µ�X����</param>
	/// <param name="y">��ɫ������������ϵ�µ�Y����</param>
	/// <returns>�Ƿ�ɹ���ȡ</returns>
	bool GetPosition(double &x, double &y);
	bool GetDirection(double &a);
	bool GetRotation(double &a);
	//��ȡ���ֵ���ͫ����,isEndΪ����Ϊ��ǰ�����е����һ����ͫ
	bool GetStar(double &x, double &y, bool &isEnd);
	//��ȡ���ֵ���ͫ���꣬��json�ַ�����ʽ
	bool GetStarJson(char *jsonBuff);
	bool GetUID(int &uid);
	bool GetInfoLoadPicture(char* path, int &uid, double &x, double &y, double &a);
	bool GetInfoLoadVideo(char * path, char * pathOutFile);

	/// <summary>
	/// �������һ��������
	/// </summary>
	/// <returns>������</returns>
	int GetLastError();
	/// <summary>
	/// ���ش�������˵��
	/// </summary>
	/// <returns>�������Ӧ����˵��</returns>
	const char* GetLastErrorStr();
};
