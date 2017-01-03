#ifndef LOG_H
#define LOG_H

#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <conio.h>
#else
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <fcntl.h>
  #include <errno.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ENABLE 1
#define DISABLE 0

/*Declare two global variables*/
extern int logging_status;
extern char *log_file_path;

#ifdef _WIN32
bool SetLogFilePath(const char kPath[] = \
      "../../HttpServer/IncludedFiles/log.txt");
#else*/
  bool SetLogFilePath(const char kPath[] = \
        "../HttpServer/IncludedFiles/log.txt");
#endif
bool SetLoggingStatus(int status);
void PathFree(void);
bool SetLoggingData(const char* kState,\
                    const char* kMessage,\
                    char* ip);
void FindDate(struct tm *timeinfo, char *_date);
void FindTime(struct tm *timeinfo, char *_time);


#endif // LOG_H
