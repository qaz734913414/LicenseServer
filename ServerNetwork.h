#pragma once

#include <atlconv.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "stdafx.h"
#include "Listener.h"
#include "ISocketStream.h"
#include "SSLHelper.h"

using namespace std;

#define MAX_BUFFER 65535
//typedef void(*FUN_CLIENT_CONNECTED)(int);

class ServerNetwork
{
public:
    ServerNetwork();
    ~ServerNetwork();


private:

    //CActiveSock * pActiveSock;
    //CSSLClient * pSSLClient;
    //CEventWrapper ShutDownEvent;


public:
    //static FUN_CLIENT_CONNECTED OnClientConnected;
    std::function<void(int client_id)> OnClientConnected;
    void OnReceiveClientRequest(ISocketStream * const StreamSock);
    int StartListen(int port);

    //void SetRecvTimeout(int secs);
    //void SetSendTimeout(int secs);
    //void ConnectToServer(CString HostName, int Port);
    //void SendData(CString content);
};
