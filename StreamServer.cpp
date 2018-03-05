#include "stdafx.h"
#include "Listener.h"
#include "ISocketStream.h"
#include "SSLHelper.h"
#include "Server.h"
#include <memory>

using namespace std;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	const int Port = 41000;
	
    Server server(Port);
    server.Init();
    server.StartService();

	return 0;
}
