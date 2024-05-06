#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#endif // WIN_32

#include "iostream"

#define DEFAULT_PORT "3000"

int main(int argc, char* argv[])
{
	struct addrinfo* result = nullptr, * ptr = nullptr, * p, hints;
	struct sockaddr_storage their_addr;

	char ipstr[INET6_ADDRSTRLEN];
	int IResult;

#ifdef _WIN32
	WSADATA wsaData;
	IResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	if (IResult != 0) {
		std::cout << "socket failed " << IResult << std::endl;
	}

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	IResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (IResult != 0) {
		std::cout << "get socket failed " << IResult << std::endl;
		WSACleanup();
		exit(1);
	}
	else {
		std::cout << "socket allocated" << std::endl;
	}

	for (p = result; p != nullptr; p = p->ai_next) {
		void* addr;
		std::string ipver;

		if (p->ai_family == AF_INET) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else {
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		std::cout << ipver << " " << ipstr << std::endl;
	}

	int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (sockfd == -1) {
		std::cout << "get socket fd failed " << IResult << std::endl;
		WSACleanup();
		exit(1);
	}

	IResult = bind(sockfd, result->ai_addr, result->ai_addrlen);
	if (IResult != 0) {
		std::cout << "Bind socked failed " << IResult << std::endl;
		WSACleanup();
		exit(1);
	}

	IResult = listen(sockfd, 5);
	if (IResult != 0) {
		std::cout << "Listen socked failed " << IResult << std::endl;
		WSACleanup();
		closesocket(sockfd);
		exit(1);
	}

	socklen_t addr_size;
	//int newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
	int newfd = accept(sockfd, nullptr, nullptr);
	if (newfd == -1) {
		std::cout << "accept socked failed " << newfd << std::endl;
		WSACleanup();
		exit(1);
	}

	const int recbuffLength = 10;
	int readed = 1;
	char* recbuff = new char[recbuffLength];
	while (readed > 0) {
		memset(recbuff, 0, sizeof(char) * recbuffLength);
		std::string msg = "Hello world";
		int len, bytes_send;

		len = msg.length();
		bytes_send = send(newfd, msg.c_str(), len, 0);

		readed = recv(newfd, recbuff, recbuffLength, 0);
		for (int i = 0; i < readed; i++) {
			std::cout << (int)recbuff[i] << ", ";
		}
		std::cout << std::endl;
		for (int i = 0; i < readed; i++) {
			std::cout << recbuff[i];
		}
		std::cout << std::endl;
		std::cout << readed << std::endl;
	}
	delete recbuff;

	freeaddrinfo(result);
}