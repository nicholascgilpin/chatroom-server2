/* Clayton Petty and Ryan Hairston
 * CSCE 438-500
 * crsd.cpp - Server side of chat service
 * PLEASE NOTE: The socket connection functions are used from previous 313 work
 * and may show up as "Copied" even though socket connections are standard.
 */
#include "common.h"

using namespace std;

//our chat room structure
struct Room {
    string name;
    string port;
	int num_members;
    vector<int> members;
	bool close = false;
};

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

int masterfd;  // listen on sock_fd
struct addrinfo hints, *serv, *res; //connection info
struct sockaddr_storage their_addr; // connector's address information
socklen_t sin_size;
char s[INET6_ADDRSTRLEN];
int rv;

vector<int> fds; //our list of file descriptors connected to main room
string port = "1024";
int nextPort = 1025;
vector<Room> rooms;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void process_slave_request(Room & room, int senderfd, const string msg);
void* create_slave_room(void* args);

/*--------------------------------------------------------------------------*/
/* FUNCTIONS */
/*--------------------------------------------------------------------------*/

//gets next port for new chat rooms
string getNextPort(){
    string newPort = to_string(nextPort);
    nextPort++;
    return newPort;
}

//Finds index of chat room by name
//return index to use reference since structs have
//hard time passing empty reference
//returns -1 if no room by that name found
int getRoomByName(const string name){
    for(int i = 0; i < rooms.size(); ++i){
        if (rooms[i].name == name){
            return i;
        }
    }
    return -1;
}

//Finds index of chat room by port
//return index to use reference since structs have
//hard time passing empty reference
//returns -1 if no room by that port found
int getRoomByPort(const string port){
    for(int i = 0; i < rooms.size(); ++i){
        if (rooms[i].port == port){
            return i;
        }
    }
    return -1;
}

//Deletes a room from the room array
void deleteRoomByName(const string name){
	for(int i = 0; i < rooms.size(); ++i){
        if (rooms[i].name == name){
			rooms.erase(rooms.begin() + i, rooms.begin() + i + 1);
        }
    }
}

//Main chat room request processor
//PARAM ch_fd: The file descriptor that sent the request
//PARAM _request: The request sent
void process_request(int & ch_fd, const string & _request) {
	
	//process a CREATE (roomname) call
    if (_request.compare(0, 6, "CREATE") == 0) {
        cout << "Request to create new channel" << endl;
        Room newChannel{};

        string newPort = getNextPort();
        string newChannelName = _request.c_str()+7;
		int foundChannel = getRoomByName(newChannelName);
		if (foundChannel == -1){ //if no channel found, make a new one
			newChannel.name = newChannelName;
			newChannel.num_members = 0;
			newChannel.port = newPort;

			rooms.push_back(newChannel);
			
			pthread_t thread_id;
			//start new thread (a new chat room) at the newPort
			pthread_create(&thread_id, NULL, create_slave_room, (void*) newPort.c_str());

			cwrite("Created new channel at port: " + newPort, ch_fd);
		} else {
			cwrite("Room with that name already exists.", ch_fd);
		}
    } else if (_request.compare(0, 4, "JOIN") == 0){ //Processing a JOIN (roomname) request
        string requestedChannelName = _request.c_str()+ 5;
		cout << "Requested channel name: " << requestedChannelName << endl;
        int foundChannel = getRoomByName(requestedChannelName);
        if (foundChannel == -1){ //Can't join non-existant room
			cout << "Room does not exist" << endl;
            cwrite("Room does not exist", ch_fd);
        } else {
			cout << rooms[foundChannel].name << " " << rooms[foundChannel].members.size() << endl;
            cwrite(requestedChannelName + " chat room located at port " + rooms[foundChannel].port + " with " + to_string(rooms[foundChannel].num_members) + " current members.", ch_fd);
			rooms[foundChannel].num_members++;
			for (int i = 0; i < fds.size(); ++ i){ //remove the clients file descriptor from our list so we dont process their requests
				if (ch_fd == fds[i]){
					cout << "+connection erased " << endl;
					fds.erase(fds.begin() + i, fds.begin() + i + 1);
				}
			}
			cout << "port sent" << endl;
        }
    } else if (_request.compare(0, 6, "DELETE") == 0) { //Processing a DELETE (roomname) request
        string requestedRoomName = _request.c_str() + 7;
		cout << "Deleting room **" << requestedRoomName << "**" << endl;
        int foundRoom = getRoomByName(requestedRoomName);
        if (foundRoom == -1) {
            cwrite("Room does not exist", ch_fd);
        } else {
			rooms[foundRoom].close = true; //Tells thread (which checks this) that the room is closing
			cout << "Current rooms: " << rooms.size() - 1 << " are left." << endl; //printing out remaining rooms on server
			for(int i = 0; i < rooms.size(); ++i) {
				cout << rooms[i].name << endl;
			}
			cwrite("Room Deleted", ch_fd);
        }
    } else { //Can only send CREATE/JOIN/DELETE requests in main room
		if (_request == ""){
			for(int i = 0; i < fds.size(); ++i){
				if (ch_fd == fds[i]){
					close(ch_fd);
					fds.erase(fds.begin() + i, fds.begin() + i + 1);
				}
			}
		} else {
			cwrite("Unknown command", ch_fd);
		}
    }

}

