#include "socket.h"
#include "log.h"

char ip[] = "127.0.0.1";

#ifndef _WIN32
bool SetParam(int _socket)
{
  bool opt_value = false;
  socklen_t size_opt_value = sizeof(socklen_t);
  int result = 0;
  result = setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR, \
                  (char*)&opt_value,size_opt_value);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! The function setsockopt() failed!", ip);
    return false;
  }
  result = setsockopt(_socket,SOL_SOCKET,SO_REUSEPORT,  \
                      (char*)&opt_value,size_opt_value);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! The function setsockopt() failed!", ip);
    return false;
  }
  return true;
}
#endif

#ifdef _WIN32
bool SockInit(void)
{
  WSADATA wsaData;
  int result = 0;
  result = WSAStartup(MAKEWORD(2, 2), &wsaData);	/* Initialize Winsock */
  if (result != 0) {
    SetLoggingData("ERROR", "Error! The function WSAStartup() failed!", ip);
    return false;
  }
  return true;
}
#endif

void SockExit(int _socket)
{
  int result = 0;
#ifdef _WIN32
  result = closesocket(_socket);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! Can not close the socket!", ip);
    return;
  }
  result = WSACleanup();
  if (!result) {
    SetLoggingData("ERROR", "Error! The function WSACleanup() failed!", ip);
    return;
  }
#else
  result = shutdown(_socket,2);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! Can not close the socket!", ip);
    return;
  }
  SetParam(_socket);
#endif
}
