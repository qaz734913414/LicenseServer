#include "stdafx.h"
#include "Server.h"


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
    return m_client_table;
}

void OnClientConnected(int id)
{

}

bool Server::StartService()
{
    //m_network.StartListen(m_listening_port, OnClientConnected);
    m_network.StartListen(m_listening_port);
    return false;
}


void serverCallback(char* text)
{

}
Server::Server(int listening_port)
{
    m_listening_port = listening_port;
    //m_network.FuncSaveClient = &SaveClientInfo;
    m_network.FuncCallback = (TestCallback)&serverCallback;
}


Server::~Server()
{
}
