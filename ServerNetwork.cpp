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


//Server��Client���ӽ�����ִ�����µ�Э����Ϊ
char* NegotiateWithClient(ISocketStream * StreamSock)
{
    cout << "-----��Socket�ѽ���, ͨ��Э����-----" << endl;

    char* MachineInfo = new char[MAX_BUFFER];

    //Step 1������client�Ļ�����Ϣ
    cout << "����client������Ϣ��... " << endl;
    int len = StreamSock->Recv(MachineInfo, sizeof(MachineInfo) - 1);
    if (len > 0)
    {
        MachineInfo[len] = '\0'; // Terminate the string, for convenience
        cout << "�յ���Ϣ�� " << MachineInfo << endl;
    }
    else
    {
        cout << "δ�յ���Ϣ���˳�... " << endl;
        return false;
    }
   
    //Step 2����client����ȷ����Ϣ
    string hello_msg = string("Hello from server");
    cout << "����ȷ����Ϣ��" << hello_msg << endl;
    len = StreamSock->Send(hello_msg.c_str(), hello_msg.length());
    if (len < 0)
    {
        cout << "���ʹ����˳�... " << endl;
        return false;
    }

    return MachineInfo;
}

static int s_client_id = 0;
//��ʼ�����˿�
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
            /* ÿ�ν���Socket���ӣ������������ */
            char* client_info = NegotiateWithClient(StreamSock);
            if (!client_info)
            {
                cout << "Э��ʧ�ܣ��ر�����..." << endl;
            }
            else
            {
                //FuncCallback(client_info);
                ////Э�̳ɹ�������client��Ϣ
                //ClientTableMap::iterator itor = ClientTable->find(client_info);
                //if (itor != ClientTable->end())
                //{
                //    cout << "client(" << client_info << ") ��ע��, ע��ʱ��Ϊ��" << ((ClientInfo)(itor->second)).RegisterTime << endl;
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
