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
    int count;              //值小于0时，表示不限数量
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
    static int m_listening_port;                //server监听的端口
	ServerNetwork m_network;			        //网络连接管理
	list<string> m_license_list;		        //保存导入的所有license文件名

public:
    void Init();                        //检查服务器运行权限，载入license
	void ImportLicense();               //导入License的方法
	bool StartService();                //启动服务
	bool StopService();                 //停止服务
    ClientTableMap GetClientTable();

    static MAP_LICENSE_RESOURCE m_license_resource;	//服务器读取的license的内容
    static ClientTableMap ClientTable;              //client注册表
    static int SetListeningPort(int port);          //设置服务器监听的端口
    static bool CheckEncryptedString(char*, char*); //检查签名的有效性
};
