#include "socket.h"
#include "client.h"
#include "log.h"

bool AcceptClient(int server_socket)
{
  /* Accept incoming connections */
  const int kArgSize = 5;
  /*Instead of creating a global structure with two fields,
   *I create an array of type int where the first 4 elements of the array
   * - the value of each digit IP address of the client, and 5 element
   * - socket client. Then I pass this array into a stream as an
   * argument to the function.*/
  int *arguments;
  int client_socket = INVALID_SOCKET;
  char *client_ip;
  struct sockaddr_in client_addr;
#ifdef _WIN32
  HANDLE client_handle;
  int client_addr_size = sizeof(sockaddr);
#else
  socklen_t client_addr_size = sizeof(sockaddr);
  int err = 0;
  pthread_t client_thread;
#endif
#ifdef _WIN32
  if (!SockInit()) {
    return false;
  }
#endif
  arguments = (int*)malloc(kArgSize * sizeof(arguments));
  memset(arguments,0,kArgSize * sizeof(arguments));
  client_socket = accept(server_socket,\
                        (sockaddr*)&client_addr,\
                         &client_addr_size);
  if (client_socket == INVALID_SOCKET) {
    SetLoggingData("ERROR", "Error! The function accept() failed!", ip);
    SockExit(server_socket);
    return false;
  }
  client_ip = inet_ntoa(client_addr.sin_addr);
#ifdef _WIN32
  arguments[0] = (int)client_addr.sin_addr.S_un.S_un_b.s_b1;
  arguments[1] = (int)client_addr.sin_addr.S_un.S_un_b.s_b2;
  arguments[2] = (int)client_addr.sin_addr.S_un.S_un_b.s_b3;
  arguments[3] = (int)client_addr.sin_addr.S_un.S_un_b.s_b4;
#else
  FindArguments(client_ip,arguments);
#endif
  arguments[4] = client_socket;
  SetLoggingData("INFO", "Connected a new client", client_ip);
#ifdef _WIN32
  client_handle = (HANDLE)_beginthreadex(0,0,&Receive,(void*)arguments,0,0);
#else
  err = pthread_create(&client_thread,0,&Receive,(void*)arguments);
  if (err != 0) {
    printf("\ncan't create thread :[%s]\n", strerror(err));
  }
#endif
  return true;
}

#ifdef _WIN32
  unsigned int __stdcall Receive(void* arguments)
#else
  void* Receive(void* arguments)
#endif
{
  int request_result = 0;
  /*IP address:   xxx.xxx.xxx.xxx = 15*/
  const int kRequestSize = 1024, kIPLen = 15;
  char client_request[kRequestSize];
  int client_socket = ((int*)arguments)[4];
  char client_ip[kIPLen];
  /* Receive the data */
  request_result = recv(client_socket, client_request, kRequestSize, 0);
  /* Form a response */
  FindClientIP(client_ip,arguments);
  CorrectsRequest(client_request);
  Response(request_result, client_ip, client_request, client_socket);
  free(arguments);
#ifdef _WIN32
  _endthreadex(0);
  return 1;
#else
  pthread_exit((void*)1);
  return (void*)1;
#endif
}

void Response(const int &request_result, char* client_ip,\
              char* client_request, const int &client_socket)
{
  if (request_result == SOCKET_ERROR) {
    /* Error receiving data */
    SetLoggingData("ERROR", "Error! The function recv() failed!", ip);
    SockExit(client_socket);
  } else if (request_result == 0) {
      /* Сonnection is closed by the client */
      SetLoggingData("INFO", "Connection closed!", client_ip);
  } else if (request_result > 0) {
      SetLoggingData("INFO", client_request, client_ip);
      if (strstr(client_request, ".jpg")) {
        if (!SendResponse(IMAGE, client_request, client_socket)) {
          SetLoggingData("WARNING", "Warning! Unable to load the picture!",\
                         ip);
          return;
        }
      } else if (strstr(client_request,".css")){
          if (!SendResponse(TEXT, client_request, client_socket)) {
            SetLoggingData("WARNING", "Warning! Can not display css!", ip);
          }
        } else {
            if (!SendResponse(TEXT, client_request, client_socket)) {
              SetLoggingData("WARNING", "Warning! Can not display html-page!",\
                             ip);
            }
          }
  }
}

