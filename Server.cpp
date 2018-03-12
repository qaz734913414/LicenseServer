#include "stdafx.h"
#include "Server.h"

ClientTableMap Server::ClientTable = {};

void AddClient(int client_id)
{

}

//检查程序是否以管理员权限运行
bool CheckAdmin()
{
    if (!IsUserAdmin())
    {
        cout << "WARNING: The server is not running as an administrator." << endl;
        return false;
    }
    return true;
}

//服务器启动时，读取已经载入的license的信息
void LoadLicense(list<string> license_list, map<string, int> license_table)
{
    std::list<string>::iterator iter;
    for (iter = license_list.begin(); iter != license_list.end(); iter++)
    {
         //TODO
        
    }
}

void Server::Init()
{
    CheckAdmin();

    LoadLicense(m_license_list, m_license_resource);
}

int Server::SetListeningPort(int port)
{
    m_listening_port = port;
    return 0;
}

ClientTableMap Server::GetClientTable()
{
    return ClientTable;
}

void OnClientConnected(int id)
{

}

bool Server::StartService()
{
    m_network.StartListen(m_listening_port);
    return false;
}

char* ConvertTimeFormat(char* rs, int num)
{
    if (num < 10)
    {
        sprintf(rs, "0%d", num);
    }
    else if(10 <= num <=99)
    {
        sprintf(rs, "%d", num);
    }
    return rs;
}

//当有client连入时，调用此回调进行注册
ClientRegState clientConnectedCallback(char* client_info)
{
    //协商成功，保存client信息

    time_t nowtime = time(NULL); //获取日历时间  
    char strtime[TIME_STRING_LEN];
    tm *ltm = localtime(&nowtime);
    char mon[3];
    char day[3];
    char hour[3];
    char min[3];
    char sec[3];
    ConvertTimeFormat(mon, ltm->tm_mon + 1);
    ConvertTimeFormat(day, ltm->tm_mday);
    ConvertTimeFormat(hour, ltm->tm_hour);
    ConvertTimeFormat(min, ltm->tm_min);
    ConvertTimeFormat(sec, ltm->tm_sec);
    sprintf_s(strtime, "%4d/%2s/%2s,%2s:%2s:%2s", ltm->tm_year + 1900, mon, day, hour, min, sec);

    ClientTableMap::iterator itor = Server::ClientTable.find(client_info);
    if (itor != Server::ClientTable.end())
    {
        (itor->second).LastConnectTime = strtime;
        sprintf((itor->second).LastConnectTime, "%s", strtime);
        cout << "client(" << client_info << ") 已注册, 注册时间为：" << (itor->second).RegisterTime << endl;
        cout << "最近连接时间为：" << (itor->second).LastConnectTime << endl;
        return REGISTERRED;
    }
    else
    {
        ClientInfo info = {};
        info.RegisterTime = (char*)malloc(TIME_STRING_LEN * sizeof(char));
        info.LastConnectTime = (char*)malloc(TIME_STRING_LEN * sizeof(char));

        sprintf(info.RegisterTime, "%s", strtime);
        sprintf(info.LastConnectTime, "%s", strtime);
        Server::ClientTable[string(client_info)] = info;
        return FIRST_REGISTER;
    }
}

//当有client申请license时，调用此回调验证申请有效性
char* clientApplyLicenseCallback(char* apply_msg)
{
    return "1/0";
}

Server::Server(int listening_port)
{
    m_listening_port = listening_port;
    ServerNetwork::ClientConnectedCallback = (FUNC_CLIENT_CONNECTED_CALLBACK)&clientConnectedCallback;
    ServerNetwork::ClientApplyLicenseCallback = (FUNC_CLIENT_APPLY_LICENSE_CALLBACK)&clientApplyLicenseCallback;
}


Server::~Server()
{
}
