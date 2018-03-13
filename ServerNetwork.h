#pragma once

#include <atlconv.h>
#include <string>
#include <list>
#include <iostream>
#include <map>
#include <iomanip>
#include <memory>
#include "stdafx.h"
#include "Listener.h"
#include "ISocketStream.h"
#include "SSLHelper.h"

using namespace std;

#define MAX_BUFFER 65535
//typedef void(*FUNC_SAVE_CLIENT)(char*);
#define TIME_STRING_LEN 64

struct ClientInfo
{
    char* RegisterTime;
    char* LastConnectTime;
    list<string> TakenModule;
};

enum ClientRegState {
    FIRST_REGISTER,
    REGISTERRED
};

typedef map<string, ClientInfo> ClientTableMap;
typedef ClientRegState(*FUNC_CLIENT_CONNECTED_CALLBACK)(char*);
typedef char*(*FUNC_CLIENT_APPLY_LICENSE_CALLBACK)(char*);

class ServerNetwork
{
public:
    ServerNetwork();
    ~ServerNetwork();


//private:

    //CActiveSock * pActiveSock;
    //CSSLClient * pSSLClient;
    //CEventWrapper ShutDownEvent;


public:
    static FUNC_CLIENT_CONNECTED_CALLBACK ClientConnectedCallback;   //新的socket建立后，执行此回调
    static FUNC_CLIENT_APPLY_LICENSE_CALLBACK ClientApplyLicenseCallback;   //新的socket建立后，执行此回调

    int StartListen(int port);
    void SetRecvTimeout(int secs);
    void SetSendTimeout(int secs);
    void ConnectToServer(CString HostName, int Port);
    void SendData(CString content);
};
