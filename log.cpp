#include "log.h"

/*Definition two global variables*/
int logging_status = ENABLE;
char *log_file_path = NULL;

bool SetLogFilePath(const char *kPath)
{
  int path_length = strlen(kPath);
  log_file_path = (char*)calloc(path_length,sizeof(kPath));
  strcpy(log_file_path,kPath);
  if (log_file_path == NULL) {
    printf("Error! Unable to allocate memory!\n");
    return false;
  }
  return true;
}

bool SetLoggingStatus(int status)
{
  if (status == ENABLE) {
    logging_status = ENABLE;
  } else if (status == DISABLE) {
      logging_status = DISABLE;
  } else {
      printf("Error! Invalid status value!\n");
      return false;
  }
  return true;
}

void PathFree(void)
{
  free(log_file_path);
}

bool ShowLog(void)
{
  FILE *file;
  const int kBufferSize = 2048;
  char buffer[kBufferSize];
  file = fopen(log_file_path,"rb");
  if (file == NULL) {
    printf("Error! Can not open file!\n");
    return false;
  }
  printf("State\tDate\tTime\tIP Address\t\tMessage\n");
  do
  {
    memset(buffer,'\0',kBufferSize);
  } while (!fgets(buffer,kBufferSize,file));
  fclose(file);
  return true;
}

bool SetLoggingData(const char* kState, const char* kMessage, char* ip)
{
  time_t raw_time;
  struct tm *timeinfo;
  char* string;
  const int kDateLen = 11, kTimeLen = 9, kStringLen = 1024;
  char new_date[kDateLen], new_time[kTimeLen];
#ifdef _WIN32
  FILE* file;
#else
  int file;
  int byte_count = 0;
#endif
  if (logging_status == DISABLE) {
    return true;
  }
  memset(new_date,'\0',kDateLen);
  memset(new_time,'\0',kTimeLen);
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  string = (char*)calloc(kStringLen,sizeof(string));
  if (string == NULL) {
    printf("Error! Unable to allocate memory!\n");
    return false;
  }
  memset(string,'\0',kStringLen);
  strcpy(string,kState);
  strcat(string,"\t");
  FindDate(timeinfo,new_date);
  strcat(string,new_date);
  strcat(string,"\t");
  FindTime(timeinfo,new_time);
  strcat(string,new_time);
  strcat(string,"\t");
  strcat(string,ip);
  strcat(string,"\t\t");
  strcat(string,kMessage);
  strcat(string,"\r\n");
  strcat(string,"\0");
#ifdef _WIN32
  file = fopen(log_file_path,"a+");
  if (file == NULL) {
    printf("Error! Can not open file!\n");
    free(string);
    return false;
  }
  if (fputs(string,file)) {
    printf("Error! Can not write data to file!\n");
    fclose(file);
    free(string);
    return false;
  }
  fclose(file);
#else
  file = open(log_file_path,O_WRONLY | O_APPEND,S_IRWXU | S_IWUSR | S_IRWXO);
  if (file == NULL) {
    free(string);
    printf("Error! Can not open file!\n");
    return false;
  }
  byte_count = strlen(string);
  if (!write(file,string,byte_count)) {
    printf("Error! Can not write data to file!\n");
    close(file);
    free(string);
    return false;
  }
  close(file);
#endif
  free(string);
  return true;
}

void FindDate(struct tm *timeinfo, char *_date)
{
  int day = timeinfo->tm_mday;
  int month = timeinfo->tm_mon + 1;       /*Corrects month value*/
  int year = timeinfo->tm_year + 1900;    /*Corrects year value*/
#ifdef _WIN32
  const int kBufferLen = 5, kBase = 10;
#else
  const int kBufferLen = 5, kBase = 10;
#endif
  char buffer[kBufferLen];
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",day);//itoa(day,buffer,base);
  if (day < kBase){
    strcpy(_date,"0");
    strcat(_date,buffer);
  } else {
      strcpy(_date,buffer);
  }
  strcat(_date,"/");
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",month);//itoa(month,buffer,base);
  if (month < kBase){
    strcat(_date,"0");
    strcat(_date,buffer);
  } else {
      strcat(_date,buffer);
  }
  strcat(_date,"/");
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",year);//itoa(year,buffer,base);
  strcat(_date,buffer);
}

void FindTime(struct tm *timeinfo, char *_time)
{
  int seconds = timeinfo->tm_sec;
  int minutes = timeinfo->tm_min;
  int hours = timeinfo->tm_hour;
#ifdef _WIN32
  int const kBufferLen = 2, kBase = 10;
#else
  int const kBufferLen = 3, kBase = 10;
#endif
  char buffer[kBufferLen];
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",hours);
  if (hours < kBase){
    strcpy(_time,"0");
    strcat(_time,buffer);
  } else {
      strcpy(_time,buffer);
  }
  strcat(_time,":");
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",minutes);
  if (minutes < kBase) {
    strcat(_time,"0");
    strcat(_time,buffer);
  } else {
      strcat(_time,buffer);
  }
  strcat(_time,":");
  memset(buffer,'\0',kBufferLen);
  sprintf(buffer,"%d",seconds);
  if (seconds < kBase) {
    strcat(_time,"0");
    strcat(_time,buffer);
  } else {
      strcat(_time,buffer);
  }
}