bool SendResponse(int response, char* client_request, const int &client_socket)
{
  FILE *page;
  int result = 0;
  const int kBufferSize = 1024, kPageNameSize = 128;
  char page_name[kPageNameSize], buffer[kBufferSize], *body;
  const char kPageNotFound[] = "404NotFound.html";
#ifdef _WIN32
  const char kDirectory[] = "../../HttpServer/IncludedFiles/";
#else
  const char kDirectory[] = "../HttpServer/IncludedFiles/";
#endif
  if (!FindName(page_name, client_request)) {
    return false;
  }
  memset(buffer, '\0', kBufferSize);
  if (response == TEXT) {
    page = fopen(page_name,"r");
  } else {
      page = fopen(page_name,"rb");
  }
  if (page == NULL) {
    if (strstr(page_name,".html")) {
      strcpy(page_name,kDirectory);
      strcat(page_name,kPageNotFound);
      page = fopen(page_name,"r");
      if (page == NULL) {
        SetLoggingData("ERROR", "Error! Can not open file!", ip);
        return false;
      }
    } else {
        SetLoggingData("ERROR", "Error! Can not open file!", ip);
        return false;
    }
  }
  body = (char*)calloc(kBufferSize * 10,sizeof(body));
  if (body == NULL) {
    SetLoggingData("ERROR", "Error! Unable to allocate memory!", ip);
    return false;
  }
  if (response == IMAGE) {
    strcat(body,"HTTP/1.1 200 OK Version: HTTP/1.1\
           Content-Type: text/jpeg;\r\n\r\n");
  }
  while(IS_DATA) {
      if (!fgets(buffer,kBufferSize,page)) {
        break;
      }
      strcat(body,buffer);
  }
  result = send(client_socket,body,strlen(body) + 1,0);
  if (result == SOCKET_ERROR) {
    SetLoggingData("ERROR", "Error! The function send() failed!", ip);
    return false;
  }
  SockExit(client_socket);
  fclose(page);
  free(body);
  return true;
}

/*Find the name of the page that should be loaded*/
bool FindName(char* name, char* client_request)
{
  int i = 0;
  const int kPageNameSize = 128, kBufferLen = 200;
  char buffer[kBufferLen], *begin_ptr, *end_ptr;
  const char kInitialPage[] = "InitialPage.html";
  const char kBadRequest[] = "400BadRequest.html";
  const char kNotImplemented[] = "501NotImplemented.html";
#ifdef _WIN32
  const char kDirectory[] = "../../HttpServer/IncludedFiles/";
#else
  const char kDirectory[] = "../HttpServer/IncludedFiles/";
#endif
  memset(buffer,'\0',kBufferLen);
  memset(name, '\0', kPageNameSize);
  begin_ptr = strstr(client_request, "GET /");
  end_ptr = strstr(client_request, "HTTP/");
  if (end_ptr == NULL) {
    strcpy(name,kBadRequest);
    return true;
  }
  if (begin_ptr == NULL) {
    strcpy(name,kNotImplemented);
    return true;
  }
  begin_ptr = client_request + sizeof("GET /") - 1;
  while(begin_ptr != end_ptr) {
      name[i] = *begin_ptr;
      begin_ptr++;
      i++;
  }
  if (strstr(name,".html") == NULL && \
      strstr(name,".css") == NULL && \
      strstr(name,".jpg") == NULL && \
      name[0] != ' ') {
    SetLoggingData("ERROR", "Error! Unknown file type", ip);
    return false;
  }
  if (name[0] == ' ') {
    strcpy(name,kInitialPage);
    strcpy(buffer,kDirectory);
    strcat(buffer,name);
    strcpy(name,buffer);
  } else {
      name[i-1] = '\0';
      strcpy(buffer,kDirectory);
      strcat(buffer,name);
      strcpy(name,buffer);
  }
  return true;
}

void CorrectsRequest(char* client_request)
{
  char *begin_ptr = client_request;
  while((begin_ptr = strstr(begin_ptr,"\r\n")) != NULL) {
      *begin_ptr = ' ';
      begin_ptr++;
      *begin_ptr = ' ';
  }
}

#ifndef _WIN32
/*Form arguments from string (client_ip)*/
void FindArguments(char *string, int* arguments)
{
  int zero_code = 48;
  int i = 0, j = 0, digit = 0;
  int ip_len = strlen(string);
  for(; i < ip_len; i++) {
    if (string[i] == '.') {
      arguments[j] = digit;
      j++;
      digit = 0;
      continue;
    }
    if (digit > 0) {
      digit *= 10;
    }
    digit += string[i] - zero_code;
  }
}
#endif
/*Form client_ip from arguments*/
void FindClientIP(char *client_ip, void *arguments)
{
  int digit = 0;
#ifdef _WIN32
  const int kBufLen = 3;
#else
  const int kBufLen = 4;
#endif
  char buf[kBufLen];
  memset(client_ip,'\0',kBufLen);
  memset(buf,'\0',kBufLen);
  digit = ((int*)arguments)[0];
  sprintf(buf,"%d",digit);//itoa(((int*)arguments)[0],buf,10);
  strcat(client_ip,buf);
  strcat(client_ip,".");
  memset(buf,'\0',kBufLen);
  digit = ((int*)arguments)[1];
  sprintf(buf,"%d",digit);//itoa(((int*)arguments)[1],buf,10);
  strcat(client_ip,buf);
  strcat(client_ip,".");
  memset(buf,'\0',kBufLen);
  digit = ((int*)arguments)[2];
  sprintf(buf,"%d",digit);//itoa(((int*)arguments)[2],buf,10);
  strcat(client_ip,buf);
  strcat(client_ip,".");
  memset(buf,'\0',kBufLen);
  digit = ((int*)arguments)[3];
  sprintf(buf,"%d",digit);//itoa(((int*)arguments)[3],buf,10);
  strcat(client_ip,buf);
}
