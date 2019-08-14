TARGET:=main #最终生成的可执行文件的名字 赋值给目标   
CSRCS+=$(wildcard ./src/*.c) #将 src文件夹内的所有.c加入 
OBJS:=$(patsubst %.c,%.o,$(CSRCS)) #将CSRCS中所有的.c替换为.o 
LIB:=$(wildcard ./lib/*.o)			#将lib中的.o加入

CC:=gcc -pthread #指定编译器 

INCLUDE+=-I./inc  #讲inc的.h文件放入
#LIB := 
$(TARGET):$(OBJS)
	#终极目标文件 依赖于 OBJS 就是所有的.o文件 
	$(CC) -o $@ $+ 
	mv $(OBJS) ./lib		#将生成的.o文件移动到./lib目录
	cp $@ ~/tftp_share
	#gcc -o 目标文件  依赖文件
	
%.o:%.c
	$(CC) -c $< -o $@ $(INCLUDE) 
clean:
	rm $(LIB) $(TARGET)
	#在命令行执行 make clean 即可执行