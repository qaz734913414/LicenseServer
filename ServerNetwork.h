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
using FUN_CLIENT_CONNECTED = void(*)(int);

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
    //std::function<void(int client_id)> OnClientConnected;
    //OnReceiveClientRequest(ISocketStream * const StreamSock);
    void OnReceiveClientRequest(ISocketStream * const StreamSock);
    //std::function<void(ISocketStream*StreamSock)> OnReceiveClientRequest(ISocketStream * const StreamSock);

    //int StartListen(int port, FUN_CLIENT_CONNECTED func_callback);
    int StartListen(int port);

    //void SetRecvTimeout(int secs);
    //void SetSendTimeout(int secs);
    //void ConnectToServer(CString HostName, int Port);
    //void SendData(CString content);
};
