#ifndef SERVER_H
#define SERVER_H

extern volatile int server_socket;

bool ServerStartup(void);
bool Bind(struct addrinfo* server_addr);
bool Listen(void);
void StopServer(void);

#endif // SERVER_H


