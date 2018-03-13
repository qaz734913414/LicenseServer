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

//获取应用程序根目录
CString GetAppPath()
{
    TCHAR modulePath[MAX_PATH];
    int bytes = GetModuleFileName(NULL, modulePath, MAX_PATH);
    CString strModulePath(modulePath);
    strModulePath = strModulePath.Left(strModulePath.ReverseFind(_T('\\')));
    return strModulePath;
}

//读取XML内容并保存到m_license_resource
bool ReadXmlFile(string szFileName)
{//读取Xml文件，并遍历
    try
    {
        CString appPath = GetAppPath();
        string seperator = "\\";
        string fullPath = string(CT2A(appPath)) + seperator + szFileName;
        
        //创建一个XML的文档对象。
        TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
        myDocument->LoadFile();
        //获得根元素，即Persons。
        TiXmlElement *RootElement = myDocument->RootElement();
        //输出根元素名称，即输出Persons。
        cout << RootElement->Value() << endl;
        //获得第一个Person节点。
        TiXmlElement *FirstPerson = RootElement->FirstChildElement();
        //获得第一个Person的name节点和age节点和ID属性。
        TiXmlElement *NameElement = FirstPerson->FirstChildElement();
        TiXmlElement *AgeElement = NameElement->NextSiblingElement();
        TiXmlAttribute *IDAttribute = FirstPerson->FirstAttribute();
        //输出第一个Person的name内容，即周星星；age内容，即；ID属性，即。
        cout << NameElement->FirstChild()->Value() << endl;
        cout << AgeElement->FirstChild()->Value() << endl;
        cout << IDAttribute->Value() << endl;
    }
    catch (string& e)
    {
        return false;
    }
    return true;
}

//服务器启动时，读取已经载入的license的信息
void LoadLicense(list<string> license_list, MAP_LICENSE_RESOURCE license_table)
{
    std::list<string>::iterator iter;
    for (iter = license_list.begin(); iter != license_list.end(); iter++)
    {
        //string XMLPath = iter->data;
        //ReadXmlFile("info.xml");
    }
    ReadXmlFile("info.xml");
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

//从UI导入License的方法
void Server::ImportLicense()
{
    //TODO：将新导入的license内容添加到m_license_resource，
    //并将新license的路径保存到m_license_list
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
    char strtime[TIME_STRING_LEN]{};
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
