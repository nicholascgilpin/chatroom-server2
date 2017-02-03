#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <vector>
#include <string>

using namespace std;

const static int MAX_MESSAGE = 255;
int status = 0;

/* Blocking read of data from the channel.
Returns a string of characters read from the channel.
Returns NULL if read failed. */
string cread(int sockfd)
{
	char buf[MAX_MESSAGE];

	if ((status = recv(sockfd, buf, MAX_MESSAGE,0)) <= 0) {
		if (status == 0) //most likely a client disconnect so we will not error
			return "";
		perror(string("Error reading!").c_str());
		return "";
	}

	string s = buf;

	return s;
}

/* Write the data to the channel. The function returns
the number of characters written to the channel. */
int cwrite(string _msg, int sockfd)
{
	if (_msg.length() >= MAX_MESSAGE) {
		cerr << "Message too long for Room!\n";
		return -1;
	}


	const char * s = _msg.c_str();

	if (send(sockfd, s, strlen(s)+1,0) < 0) {
	  perror(string("Error writing!").c_str());
	}
}