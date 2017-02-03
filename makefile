# makefile
# adding capability to use makefile in various environments without modifications
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    cc = clang++
else ifeq ($(UNAME), Darwin)
    cc = g++
else
    cc = g++
endif

all: server client

  clean: 
	rm crsd crc

 server: crsd.cpp
	$(cc) -std=c++11 -g -o crsd crsd.cpp -lpthread

client: crc.cpp
	$(cc) -std=c++11 -g -o crc crc.cpp -lpthread
