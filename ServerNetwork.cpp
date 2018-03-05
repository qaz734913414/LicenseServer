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



//void ServerNetwork::OnReceiveClientRequest(ISocketStream * const StreamSock)
//{
//    OnClientConnected(s_client_id++);
//    // This is the code to be executed each time a socket is opened
//    char MsgText[MAX_BUFFER]; // Because the simple text messages we exchange are char not wchar
//
//                              //
//    cout << "A connection has been made, worker started, sending hello" << endl;
//    CString hello_msg = CString("Hello from server");
//    StreamSock->Send(hello_msg, hello_msg.GetLength());
//
//    int len = StreamSock->Recv(MsgText, sizeof(MsgText) - 1);
//    if (len > 0)
//    {
//        MsgText[len] = '\0'; // Terminate the string, for convenience
//        cout << "Received " << MsgText << endl;
//        cout << "Sending goodbye from server" << endl;
//        StreamSock->Send("Goodbye from server", 19);
//    }
//    else
//        cout << "No response data received " << endl;
//    cout << "Exiting worker" << endl << endl;
//}

static int s_client_id = 0;
//��ʼ�����˿�
int ServerNetwork::StartListen(int port)
{
    unique_ptr<CListener> Listener(new CListener());
    Listener->SelectServerCert = SelectServerCert;
    Listener->ClientCertAcceptable = ClientCertAcceptable;
    Listener->Initialize(port);

    cout << "Starting to listen on port " << port << ", will find certificate for first connection." << endl;

    //Listener->BeginListening(std::function<OnReceiveClientRequest(ISocketStream * const StreamSock)>);
    Listener->BeginListening([](ISocketStream * const StreamSock)
    {
        OnClientConnected(s_client_id++);
        // This is the code to be executed each time a socket is opened
        char MsgText[MAX_BUFFER]; // Because the simple text messages we exchange are char not wchar

        //
        cout << "A connection has been made, worker started, sending hello" << endl;
        CString hello_msg = CString("Hello from server");
        StreamSock->Send(hello_msg, hello_msg.GetLength());

        int len = StreamSock->Recv(MsgText, sizeof(MsgText) - 1);
        if (len > 0)
        {
            MsgText[len] = '\0'; // Terminate the string, for convenience
            cout << "Received " << MsgText << endl;
            cout << "Sending goodbye from server" << endl;
            StreamSock->Send("Goodbye from server", 19);
        }
        else
            cout << "No response data received " << endl;
        cout << "Exiting worker" << endl << endl;
    });

    cout << "Listening, press Enter to exit.\n" << endl;
    getchar();
    Listener->EndListening();

    return 0;
}
