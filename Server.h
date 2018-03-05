#pragma once

#include <string>
#include <list>
#include <iostream>
#include <map>
#include "ServerNetwork.h"
using namespace std;

#define MAX_MODULENAME_LEN 100

//struct ModuleCount
//{
//    char ModuleName[MAX_MODULENAME_LEN];
//    int Count;
//};

class Server
{
public:
	Server(int);
	~Server();

private:
    int m_listening_port;
	ServerNetwork m_network;			    //网络连接管理
	char* m_data_buffer;				    //消息缓存
	list<string> m_license_list;		    //保存导入的所有license文件名
	map<string, int> m_license_resource;	//服务器读取的license的内容

    void ReceiveFromServer();		//接收消息的方法
	bool VerifyClient();			//验证客户端的身份
	bool VerifyLicense();			//验证License的有效性

public:
    void Init();
    int SetListeningPort(int port); //设置服务器监听的端口

	void ImportLicense();			//导入License的方法
	bool StartService();			//启动服务
	bool StopService();				//停止服务


};

