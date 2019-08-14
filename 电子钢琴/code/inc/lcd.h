#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

void draw_point(int x,int y,int color);
int Init_lcd();
void Uinit_lcd();

#endif