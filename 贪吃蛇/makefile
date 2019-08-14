#makefile通用模版
TARGET := snake
CC := arm-linux-gcc
HEADPATH := ./
#LIBPATH := ./

#step1: 获取所有的.c文件
CSRCS := $(wildcard ./*.c)
#CSRCS += $(wildcard ../sub/*.c)
HEAD:=-Ihead
KU:=-lpthread
#step2: 把.c文件编译成.o文件
%.o : %.c
	$(CC) -c $< -o $@ 
	
#step3: 把所有的.o文件链接成一个可执行文件
OBJS := $(patsubst %.c,%.o, $(CSRCS))
$(TARGET) : $(OBJS)
	$(CC) $+ -o  $@  $(KU)
	cp $@  ~/tftp/

#step4: 用伪目标文件实现还原编译
clean :
	rm -rf $(OBJS) $(TARGET)
	
#make一下实现自动编译,make clean实现还原编译	