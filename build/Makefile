CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g

TARGET = server 
OBJS = ../code/log/*.cpp  ../code/buffer/*.cpp ../test/*.cpp
		../code/pool/*.h

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET) -pthread -lmysqlclient

clean: 
	rm -rf ../bin/$(TARGET)