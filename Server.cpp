#include "stdafx.h"
#include "Server.h"

//init static members
ClientTableMap Server::ClientTable = {};
int Server::m_listening_port = 40000;
MAP_LICENSE_RESOURCE Server::m_license_resource = map<string, LicModInfo>{};

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

//读取license内容并保存到m_license_resource,license格式为：
//SERVER [ENCRYPTED_ID] [PORT(optional)]
//INCREMENT [MODULE_NAME] [MODULE_ID] [VERSION] [EXPIRE_DATE] [COUNT] ISSUED=[DATE] SIGN="[SIGN_INFO]"
bool ReadLicenseFile(string fullPath, MAP_LICENSE_RESOURCE* license_table)
{
    try
    {
        char buffer[READ_FILE_BUFFER_SIZE];
        fstream licFile;
        licFile.open(fullPath, ios::in);

        cout << "--- license 路径:---" << fullPath << endl;

        //----Step 1：解析license头部----
        cout << "检查 license 头部: " << endl;
        licFile.getline(buffer, READ_FILE_BUFFER_SIZE, '\n');
        const char *delim = " ";    //分割符为空格
        char *p;
        int r;

        //检查"SERVER"
        p = strtok(buffer, delim);
        cout << p << endl;
        r = strcmp(p, "SERVER");
        if (r != 0) return false;
        //检查[ENCRYPTED_ID]
        p = strtok(NULL, delim);
        cout << p << endl;
        bool valid = Server::CheckEncryptedString(p, "shoud_be_machine_id");
        if (!valid) return false;
        //读取port（可选项）
        p = strtok(NULL, delim);
        if (p)
        {
            int port = atoi(p);
            if (port >= 0)
            {
                cout << port << endl;
                bool valid = Server::SetListeningPort(port);
            }
        }

        //----Step 2：读取具体的模块信息----
        while (!licFile.eof())
        {
            string modID;
            LicModInfo cinfo;
            licFile.getline(buffer, READ_FILE_BUFFER_SIZE, '\n');
            p = strtok(buffer, delim);
            if (!p) continue;
            cout << "increment: " << p << endl;

            p = strtok(NULL, delim);
            cinfo.name = p;
            if (!p) continue;
            cout << "mod name: " << p << endl;

            p = strtok(NULL, delim);
            modID = p;
            if (!p) continue;
            cout << "mod id: " << p << endl;

            p = strtok(NULL, delim);
            cinfo.version = p;
            if (!p) continue;
            cout << "ver: " << p << endl;

            p = strtok(NULL, delim);
            cinfo.expire_date = p;
            if (!p) continue;
            cout << "exp date: " << p << endl;

            p = strtok(NULL, delim);
            int t = strcmp(p, "uncounted");
            if (t == 0)
            { 
                cinfo.count = -1;
            }
            else
            {
                cinfo.count = atoi(p);
            }
            if (!p) continue;
            cout << "count: " << p << endl;

            p = strtok(NULL, delim);
            cinfo.issue_date = p;
            if (!p) continue;
            cout << "iss date: " << p << endl;

            p = strtok(NULL, delim);
            cinfo.sign = p;
            if (!p) continue;
            cout << "sign: " << p << endl;

            license_table->insert(std::pair<string, LicModInfo>(modID, cinfo));
        }
        licFile.close();
    }
    catch (string& e)
    {
        return false;
    }
    return true;
}

//服务器启动时，读取已经载入的license的信息
void LoadLicense(list<string>* license_list, MAP_LICENSE_RESOURCE* license_table)
{
    std::list<string>::iterator iter;
    for (iter = license_list->begin(); iter != license_list->end(); iter++)
    {
        //string XMLPath = iter->data;
        //ReadXmlFile("info.xml");
    }
    
    //使用临时的license地址
    CString appPath = GetAppPath();
    string seperator = "\\";
    string fullPath = string(CT2A(appPath)) + seperator + "license.lic";

    ReadLicenseFile(fullPath, license_table);
}

void Server::Init()
{
    CheckAdmin();

    LoadLicense(&m_license_list, &m_license_resource);
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

char* ResolveString(char* encryptedString)
{
    char rstr[LICENSE_INFO_BUFFER_SIZE]{};
    //todo： 用公钥解析字符串

    return rstr;
}

bool Server::CheckEncryptedString(char * encryptedString, char * expectedString)
{
    char* rstr = ResolveString(encryptedString);
    //todo: 比较两个字符串的正确性

    return true;
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

////检查请求的Module是否有license剩余
//bool CheckLicenseCount(string modName)
//{
//
//}
//当有client申请license时，调用此回调验证申请有效性
string clientApplyLicenseCallback(char* apply_msg)
{
    //string substring = apply_msg;
    string retValue;
    char* substring = strtok(apply_msg, LIC_APPLY_STR_DELIMITER);
    do
    {
        if (!substring) continue;

        //substring = substring.substr(0, substring.find(LIC_APPLY_STR_DELIMITER));
        if (Server::m_license_resource[substring].count > 0)
        {
            Server::m_license_resource[substring].count--;
            retValue += "1";
        }
        else if (Server::m_license_resource[substring].count == -1)
        {
            retValue += "1";
        }
        else
        {
            retValue += "0";
        }
       
        substring = strtok(NULL, LIC_APPLY_STR_DELIMITER);
    } while (substring && strlen(substring) >= 0);

    return retValue;
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
