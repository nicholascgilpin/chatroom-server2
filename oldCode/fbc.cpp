/* Clayton Petty and Ryan Hairston
 * CSCE 438-500
 * crc.cpp - Client side of chat service
 * PLEASE NOTE: The socket connection functions are used from previous 313 work
 * and may show up as "Copied" even though socket connections are standard.
 */

#include "common.h"
#include <sys/poll.h>

using namespace std;

//const string _server_host_name = "sun.cse.tamu.edu";
const string _server_host_name = "localhost";
int MASTER_PORT = 1024;
string port = "1024";

struct addrinfo hints, *serv, *res;
int fd;  // listen on fd

//Connect on a given port number
//Returns the file descriptor to use
int connect_on_port(string _port_no)
{
	port = _port_no;
	int sockfd;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status;

	if ((status = getaddrinfo(_server_host_name.c_str(), _port_no.c_str(), &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}

	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		perror ("Error creating socket\n");
		exit(1);
	}

	// connect!
	if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
		fprintf(stderr, "Connect error: %d\n", errno);
		exit(1);
	}
	
	return sockfd;
}

int main()
{
	fd_set read_fd_set;
	struct timeval to = {0,10}; // Select timeout
	int max, selected = 0;

	bool join = false;
	string msg = "";
	string input = "";
	struct pollfd stdin_poll = { .fd = STDIN_FILENO
                           , .events = POLLIN | POLLRDBAND | POLLRDNORM | POLLPRI };

	fd = connect_on_port(to_string(MASTER_PORT)); //connect to main port for beginning
	
	while(true){

		FD_ZERO(&read_fd_set);
		max = fd;
		FD_SET(fd, &read_fd_set);
		
		selected = select(max+1, &read_fd_set, NULL, NULL, &to);
		
		//If incoming message from server
		if (FD_ISSET(fd, &read_fd_set)) {
			string msg = cread(fd);
			if (msg.compare(0, 4, "CHAT") == 0) { //chat room being deleted
				msg += "\nREJOINING MAIN ROOM";
				fd = connect_on_port(to_string(MASTER_PORT).c_str());
			} else if (msg.compare(0, 8, "CLIENT: ") == 0) { //checking if message sent by client to prevent "prank" server messages"
				msg = msg.c_str() + 8;
			}
			
			printf("%s\n", msg.c_str());
			if (join) { //if you sent a JOIN message (and you were at port 1024 at the time)
				const char* portmsg = strstr(msg.c_str(), "at port ");
				if (portmsg != NULL) {
					//getting port from message
					char* port = new char[5];
					port[4] = '\0';
					strncpy(port, portmsg + 8, 4);

					fd = connect_on_port(port); //Join new room on new port
					free(port);
					string msg = cread(fd);
					printf("%s\n", msg.c_str());
				}
				join = false; //prevents prank chat room message "at port xxxx" disconnecting users
			}
		}
		//If there is user input (non-blocking)
		if (poll(&stdin_poll, 1, 0) == 1) {
			getline(cin, msg);
			cwrite("CLIENT: " + msg, fd); //put header in front of client to distinguish client vs server messages

			if (msg.compare(0, 4, "JOIN") == 0 && port == "1024") //prevents prank chat room message "at port xxxx" disconnecting users
				join = true;
		}
	}
	return 0;
}

