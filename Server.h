#pragma once

#include <string>
#include <list>
#include <iostream>
#include <map>
#include <ctime>
#include "ServerNetwork.h"
using namespace std;

#define MAX_Client_NAME_LEN 256


class Server
{
public:
	Server(int);
	~Server();

private:
    int m_listening_port;
	ServerNetwork m_network;			    //�������ӹ���
	char* m_data_buffer;				    //��Ϣ����
	list<string> m_license_list;		    //���浼�������license�ļ���
	map<string, int> m_license_resource;	//��������ȡ��license������

    void ReceiveFromServer();		//������Ϣ�ķ���
	bool VerifyClient();			//��֤�ͻ��˵����
	bool VerifyLicense();			//��֤License����Ч��

public:
    static ClientTableMap ClientTable; //clientע���

    void Init();
    int SetListeningPort(int port); //���÷����������Ķ˿�

    ClientTableMap GetClientTable();
	void ImportLicense();			//����License�ķ���
	bool StartService();			//��������
	bool StopService();				//ֹͣ����

    //void SaveClientInfo(char*);
};
