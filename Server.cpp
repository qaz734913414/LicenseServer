#include "stdafx.h"
#include "Server.h"

ClientTableMap Server::ClientTable = {};

void AddClient(int client_id)
{

}

//�������Ƿ��Թ���ԱȨ������
bool CheckAdmin()
{
    if (!IsUserAdmin())
    {
        cout << "WARNING: The server is not running as an administrator." << endl;
        return false;
    }
    return true;
}

//����������ʱ����ȡ�Ѿ������license����Ϣ
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

//����client����ʱ�����ô˻ص�����ע��
ClientRegState clientConnectedCallback(char* client_info)
{
    //Э�̳ɹ�������client��Ϣ

    time_t nowtime = time(NULL); //��ȡ����ʱ��  
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
        cout << "client(" << client_info << ") ��ע��, ע��ʱ��Ϊ��" << (itor->second).RegisterTime << endl;
        cout << "�������ʱ��Ϊ��" << (itor->second).LastConnectTime << endl;
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

//����client����licenseʱ�����ô˻ص���֤������Ч��
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
