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

void Server::StartServer(std::string host, std::string Port)
{
    if (BindListener(host, Port) != 0)
    {
        errorStream << "StartServer failed " << std::endl;
        perror(errorStream.str().c_str());
        Cleanup();
        exit(1);
    }

    StartListening();
}

int Server::BindListener(std::string host, std::string port)
{
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int IResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &listenerAddr);
    if (IResult != 0)
    {
        errorStream << "get addr failed " << IResult << std::endl;
        perror(errorStream.str().c_str());
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
        errorStream << "bind socket failed " << IResult << std::endl;
        perror(errorStream.str().c_str());
        return IResult;
    }

    freeaddrinfo(listenerAddr);
    return 0;
}

int Server::StartListening()
{
    std::stringstream error;
    ThreadListener tl = ThreadListener();

    int IResult = listen(listen_fd, 5);
    if (IResult != 0)
    {
        error << "socket listening failed " << IResult << std::endl;
        perror(error.str().c_str());
        return IResult;
    }
    const int MAX_THREADS = 3;
    int last_thread = 0;

    DWORD dwThreadId[MAX_THREADS];
    HANDLE hThreads[MAX_THREADS];
    socklen_t addr_size;

    SOCKET new_fd = accept(listen_fd, nullptr, nullptr);
    if (new_fd == INVALID_SOCKET)
    {
        error << "accept connetion failed " << IResult << std::endl;
        perror(error.str().c_str());
        return IResult;
    }

    hThreads[last_thread] = CreateThread(
        NULL, 0, StartConnectListenerWinSock,
        &new_fd, 0, &dwThreadId[last_thread]);

    // tl.StartConnectListener(new_fd);

    return 0;
}

void Server::Cleanup()
{
#ifdef _WIN32
    WSACleanup();
#endif
    freeaddrinfo(listenerAddr);
}

const int BUFF_SIZE = 8192;

DWORD WINAPI StartConnectListenerWinSock(LPVOID lpParam)
{
    HANDLE hStdout;
    SOCKET socket;

    TCHAR msgBuff[BUFF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    socket = *((SOCKET*)lpParam);

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    while (true)
    {
        memset(msgBuff, 0, sizeof(char) * BUFF_SIZE);
        int result = recv(socket, msgBuff, BUFF_SIZE, 0);

        StringCchLength(msgBuff, BUFF_SIZE, &cchStringSize);
        WriteConsole(hStdout, msgBuff, (DWORD)cchStringSize, &dwChars, NULL);
    }
}