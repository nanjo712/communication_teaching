#ifndef SOCKET_H_
#define SOCKET_H_

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#endif

bool socket_init();
bool socket_cleanup();
socket_t socket_create(int type);
int socket_bind(socket_t socket, int port);
int socket_listen(socket_t socket);
socket_t socket_accept(socket_t socket);
int socket_connect(socket_t socket, const char *ip, int port);
int socket_send(socket_t socket, const char *buffer, int size);
int socket_receive(socket_t socket, char *buffer, int size);
int socket_sendto(socket_t socket, const char *buffer, int size, const char *ip,
                  int port);
int socket_recvfrom(socket_t socket, char *buffer, int size, char *ip,
                    int *port);
void socket_close(socket_t socket);

#endif /* SOCKET_H_ */