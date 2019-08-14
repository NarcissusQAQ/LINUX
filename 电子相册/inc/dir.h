#ifndef _DIR_H_
#define _DIR_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <dirent.h>
struct list*search_dir(struct list*list,char*pathname);

#endif