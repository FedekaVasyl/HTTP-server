#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <Windows.h>
  #include <process.h>
  #include <conio.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <pthread.h>
#endif

#undef INVALID_SOCKET
#define INVALID_SOCKET -1
#undef SOCKET_ERROR
#define SOCKET_ERROR -1

#pragma comment(lib,"Ws2_32.lib")

extern char ip[10];

void SockExit(int _socket);
#ifdef _WIN32
  bool SockInit(void);
#else
  bool SetParam(int _socket);
#endif

#endif // SOCKET_H
