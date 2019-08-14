#include "jpg_display.h"

/*
	jpg_display：显示一张jpg图片
*/
int jpg_display(char*jpg_name)
{
	//1.分配并初始化一个jpeg解压对象
	struct jpeg_decompress_struct dinfo; //声明一个解压的对象
	
	struct jpeg_error_mgr jerr; //声明一个出错信息的对象
	
	dinfo.err = jpeg_std_error(&jerr); //初始化这个出错对象
	
	jpeg_create_decompress(&dinfo); //初始化dinfo这个解压对象

	//2. 指定要解压缩的图像文件
		FILE *fp;
		fp = fopen(jpg_name, "r");
		if (fp == NULL)
		{
			perror("fopen error:");
			return -1;
		}
		
		jpeg_stdio_src(&dinfo,fp); //指定要解压的图像文件
				
				
				
	//3.调用jpeg_read_header()获取图像信息
	
	  jpeg_read_header(&dinfo, TRUE);
		
	
	
	//4.	用于设置jpeg解压对象dinfo的一些参数。可采用默认参数



	//5. 调用jpeg_start_decompress()启动解压过程
		jpeg_start_decompress(&dinfo);

	//6. 读取一行或者多行扫描线上数据并处理，通常的代码是这样子的:

	unsigned char *buffer = malloc(dinfo.output_width * dinfo.output_components);
	
		//dinfo.output_scanline , 表示的意思是，已经扫描了多少行
		int x;
		while (dinfo.output_scanline < dinfo.output_height)
		{
			x = dinfo.output_scanline;	
			jpeg_read_scanlines(&dinfo,  //解压对象
								&buffer,//保存解压后数据的二级指针, 
								1 //读取多少行数据来解压
								);	
								//dinfo.output_scanline + 1	
			unsigned char a,r,g,b;
			unsigned char*p = buffer;
			int y;
			for(y = 0;y < dinfo.output_width;y++)
			{
				if(dinfo.output_components == 3)
				{
					a = 0;
				}
				else
				{
					a = *(p++);
				}
				r = *(p++);
				g = *(p++);
				b = *(p++);
				int color = a << 24 | r << 16 |g << 8| b;
				
				draw_point(x,y,color);
				
			}
		}
	

		//7. 调用jpeg_finish_decompress()完成解压过程
			jpeg_finish_decompress(&dinfo);

		//8. 调用jpeg_destroy_decompress释放jpeg解压对象dinfo
			jpeg_destroy_decompress(&dinfo);
			
	
		//9.关闭jpg文件释放存储读取信息的空间
		fclose(fp);
		free(buffer);
}