//Creates master room at port 1024
void create_master_room(){
	string s_port = "1024";
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	// Setup host
	if ((rv = getaddrinfo(NULL, s_port.c_str(), &hints, &serv)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// Create the socket
	if ((masterfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
		perror("server: socket");
		exit(1);
	}

	// Set socket options (allow multiple connections)
	int optval = 1;
	setsockopt(masterfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	// Bind the socket to the address
	if (bind(masterfd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(masterfd);
		perror("server: bind");
		exit(1);
	}

	// Unsure
	freeaddrinfo(serv); // all done with this structure

	// Begin listening on oscket
	if (listen(masterfd, 20) == -1) {
		perror("listen");
		exit(1);
	}

	fd_set read_fd_set;
	fds.push_back(masterfd);

	while(1) {
		sin_size = sizeof their_addr;

		FD_ZERO(&read_fd_set);
		int max = masterfd;
		for (int i = 0; i < fds.size(); ++i){
			FD_SET(fds[i], &read_fd_set);
			if (max < fds[i]){
				max = fds[i];
			}
		}
		
		//wait to see which fds are active
		int n = select(max + 1, &read_fd_set, NULL, NULL, NULL);

		if (n == -1) {
			perror("Invalid # of file descriptors found in master");
			cout << fds.size() << endl;
			exit(1);
			continue;
		} else if (n == 0){
			perror("Timeout, no activity");
		}
		else {
			if (FD_ISSET(masterfd, &read_fd_set)){ //new connection detected
				printf("New Connection Received\n");
				int incomingfd = accept(masterfd, (struct sockaddr *)&their_addr, &sin_size);
				cwrite("Welcome to chat room service", incomingfd);
				fds.push_back(incomingfd);
			} else { //otherwise process a client request
				for (int i = 1; i < fds.size(); ++i) {
					if (FD_ISSET(fds[i], &read_fd_set)){
						string msg = cread(fds[i]);
						msg = msg.c_str() + 8; //removing "CLIENT: " from message
						process_request(fds[i], msg);
					}
				}
			}
		}
	}
}

//Processing chat room requests.
void process_slave_request(Room & room, int senderfd, const string msg) {
	if (msg == ""){ //Socket sends empty request when closed.
		cout << "Detected client dissconnect: Closing socket." << endl;
		for(int i = 0; i < room.members.size(); ++i){
			if (senderfd == room.members[i]){
				close(senderfd);
				room.members.erase(room.members.begin() + i, room.members.begin() + i + 1);
			}
		}
	} else if (msg == "CHAT ROOM SAYS: Being deleted. Goodbye!") { //Hardcoded for server to send this message to all clients in room when closing 
		for(int i = 1; i < room.members.size(); ++i){ //send delete message
			cwrite(msg, room.members[i]);
		}
		for(int i = 0; i < room.members.size(); ++i){ //close all connections
			close(room.members[i]);
			room.members.erase(room.members.begin() + i, room.members.begin() + i + 1);
		}
		deleteRoomByName(room.name); //delete room from list of rooms
	} else { //If no other special conditions, forward message to all in chat room
		for (int i = 1; i < room.members.size(); ++i) {
    		if (room.members[i] != senderfd){
                cwrite(msg, room.members[i]);
    		}
		}
	}
}

//Creating a chat room at port (char*) args)
void* create_slave_room(void* args) {
	string slavePort = (char*) args;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	int slavefd;

	// Setup host
	if ((rv = getaddrinfo(NULL, slavePort.c_str(), &hints, &serv)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// Create the socket
	if ((slavefd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
		perror("server: socket");
		exit(1);
	}

	// Set socket options (allow multiple connections)
	int optval = 1;
	setsockopt(slavefd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	// Bind the socket to the address
	if (bind(slavefd, serv->ai_addr, serv->ai_addrlen) == -1) {
		close(slavefd);
		perror("server: bind");
		exit(1);
	}

	// Unsure
	freeaddrinfo(serv); // all done with this structure

	// Begin listening on socket
	if (listen(slavefd, 20) == -1) {
		perror("listen");
		exit(1);
	}

	fd_set read_fd_set;
	int room = getRoomByPort(slavePort);
	rooms[room].members.push_back(slavefd);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while(1) {
		sin_size = sizeof their_addr;

		FD_ZERO(&read_fd_set);
		int max = slavefd;
		for (int i = 0; i < rooms[room].members.size(); ++i){
			FD_SET(rooms[room].members[i], &read_fd_set);
			if (max < rooms[room].members[i]){
				max = rooms[room].members[i];
			}
		}

		//We check every second (from the timeout) if the room has been "deleted"
		if (rooms[room].close == true) {
			process_slave_request(rooms[room], -1, "CHAT ROOM SAYS: Being deleted. Goodbye!");
			break;
		} 
		
		int n = select(max + 1, &read_fd_set, NULL, NULL, &tv);

		if (n == -1) {
			perror("Invalid # of file descriptors found in slave");
			continue;
		} else if (n == 0) { //check every second for updates (don't block thread)
			tv.tv_sec = 1; //reset so it doesn't always throw a timeout
		}
		else {
			if (FD_ISSET(slavefd, &read_fd_set)){ //new connection to chat room
				printf("New slave Connection Received\n");
				int incomingfd = accept(slavefd, (struct sockaddr *)&their_addr, &sin_size);
				cwrite("Welcome to chat room " + rooms[room].name, incomingfd);
				rooms[room].members.push_back(incomingfd);
			} else { //otherwise just process request
				for (int i = 1; i < rooms[room].members.size(); ++i) {
					if (FD_ISSET(rooms[room].members[i], &read_fd_set)){
						string msg = cread(rooms[room].members[i]);
						process_slave_request(rooms[room], rooms[room].members[i], msg);
					}
				}
			}
		}
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	create_master_room();
}
