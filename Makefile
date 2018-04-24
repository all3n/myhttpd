SRCS = $(wildcard *.cpp)


OBJS = $(SRCS:.cpp=.o)

CPP = g++

INCLUDES = -I.

LIBS = -lpthread

CCFLAGS = -g  -Wall -O0

OUTPUT = main

all:$(OUTPUT)

$(OUTPUT) : $(OBJS)
	$(CPP) $^ -o $@ $(INCLUDES) $(LIBS)

%.o : %.cpp
	$(CPP) -c $< $(CCFLAGS)

clean:
	rm -rf main *.o

debug:
	@echo $(SRCS)
	@echo $(OBJS)

.PHONY:clean debug
