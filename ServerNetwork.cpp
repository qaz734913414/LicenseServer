#include "stdafx.h"
#include "ServerNetwork.h"


ServerNetwork::ServerNetwork()
{
}


ServerNetwork::~ServerNetwork()
{
}

CString GetCertName(PCCERT_CONTEXT pCertContext)
{
    CString certName;
    auto good = CertGetNameString(pCertContext, CERT_NAME_FRIENDLY_DISPLAY_TYPE, 0, NULL, certName.GetBuffer(128), certName.GetAllocLength() - 1);
    certName.ReleaseBuffer();
    if (good)
        return certName;
    else
        return L"<unknown>";
}

SECURITY_STATUS SelectServerCert(PCCERT_CONTEXT & pCertContext, LPCTSTR pszSubjectName)
{
    SECURITY_STATUS status;
    status = CertFindServerBySignature(pCertContext,
        "a9 f4 6e bf 4e 1d 6d 67 2d 2b 39 14 ee ee 58 97 d1 d7 e9 d0", true);  // "true" looks in user store, "false", or nothing looks in machine store
    if (!pCertContext) // If we don't already have a certificate, try and select a likely looking one
        status = CertFindServerByName(pCertContext, pszSubjectName); // Add "true" to look in user store, "false", or nothing looks in machine store
    if (pCertContext)
        wcout << "Server certificate requested for " << pszSubjectName << ", found \"" << (LPCWSTR)GetCertName(pCertContext) << "\"" << endl;
    return status;
}

bool ClientCertAcceptable(PCCERT_CONTEXT pCertContext, const bool trusted)
{
    if (trusted)
        cout << "A trusted";
    else
        cout << "An untrusted";
    wcout << " client certificate was returned for \"" << (LPCWSTR)GetCertName(pCertContext) << "\"" << endl;
    return NULL != pCertContext; // Meaning any certificate is fine, trusted or not, but there must be one
}


//Server与Client连接建立后，执行以下的协商行为
char* NegotiateWithClient(ISocketStream * StreamSock)
{
    cout << "-----新Socket已建立, 通信协商中-----" << endl;

    char* MachineInfo = new char[MAX_BUFFER];

    //Step 1：接收client的机器信息
    cout << "接收client机器信息中... " << endl;
    int len = StreamSock->Recv(MachineInfo, sizeof(MachineInfo) - 1);
    if (len > 0)
    {
        MachineInfo[len] = '\0'; // Terminate the string, for convenience
        cout << "收到消息： " << MachineInfo << endl;
    }
    else
    {
        cout << "未收到消息，退出... " << endl;
        return false;
    }
   
    //Step 2：向client返回确认信息
    string hello_msg = string("Hello from server");
    cout << "返回确认消息：" << hello_msg << endl;
    len = StreamSock->Send(hello_msg.c_str(), hello_msg.length());
    if (len < 0)
    {
        cout << "发送错误，退出... " << endl;
        return false;
    }

    return MachineInfo;
}

static int s_client_id = 0;
//开始监听端口
int ServerNetwork::StartListen(int port)
{
    unique_ptr<CListener>Listener(new CListener());
    Listener->SelectServerCert = SelectServerCert;
    Listener->ClientCertAcceptable = ClientCertAcceptable;
    Listener->Initialize(port);

    cout << "Starting to listen on port " << port << ", will find certificate for first connection." << endl;

    Listener->BeginListening
    (
        [](ISocketStream * const StreamSock)
        {
            /* 每次建立Socket连接，都会进入这里 */
            char* client_info = NegotiateWithClient(StreamSock);
            if (!client_info)
            {
                cout << "协商失败，关闭连接..." << endl;
            }
            else
            {
                //FuncCallback(client_info);
                ////协商成功，保存client信息
                //ClientTableMap::iterator itor = ClientTable->find(client_info);
                //if (itor != ClientTable->end())
                //{
                //    cout << "client(" << client_info << ") 已注册, 注册时间为：" << ((ClientInfo)(itor->second)).RegisterTime << endl;
                //}
                //else
                //{
                //    ClientInfo info;
                //    strcpy_s(info.RegisterTime, "2018/03/6, 17:30");
                //    strcpy_s(info.LastConnectTime, "2018/03/6, 18:30");
                //    (*ClientTable)[string(client_info)] = info;
                //}
            }
        }
    );

    cout << "Listening, press Enter to exit.\n" << endl;
    getchar();
    Listener->EndListening();

    return 0;
}
