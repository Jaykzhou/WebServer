CXX = g++
CFLAGS = -std=c++14 -O2 -Wall -g

TARGET = test 
OBJS = ../code/log/*.cpp  ../code/buffer/*.cpp ../test/*.cpp

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET) -pthread -lmysqlclient

clean: 
	rm -rf ../bin/$(TARGET)