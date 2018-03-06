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
typedef void(*FUNC_SAVE_CLIENT)(char*);
//using FUNC_CLIENT_CONNECTED = void(*)(int);
#define MAX_TIME_LEN 16

struct ClientInfo
{
    char RegisterTime[MAX_TIME_LEN];
    char LastConnectTime[MAX_TIME_LEN];
    list<string> TakenModule;
};

typedef map<string, ClientInfo> ClientTableMap;


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
    //static ClientTableMap* ClientTable;
    ClientTableMap* ClientTable;
    int StartListen(int port);
    static int ttest;

    void SetRecvTimeout(int secs);
    void SetSendTimeout(int secs);
    void ConnectToServer(CString HostName, int Port);
    void SendData(CString content);
};
