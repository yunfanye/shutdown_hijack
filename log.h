#ifndef __LOG__
#define __LOG__

#include <stdio.h>

FILE* log_file_fd;

int open_log_file(char* filename);
int close_log_file(char* filename);
void log_info(char* format, ...);
int log_error(char *error_str);

#endif