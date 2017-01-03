#include <QCoreApplication>
#include "socket.h"
#include "client.h"
#include "server.h"
#include "log.h"

#define SERVER_WORKS   1
#define SERVER_STOPS   0

#define START_SERVER   1
#define STOP_SERVER    2
#define ENABLE_LOG     3
#define DISABLE_LOG    4
#define CHANGE_PATH    5
#define QUIT           6

#ifdef _WIN32
  unsigned int __stdcall StartAccepting(void*)
#else
  void* StartAccepting(void*)
#endif
{
  int status = SERVER_WORKS;
  while (status) {
    if (!AcceptClient(server_socket)) {
      status = SERVER_STOPS;
    }
  }
  return NULL;
}

bool ConsoleMenu(void)
{
  int option = 0;
  int status = SERVER_WORKS;
  const int kPathLen = 1024;
  char path[kPathLen];
#ifdef _WIN32
  HANDLE accept_handle;
#else
  int err = 0;
  pthread_t accept_thread;
#endif
  memset(path,'\0',kPathLen);
  while (status) {
    printf("Server options:\n");
    printf("| 1 - Start server | 2 - Stop server | "
           "3 - Enable logging | 4 - Disable logging | "
           "5 - Change log file path | "
           "6 - Quit |\n");
    printf("Select option:\t");
    scanf("%d",&option);
    switch(option) {
      case START_SERVER: {
        if (!ServerStartup()) {
          printf("Error! The function ServerStartup() failed!\n");
          break;
        }
#ifdef _WIN32
        accept_handle = (HANDLE)_beginthreadex(0,0,&StartAccepting,\
                                               (void*)0,0,0);
#else
        err = pthread_create(&accept_thread,NULL,&StartAccepting,NULL);
        if (err != 0) {
          printf("\ncan't create thread :[%s]\n", strerror(err));
        }
#endif
        break;
      }
      case STOP_SERVER: {
        StopServer();
        printf("Server stopped.................\n");
        break;
      }
      case ENABLE_LOG: {
        SetLoggingStatus(ENABLE);
        printf("Logging enabled.................\n");
        break;
      }
      case DISABLE_LOG: {
        SetLoggingStatus(DISABLE);
        printf("Logging disabled.................\n");
        break;
      }
      case CHANGE_PATH: {
        PathFree();
        if (!SetLoggingStatus(DISABLE)) {
          break;
        }
        printf("Enter path to file:\n");
        scanf("\n%s",&path);
        if (!SetLogFilePath(path)) {
          printf("Error! Can not set log file path!\n");
          break;
        }
        memset(path,'\0',kPathLen);
        if (!SetLoggingStatus(ENABLE)) {
          break;
        }
        printf("Path changed.................\n");
        break;
      }
      case QUIT: {
#ifdef _WIN32
        CloseHandle(accept_handle);
#endif
        StopServer();
        printf("Quit (Press any key).................\n");
        status = SERVER_STOPS;
        break;
      }
      default: {
        printf("Error! Wrong command!\n");
        break;
      }
    }
  }
  return true;
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  if (!SetLogFilePath()) {
    printf("Error! Can not set log file path!\n");
    return false;
  }
  if (!ConsoleMenu()) {
    StopServer();
  }
  a.exit();
}
