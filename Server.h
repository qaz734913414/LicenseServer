#pragma once

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <ctime>
#include <fstream>
#include "ServerNetwork.h"
#include "tinyxml.h"

using namespace std;

#define MAX_CLIENT_NAME_LEN 256
#define READ_FILE_BUFFER_SIZE 512
#define LICENSE_INFO_BUFFER_SIZE 512
#define PUB_LICKEY "tempPubKey"
#define LIC_APPLY_STR_DELIMITER "/"

struct LicModInfo
{
    string name;
    string version;
    string expire_date;
    int count;              //ֵС��0ʱ����ʾ��������
    string issue_date;
    string sign;
};
typedef map<string, LicModInfo> MAP_LICENSE_RESOURCE;

class Server
{
public:
	Server(int);
	~Server();

private:
    static int m_listening_port;                //server�����Ķ˿�
	ServerNetwork m_network;			        //�������ӹ���
	list<string> m_license_list;		        //���浼�������license�ļ���

public:
    void Init();                        //������������Ȩ�ޣ�����license
	void ImportLicense();               //����License�ķ���
	bool StartService();                //��������
	bool StopService();                 //ֹͣ����
    ClientTableMap GetClientTable();

    static MAP_LICENSE_RESOURCE m_license_resource;	//��������ȡ��license������
    static ClientTableMap ClientTable;              //clientע���
    static int SetListeningPort(int port);          //���÷����������Ķ˿�
    static bool CheckEncryptedString(char*, char*); //���ǩ������Ч��
};
