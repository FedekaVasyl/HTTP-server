#include "socket.h"
#include "server.h"
#include "log.h"

int volatile server_socket;

bool ServerStartup(void)
{
  int result = 0;
  struct addrinfo addr_template;
  struct addrinfo* server_addr = NULL;
#ifdef _WIN32
  const PCSTR kHost = "127.0.0.1";
  const PCSTR kPort = "8000";
#else
  const char kHost[] = "127.0.0.1";
  const char kPort[] = "8000";
#endif
#ifdef _WIN32
  if (!SockInit()) {
    return false;
  }
#endif
  memset(&addr_template,0,sizeof(addr_template));
  addr_template.ai_family = AF_INET;
  addr_template.ai_socktype = SOCK_STREAM;
  addr_template.ai_protocol = IPPROTO_TCP;
  addr_template.ai_flags = AI_PASSIVE;
  result = getaddrinfo(kHost, kPort, &addr_template, &server_addr);
  if (result != 0) {
    SetLoggingData("ERROR", "Error! The function getaddrinfo() failed!", ip);
    SockExit(server_socket);
    return false;
  }
  /* Create the socket */
  server_socket = socket(server_addr->ai_family,      \
                         server_addr->ai_socktype,    \
                         server_addr->ai_protocol);
  if (server_socket == INVALID_SOCKET) {
    SetLoggingData("ERROR", "Error! The function socket() failed!", ip);
    freeaddrinfo(server_addr);
    SockExit(server_socket);
    return false;
  }
#ifndef _WIN32
  if (!SetParam(server_socket)) {
    return false;
  }
#endif
  if (!Bind(server_addr) || !Listen()) {
    return false;
  }
  printf("The server is started.................\n");
  SetLoggingData("INFO", "The server is started", ip);
  return true;
}

/* Bind the socket to the IP-address */
bool Bind(struct addrinfo* server_addr)
{
  int result = 0;
  result = bind(server_socket, server_addr->ai_addr, \
      (int)server_addr->ai_addrlen);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! The function bind() failed!", ip);
    freeaddrinfo(server_addr);
    StopServer();
    return false;
  }
  return true;
}

/* Initialize the listening socket */
bool Listen(void)
{
  int result = 0;
  result = listen(server_socket, SOMAXCONN);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! The function listen() failed!", ip);
    StopServer();
    return false;
  }
  return true;
}

void StopServer(void)
{
  SockExit(server_socket);
}
