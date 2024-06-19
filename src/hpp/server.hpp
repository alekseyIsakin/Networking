#ifndef SERVER_HPP
#define SERVER_HPP

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#endif // WIN_32

DWORD WINAPI StartConnectListener(LPVOID lpParam);

class Server
{
private:
    std::stringstream errorStream;
    int listen_fd;
    int lastError;
    addrinfo *listenerAddr;

    int BindListener(std::string host, std::string Port);
    void Cleanup();
    int StartListening();

public:
    void Init();
    void StartServer(std::string host, std::string Port);
};

DWORD WINAPI StartConnectListenerWinSock(LPVOID lpParam);


class ThreadListener
{
public:
    void StartConnectListener(SOCKET socket);
};

#endif