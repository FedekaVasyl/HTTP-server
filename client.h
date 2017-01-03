#ifndef CLIENT_H
#define CLIENT_H

#define IS_DATA 1
#undef TEXT
#define TEXT 1
#define IMAGE 2

bool AcceptClient(int server_socket);
void Response(const int &request_result, char* client_ip,\
              char* client_request, const int &client_socket);
bool SendResponse(int response, char* client_request,\
                  const int &client_socket);
bool FindName(char* name, char* client_request);
void PageNotFound(void);
#ifdef _WIN32
  unsigned int __stdcall Receive(void* arguments);
#else
  void* Receive(void* arguments);
  void FindArguments(char *string, int* arguments);
#endif
void FindClientIP(char* client_ip, void* arguments);
void CorrectsRequest(char* client_request);

#endif // CLIENT_H
