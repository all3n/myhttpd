SRCS = $(wildcard *.cpp)


OBJS = $(SRCS:.cpp=.o)

CPP = g++   #代表所使用的编译器

INCLUDES = -I.

LIBS = -lpthread

CCFLAGS = -g  -Wall -O0   #附加参数

OUTPUT = main   #输出程序名称

all:$(OUTPUT)

$(OUTPUT) : $(OBJS)
	$(CPP) $^ -o $@ $(INCLUDES) $(LIBS)

%.o : %.cpp
	$(CPP) -c $< $(CCFLAGS)

clean:
	rm -rf main *.o    #清除中间文件及生成文件

debug:
	@echo $(SRCS)
	@echo $(OBJS)

.PHONY:clean debug
