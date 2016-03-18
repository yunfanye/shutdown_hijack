// This file contains utility functions for writing to the log file specified
// by the command line input for lisod server.
// Log information includes: 
// 1. IP addresses and browser information/settings as coming from connected clients.
// 2. All errors encountered to the log file.

// include library needed for vfprintf functions
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"

#define LOG_BUF_SIZE 4096

// This function returns -1 if fails opening file
int open_log_file(char* filename){
	log_file_fd = fopen(filename, "w+");
	if(log_file_fd==NULL){
		printf("open file error! %s\n", strerror(errno));
		return -1;
	}
	return 1;
}

int close_log_file(char* filename){
	if(log_file_fd!=NULL){
		fclose(log_file_fd);
	}
	return 1;
}

// This function writes a message with format and variable length arguments
// The usage is like printf(format, arguments).
void log_info(char* format, ...){
	if(log_file_fd!=NULL) {
		va_list arguments;
		va_start(arguments, format);
		vfprintf(log_file_fd, format, arguments);
		va_end(arguments);
		fflush(log_file_fd);
	}
}

int log_error(char *error_str)
{
	int retval = -errno;
	char buf[LOG_BUF_SIZE];
	if(errno != ENODATA) {
		snprintf(buf, LOG_BUF_SIZE, "FS Error: %s, %s\n", 
			error_str, strerror(errno));
		log_info(buf);
	}
	/* FUSE always returns -errno to caller (yes, it is negative errno!) */
	return retval;
}