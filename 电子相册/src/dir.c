#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <dirent.h>

List* search_dir(List* list,char*pathname)
{
	//step1:打开目录
	DIR* dirp = opendir(pathname);
	if(dirp == NULL)
	{	
		perror("opendir error:");
		return NULL;
	}
	//step2:读取目录项
	struct dirent *dirent = NULL;
	while(dirent = readdir(dirp))
	{
		if(strcmp(dirent->d_name,".") == 0 ||strcmp(dirent->d_name,"..") == 0)
		{
			continue;
		}
		char buf[257];
		sprintf(buf,"%s/%s",pathname,dirent->d_name);
		struct stat st;
		stat(buf,&st);
		if(S_ISREG(st.st_mode))
		{
			
			if(strcmp(buf+strlen(buf)-4,".bmp") == 0)
			{
				add_a_node(list,buf);
			}	
			else if(strcmp(buf+strlen(buf)-4,".jpg") == 0)
			{
				add_a_node(list,buf);
			}
		}
		else if(S_ISDIR(st.st_mode))
		{
			search_dir(list,buf);
		}
	}
	
	//step3.关闭目录
	closedir(dirp);
	
	return list;
}