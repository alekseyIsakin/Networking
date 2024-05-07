#ifndef SERVER_HPP
#define SERVER_HPP

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
#endif // WIN_32


class Server {
private:
    int listen_fd;
    int lastError;
    addrinfo *listenerAddr;
public:
    void Init();
    int BindListener(std::string host, std::string Port);
    void Cleanup();
    int StartListening();
};

#endif