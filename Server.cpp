#include "stdafx.h"
#include "Server.h"

//init static members
ClientTableMap Server::ClientTable = {};
int Server::m_listening_port = 40000;
MAP_LICENSE_RESOURCE Server::m_license_resource = map<string, LicModInfo>{};

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

//��ȡӦ�ó����Ŀ¼
CString GetAppPath()
{
    TCHAR modulePath[MAX_PATH];
    int bytes = GetModuleFileName(NULL, modulePath, MAX_PATH);
    CString strModulePath(modulePath);
    strModulePath = strModulePath.Left(strModulePath.ReverseFind(_T('\\')));
    return strModulePath;
}

//��ȡlicense���ݲ����浽m_license_resource,license��ʽΪ��
//SERVER [ENCRYPTED_ID] [PORT(optional)]
//INCREMENT [MODULE_NAME] [MODULE_ID] [VERSION] [EXPIRE_DATE] [COUNT] ISSUED=[DATE] SIGN="[SIGN_INFO]"
bool ReadLicenseFile(string fullPath, MAP_LICENSE_RESOURCE* license_table)
{
    try
    {
        char buffer[READ_FILE_BUFFER_SIZE];
        fstream licFile;
        licFile.open(fullPath, ios::in);

        cout << "--- license ·��:---" << fullPath << endl;

        //----Step 1������licenseͷ��----
        cout << "��� license ͷ��: " << endl;
        licFile.getline(buffer, READ_FILE_BUFFER_SIZE, '\n');
        const char *delim = " ";    //�ָ��Ϊ�ո�
        char *p;
        int r;

        //���"SERVER"
        p = strtok(buffer, delim);
        cout << p << endl;
        r = strcmp(p, "SERVER");
        if (r != 0) return false;
        //���[ENCRYPTED_ID]
        p = strtok(NULL, delim);
        cout << p << endl;
        bool valid = Server::CheckEncryptedString(p, "shoud_be_machine_id");
        if (!valid) return false;
        //��ȡport����ѡ�
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

        //----Step 2����ȡ�����ģ����Ϣ----
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

//����������ʱ����ȡ�Ѿ������license����Ϣ
void LoadLicense(list<string>* license_list, MAP_LICENSE_RESOURCE* license_table)
{
    std::list<string>::iterator iter;
    for (iter = license_list->begin(); iter != license_list->end(); iter++)
    {
        //string XMLPath = iter->data;
        //ReadXmlFile("info.xml");
    }
    
    //ʹ����ʱ��license��ַ
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

//��UI����License�ķ���
void Server::ImportLicense()
{
    //TODO�����µ����license������ӵ�m_license_resource��
    //������license��·�����浽m_license_list
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
    //todo�� �ù�Կ�����ַ���

    return rstr;
}

bool Server::CheckEncryptedString(char * encryptedString, char * expectedString)
{
    char* rstr = ResolveString(encryptedString);
    //todo: �Ƚ������ַ�������ȷ��

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

//����client����ʱ�����ô˻ص�����ע��
ClientRegState clientConnectedCallback(char* client_info)
{
    //Э�̳ɹ�������client��Ϣ

    time_t nowtime = time(NULL); //��ȡ����ʱ��  
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

////��������Module�Ƿ���licenseʣ��
//bool CheckLicenseCount(string modName)
//{
//
//}
//����client����licenseʱ�����ô˻ص���֤������Ч��
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
