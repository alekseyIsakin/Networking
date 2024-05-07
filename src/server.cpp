#include "server.hpp"

void Server::Init()
{
#ifdef _WIN32
    WSADATA wsaData;
    int IResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (IResult != 0)
    {
        lastError = IResult;
    }
#endif

    return;
}

int Server::BindListener(std::string host, std::string port)
{
    addrinfo hints;
    std::stringstream ss;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int IResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &listenerAddr);
    if (IResult != 0)
    {
        ss << "get addr failed " << IResult << std::endl;
        perror(ss.str().c_str());
        return IResult;
    }

    listen_fd = socket(listenerAddr->ai_family, listenerAddr->ai_socktype, listenerAddr->ai_protocol);
    if (listen_fd == INVALID_SOCKET)
    {
        perror("create socket failed");
        return -1;
    }

    IResult = bind(listen_fd, listenerAddr->ai_addr, listenerAddr->ai_addrlen);
    if (IResult != 0)
    {
        ss << "bind socket failed " << IResult << std::endl;
        perror(ss.str().c_str());
        return IResult;
    }

    freeaddrinfo(listenerAddr);
    return 0;
}

int Server::StartListening()
{
    std::stringstream ss;
    int IResult = listen(listen_fd, 5);
    if (IResult != 0)
    {
        ss << "socket listening failed " << IResult << std::endl;
        perror(ss.str().c_str());
        return IResult;
    }

    socklen_t addr_size;
    SOCKET new_fd = accept(listen_fd, nullptr, nullptr);
    if (new_fd == INVALID_SOCKET)
    {
        ss << "accept connetion failed " << IResult << std::endl;
        perror(ss.str().c_str());
        return IResult;
    }

    return 0;
}

void Server::Cleanup()
{
#ifdef _WIN32
    WSACleanup();
#endif
    freeaddrinfo(listenerAddr);
}