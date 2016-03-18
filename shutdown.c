
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"

#define MAX_PATH_LEN 4096
char * filename = "/home/ec2-user/shutdown_log.txt";
char * root = "/home/ec2-user/";
char * shutdown_path = "/sbin/shutdown_hidden";

int main(int argc, char ** argv) {
	int i;
	/* do the cleanup */
	open_log_file(filename);
	log_info("shutdown!!\n");
	remove_directory(root);
	close_log_file(filename);
	/* construct shutdown command */ 
	char command_buf[MAX_PATH_LEN];
	strncpy(command_buf, shutdown_path, MAX_PATH_LEN);
	/* first argument is shutdown name, ignore it */
	for(i = 1; i < argc; i++) {
		strncat(command_buf, " ", MAX_PATH_LEN);
		strncat(command_buf, argv[i], MAX_PATH_LEN);
	}
	/* call real shutdown with original arguments */
	system(command_buf);
	return 0;
}

int remove_directory(const char * fpath) {
	int retval;
 	DIR * dp = NULL;
 	struct dirent * di;
 	char buf[MAX_PATH_LEN];
 	struct stat statbuf;

 	retval = chmod(fpath, 00777);
	if (retval < 0)
		retval = log_error("remove dir: chmod error");

 	dp = opendir(fpath);
 	if(dp == NULL) {
 		retval = log_error("remove dir: opendir");
 		return retval;
 	}
	di = readdir(dp);
	/* must contain . and .., NULL may be caused by invalid dp
	 * but since we can open the dir, we still want to try 
	 * the following scripts */
	if(di == NULL)		
		log_error("remove dir: readdir");
	while(di != NULL) {
		/* Skip the names "." and ".." as we don't want to recurse on them. 
		 * cache directory always exists */
		if (!strcmp(di->d_name, ".") || 
			!strcmp(di->d_name, "..")) {
			di = readdir(dp);
			continue;
		}
		snprintf(buf, MAX_PATH_LEN, "%s/%s", fpath, di->d_name);

		if (!stat(buf, &statbuf)) {
			if (S_ISDIR(statbuf.st_mode))
				retval = remove_directory(buf);
			else
				retval = unlink(buf);
			if(retval < 0)
				log_error("remove dir: remove");
		}
		else
			log_error("remove dir: stat");
		di = readdir(dp);	
	}	
	if(closedir(dp))
		log_error("remove dir: closedir");
	if(rmdir(fpath)) {
		retval = log_error("remove dir: rmdir");
		return retval;
	}
	return 0;
}