#include "native_system_handle/Socket.h"

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

bool socket_init()
{
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
#else
    return true;
#endif
}

bool socket_cleanup()
{
#ifdef _WIN32
    return WSACleanup() == 0;
#else
    return true;
#endif
}

socket_t socket_create(int type) { return socket(AF_INET, type, 0); }

int socket_bind(socket_t socket, int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return bind(socket, (struct sockaddr *)&addr, sizeof(addr));
}

int socket_listen(socket_t socket) { return listen(socket, SOMAXCONN); }

socket_t socket_accept(socket_t socket) { return accept(socket, NULL, NULL); }

int socket_connect(socket_t socket, const char *ip, int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons(port);
    return connect(socket, (struct sockaddr *)&addr, sizeof(addr));
}

int socket_send(socket_t socket, const char *buffer, int size)
{
    return send(socket, buffer, size, 0);
}

int socket_receive(socket_t socket, char *buffer, int size)
{
    return recv(socket, buffer, size, 0);
}

int socket_sendto(socket_t socket, const char *buffer, int size, const char *ip,
                  int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons(port);
    return sendto(socket, buffer, size, 0, (struct sockaddr *)&addr,
                  sizeof(addr));
}

int socket_recvfrom(socket_t socket, char *buffer, int size, char *ip,
                    int *port)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int ret =
        recvfrom(socket, buffer, size, 0, (struct sockaddr *)&addr, &addr_len);
    if (ret >= 0)
    {
        *port = ntohs(addr.sin_port);
        inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    }
    return ret;
}

void socket_close(socket_t socket)
{
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}