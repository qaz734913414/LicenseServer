#include "stdafx.h"
#include "ServerNetwork.h"

FUNC_CLIENT_CONNECTED_CALLBACK ServerNetwork::ClientConnectedCallback = NULL;
FUNC_CLIENT_APPLY_LICENSE_CALLBACK ServerNetwork::ClientApplyLicenseCallback = NULL;

ServerNetwork::ServerNetwork()
{
    //int ServerNetwork::numtest = 0;
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
//Э��ͨ�����򷵻�client��MachineInfo
//Э��ʧ�ܣ��򷵻ؿ�
char* NegotiateWithClient(ISocketStream * StreamSock)
{
    cout << "-----��Socket�ѽ���, ͨ��Э����-----" << endl;

    char MachineInfo[MAX_BUFFER]{};

    //Step 1������client�Ļ�����Ϣ
    cout << "����client������Ϣ��... " << endl;
    int len = StreamSock->Recv(MachineInfo, MAX_BUFFER);
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

            //Step 1: ��֤client���
            char* client_info = NegotiateWithClient(StreamSock);
            if (!client_info)
            {
                cout << "Э��ʧ�ܣ��ر�����..." << endl;
                return;
            }

            cout << "**Э�̳ɹ�**" << endl;

            //Step 2: ���ӳɹ���ע��client��Ϣ
            ClientRegState r = ClientConnectedCallback(client_info);

            if (r == REGISTERRED)
            {
                //����client�Ѿ�ע��������������
                char RecvBuffer[MAX_BUFFER]{};
                cout << "�ȴ�client������... " << endl;
                int len = StreamSock->Recv(RecvBuffer, MAX_BUFFER);

                string strMsg = string(RecvBuffer);
                string::size_type pos = strMsg.find("Heartbeat");
                if (pos != string::npos)
                {
                    //���Ѿ�ע�����client��������Ϣ���������������ַ���"HeartbeatConfirmed"��
                    cout << "�յ�client�������� " << RecvBuffer << endl;
                    
                    string retString = "HeartbeatConfirmed";
                    cout << "��Ӧclient�������� " << retString << endl;
                    len = StreamSock->Send(retString.c_str(), retString.length());
                }
            }
            else if (r == FIRST_REGISTER)
            {
                //�Գ���ע���client��׼������license������
                char RecvBuffer[MAX_BUFFER]{};
                cout << "�ȴ�client����license... " << endl;

                int len = StreamSock->Recv(RecvBuffer, MAX_BUFFER);

                cout << "�յ�client license���� " << RecvBuffer << endl;
                string result = ClientApplyLicenseCallback(RecvBuffer);

                cout << "��Ӧclient license���� " << result << endl;
                len = StreamSock->Send(result.c_str(), result.length());
            }

            StreamSock->Disconnect();
        }
    );

    cout << "Listening, press Enter to exit.\n" << endl;
    getchar();
    Listener->EndListening();

    return 0;
}
