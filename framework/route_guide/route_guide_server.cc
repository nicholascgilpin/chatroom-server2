	/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <cstdlib>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "chatservice.grpc.pb.h"

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using chatserver::Stats;
using chatserver::timeline;
using chatserver::JoinRequest;
using chatserver::clientUser;
using chatserver::commandService;
using chatserver::Requests;
using chatserver::TimelineDB;
//using chatserver::commandService;
//using chatserver::chatStream;

// Debugging Code /////////////////////////////////////////////////////////////
const bool DEBUG = false; // Toggles debugging print messages

// Create a test timeline list (Only simulates create/diskIO)
void initTimelineList(vector<timeline>* tl){
	
	timeline t1 = timeline();
	timeline t2 = timeline();
	timeline t3 = timeline();

	t1.set_name("x");
	t2.set_name("y");
	t3.set_name("z");
	
	string* sub1 = t1.add_subscribed();
	string* sub2 = t1.add_subscribed();
	string* sub3 = t3.add_subscribed();
	
	*sub1 = "b";
	*sub2 = "c";
	*sub3 = "a";
	
	Stats* s1 = t1.add_statuses();
	Stats* s2 = t1.add_statuses();
	Stats* s3 = t2.add_statuses();
	Stats* s4 = t2.add_statuses();
	
	s1->set_name("a");
	s1->set_msg("Test message: Boba");
	s1->set_timestamp(4);
	s2->set_name("b");
	s2->set_msg("Test message: Queso");
	s2->set_timestamp(3);
	s3->set_name("c");
	s3->set_msg("Test message: Icecream");
	s3->set_timestamp(1);
	s4->set_name("c");
	s4->set_msg("Test message: Chocolate");
	s4->set_timestamp(2);
	
	tl->push_back(t1);
	tl->push_back(t2);
	tl->push_back(t3);
	
}

// Global Variables ////////////////////////////////////////////////////////////
vector<clientUser> userList = vector<clientUser>();
vector<timeline> timelineList = vector<timeline>(); // stores each person's timelines 
int serverCounter = 0; // Acts as the server's clock; used for timestamps

// Increments and returns the latest time; @TODO: add mutex locks in future
int stamp(){
	serverCounter += 1;
	return serverCounter;
}

// Finds and returns a timeline pointer for user with nameX 
timeline* getTimelinePointer(string nameX){
	for (size_t i = 0; i < timelineList.size(); i++) {
		if (nameX == timelineList[i].name()){
			return &timelineList[i];
		}
	}
	return NULL;
}

// Allow message ranking for sorting
bool msgGreaterThan(const Stats& l, const Stats& r){
	return (l.timestamp() < r.timestamp());
}

// Gets the most recent k messages for a person
// mostRecentTime is the timestamp of the most recent message sent to client
std::vector<Stats> getRecentMessages(string name, int k, int mostRecentTime){
	std::vector<Stats> messages;
	timeline* userTimeline = getTimelinePointer(name);
	timeline* subbedTimeline; // Timeline of person to whom user is subscibed
	
	// Gather all messages from user's subscriptions
	if (userTimeline == NULL) {
		std::cerr << "Error: User " << name << " exists, but has no timeline!" << '\n';
	} else {
		for (size_t i = 0; i < userTimeline->subscribed_size(); i++) {
			subbedTimeline = getTimelinePointer(userTimeline->subscribed(i));
			if (subbedTimeline == NULL) {
				std::cerr << "Error: User " << name << " exists, but has no timeline!" << '\n';
			} else {
				for (size_t j = 0; j < subbedTimeline->statuses_size(); j++) {
					if (subbedTimeline->statuses(j).timestamp() > mostRecentTime) {
						messages.push_back(subbedTimeline->statuses(j));
					}
				}
			}
		}
	}
	std::sort(messages.begin(), messages.end(), msgGreaterThan);
	if ((messages.size() > k) && (k >= 0)) {
		messages.resize(k);
	}
	// if (DEBUG) {
	// 	cout << "Most recent messages:\n";
	// 	for (size_t i = 0; i < messages.size(); i++) {
	// 		cout << messages[i].timestamp() << endl;
	// 	}
	// }
	return messages;
}

bool checkUserList(string username){
    cout<<"Size of list: " << userList.size() << "\n";
    for(int i = 0; i < userList.size(); i++){
        if(userList[i].name() == username){
            cout<<"username: " + username + " exists!\n";
            return true;
        }           
    }
    cout<<"Username " + username + " Does not exist.\n";
    return false;
}

void printUserList(){
    if(userList.size() == 0){
        cout<<"Userlist is empty";
    }   
    else {
        for(int i = 0; i < userList.size(); i++){
            cout<< "Username from Print User List: " + userList[i].name() + "\n";
        }
    }
}


/*void printSpecificTimelines(string username){
    if(timelineList.size() == NULL){
        cout<<"Timeline List is empty";
    }   
    else {
        for(int i = 0; i < timelineList.size(); i++){
            cout<< "Username from Print All Timeline List: " + timelineList[i].name() + "\n";
        }
    }
}*/

string processListRequest(string username){
    string allTimelines = "All Users:\n [";
    string usersTimelines = "Your Subscriptions:\n [";
    string messageToSend = "";
    int temp;

    for (int i = 0; i < timelineList.size(); i++){
        if(timelineList[i].name() == username){
            cout<<"\nIn processListRequest: Found user!\n"; //Bugfixing print, comment out later
            temp = i;
            break;
        }
    }

    for(int i = 0; i < userList.size(); i++){
        allTimelines += userList[i].name() + ",\n";
    }
    allTimelines += "]\n";


    if( timelineList[temp].subscribed_size() == 0){
        usersTimelines = "You are not subscribed to anyone.]\n";
    }
    else{
        for(int j = 0; j < timelineList[temp].subscribed_size(); j++){
            usersTimelines += timelineList[temp].subscribed(j) + ",\n";
        }
        usersTimelines += "]\n";
    }

    messageToSend = allTimelines + "__________________________________________\n" + usersTimelines;
    return messageToSend;
    
}

string processSubscription(string user, string userToSubscribeTo){
    string response;
    int temp = 0;
    cout<<"\nIn processSubscription\n";

    if(checkUserList(userToSubscribeTo) == false){
        cout<<"\nInvalid user input from client, no user to subscribe to.\n";
        response = "Cannot join user: " + userToSubscribeTo +" user does not exist.\n";
        return response;
    }
    
    if(user == userToSubscribeTo){
        response = "You cannot subscribe to yourself.\n";
        return response;
    }

    for (int i = 0; i < timelineList.size(); i++){
        if(timelineList[i].name() == user){
            cout<<"\nIn process subscription: Found user!\n"; //Bugfixing print, comment out later
            temp = i;
            break;
        }
    }

  //  cout<< "\nBefore second for. Temp is: " << temp << "\n"; //Bugfixing print, comment out later
 //   cout<< "\nChecking timeline list: " << timelineList[temp].subscribed_size() << "\n"; //Bugfixing print, comment out later

    if(timelineList[temp].subscribed_size() == 0){
        string* hold = timelineList[temp].add_subscribed();
        *hold = userToSubscribeTo;
        //  timelineList[i].subscribed(i).push_back(userToSubscribeTo);
        response = "Server Response: Subscribed to: " + userToSubscribeTo + "\n";
        return response;  
    }
    else{
        for(int j = 0; j < timelineList[temp].subscribed_size(); j++){
        //    cout<< "Subscribed size: " << timelineList[temp].subscribed_size() << endl; //Bugfixing print, comment out later
            
            if(timelineList[temp].subscribed(j) == userToSubscribeTo){
                response = "You are already subscribed to: " + userToSubscribeTo + "\n";
                return response;
            }
            else {
                string* hold = timelineList[temp].add_subscribed();
                *hold = userToSubscribeTo;
                //  timelineList[i].subscribed(i).push_back(userToSubscribeTo);
                response = "Server Response: Subscribed to: " + userToSubscribeTo + "\n";
                return response;  
            }
        }
    }

    response = "Error in join function.\n";
    return response;
}

//Process Leave command.
string processUnsubscribe(string username, string chatroom){
    string response;
    int temp = 0;
    int temp2 = 0;
    cout<<"\nIn processUnsubscribe\n";

    //If the chatroom doesnt exist, don't try to leave anything
    if(checkUserList(chatroom) == false){
        cout<<"\nInvalid user input from client, no user to unsubscribe from.\n";
        response = "Cannot Leave user: " + chatroom +" does not exist.\n";
        return response;
    }
    
    //Users cannot unsub from themselves cuz why???
    if(username == chatroom){
        response = "You cannot leave yourself.\n";
        return response;
    }

    //Get location of username's timeline from timeline list.
    for (int i = 0; i < timelineList.size(); i++){
        if(timelineList[i].name() == username){
            cout<<"\nIn processUnsubscribe: Found user!\n"; //Bugfixing print, comment out later
            temp = i;
            break;
        }
    }

    //If the user isn't subscribed to anyone, don't try to leave anything.
    if(timelineList[temp].subscribed_size() == 0){
        response = "You cannot leave anyone as you are not subscribed to anyone.\n";
        return response;  
    }
    else{
        //GRPC Proto buffers have no handler for removing an arbitrary array element
        //So we have to handle it manually.
        //We create a new list, copy everything but the room the user wants to leave, and store it 
        //Back into the timelinelist.
        timeline copyAllButOne = timeline();
        copyAllButOne.set_name(username);

        //Find index of the name of the chatroom the user wants to leave
        for(int j = 0; j < timelineList[temp].subscribed_size(); j++){
            if(timelineList[temp].subscribed(j) == chatroom){
                cout<<"\nIn processUnsubscribe: Found chatroom!\n"; //Bugfixing print, comment out later
                temp2 = j;
                break;
            }    
        }    

        //If the index of the chatroom is at index 0, copy the original list from 1 to end
        if(temp2 == 0){
            for(int h = 1; h < timelineList[temp].subscribed_size(); h++){
                string* hold = copyAllButOne.add_subscribed();
                *hold = timelineList[temp].subscribed(h);
            }
            timelineList[temp] = copyAllButOne;
            response = "Unsubscribed from: " + chatroom + "\n";
            return response;
        }
        else {
            //Otherwise, copy up to the element and after the element into the copyAllButOne list.
            for(int k = 0; k < temp2; k++){
                string* hold = copyAllButOne.add_subscribed();
                *hold = timelineList[temp].subscribed(k);
            }
        
            for(int l = temp2+1; l < timelineList[temp].subscribed_size(); l++){
                string* hold = copyAllButOne.add_subscribed();
                *hold = timelineList[temp].subscribed(l);
            }
            //Set the original list to be the new copyAllButOne list, which contains all of the
            //User's original chatrooms except for the one they left.
            timelineList[temp] = copyAllButOne;
            response = "Unsubscribed from: " + chatroom + "\n";
            return response;
        }
    }
    
    response = "Error in leave function.\n";
    return response;
}
    
    // Bob has his own timeline
//timeline list has a data structure called subscribed; this is who Bob is following

class chatServiceServer final : public commandService::Service {
    public:
    chatServiceServer() {}
    
    // process client JOIN command
    Status Join(ServerContext* context, const Requests* statusPost,
                     Requests* statusGet) override {
        cout << "Server in join\n";
        statusGet->set_joinreply(processSubscription(statusPost->loginrequest(), statusPost->joinrequest()));
            //"Joined Chat Room " + statusPost->name() + "\n");
        return Status::OK;
    }

		// Create a new timeline and user if one with name doesn't alrady exist
    Status User(ServerContext* context, const Requests* request, Requests* reply) override {
        cout << "Server in User function\n";
        if(checkUserList(request->loginrequest()) == false){
            clientUser newUser = clientUser();
            timeline newUserTimeline = timeline();

            newUser.set_name(request->loginrequest());
            newUserTimeline.set_name(request->loginrequest());

            cout<<"\nPushing back new user: " + newUser.name() + "\n";

            userList.push_back(newUser);
            timelineList.push_back(newUserTimeline);

            reply->set_loginreply("Welcome, " + request->loginrequest() + "\n");
						
						if (DEBUG) {
							printUserList();
							//printAllTimelines();
							cout<<"Size of user list: " << userList.size() << "\n";
							cout<<"Size of timeline list: " << timelineList.size() << "\n";
						}
						
            return Status::OK;
        }
        else{
            cout << "\nAcessing userlist\n";
            reply->set_loginreply("Welcome, " + request->loginrequest() + "\n");
            return Status::OK;
        }
    }

    Status List(ServerContext* context, const Requests* listRequest,
                    Requests* reply) override {
        cout << "Server in List\n";
        reply->set_listreply(processListRequest(listRequest->loginrequest()));
        return Status::OK;
    }

    Status Leave(ServerContext* context, const Requests* request, 
                    Requests* reply) override {
        cout<< "Server in leave function\n";
        reply->set_leavereply(processUnsubscribe(request->loginrequest(), request->leaverequest()));
        return Status::OK;
    }
		
		Status chat(ServerContext* context,
	     ServerReaderWriter<Stats, Stats>* stream) override {
			Stats recved;
			timeline* mailbox;
			static string name;
			static int mostRecentMessageSent = -1;
			 
			 cout << "Send recent 20 messages on chat request" << endl;
			 stream->Read(&recved);
			 cout << "First message:\n" << recved.DebugString() << endl;
			 std::vector<Stats> recentMsgs = getRecentMessages(recved.name(), 20, -1);
			 for (const Stats& msg : recentMsgs) {
				 if (msg.timestamp() > mostRecentMessageSent) {
					 mostRecentMessageSent = msg.timestamp();
				 }
				 stream->Write(msg);
			 }
			 name = recved.name();
			 
			std::thread writer([stream]() {
				std::vector<Stats> outgoing;
				while (true) {
					outgoing = getRecentMessages(name, -1, mostRecentMessageSent);
					if (outgoing.size() > 0) {
						cout << "Writer/" << name <<": Sending message" << endl;
						for (const Stats& msg : outgoing) {
							if (msg.timestamp() > mostRecentMessageSent) {
								mostRecentMessageSent = msg.timestamp();
							}
							stream->Write(msg);
						}
						outgoing.resize(0);
					}
				}
			});
			
			// Read in messages
		  do {
				// Timestamp and store the message in this user's timeline
				if ((mailbox = getTimelinePointer(recved.name())) == NULL) {
					cerr << "Error: A client who doesn't exist is talking to us...\n";
				}
				else{
					recved.set_timestamp(stamp());
					Stats* temp = mailbox->add_statuses();
					*temp = recved;
				}
				// if (DEBUG) {
				// 	cout << "Mail box contents:\n" << mailbox->DebugString() << endl;
				// }
				sleep(1); // Keep the terminals readable by not replying like a maniac 
				cout << "Server/" << name << ": waiting for a read\n";
			} while (stream->Read(&recved));
			// End chat
		}
// End class
};

// Write each person's chatroom/timeline to the disk in a binary format
void TimelinesToDisk(){
	cerr << "\nSerializing timelines...\n"; 
	TimelineDB db = TimelineDB();
	for (size_t i = 0; i < timelineList.size(); i++) {
		timeline* t = db.add_timeline();
		*t = timelineList[i];
	}
	serverCounter += 10; // Ensure the server time is later than any message
	db.set_servercounter(serverCounter);
	std::string  fileName("Error in function TimelinesToDisk");
	fileName = "db.bin";
	fstream fs(fileName, ios::out | ios::trunc | ios::binary);
	if (!db.SerializeToOstream(&fs)) {
		cerr << "Failed to write to disk." << endl;
	}
	fs.close();
	exit(0);
}
// Read the timelines from the disk to a vector
int TimelinesFromDisk(vector<timeline> tl){
	std::string fileName = "db.bin";
	TimelineDB db = TimelineDB();
	
	cout << "Checking for chat logs...";
	fstream input(fileName, ios::in | ios::binary);
  if (!db.ParseFromIstream(&input)) {
    cout << " No chat logs found.\n" << endl;
    return -1;
  }	
	else{
		cout << " Chat logs found!\n";
		if (DEBUG) {
			cout << db.DebugString();
		}
		for (size_t i = 0; i < db.timeline_size(); i++) {
			clientUser newUser = clientUser();
			newUser.set_name(db.timeline(i).name());
			userList.push_back(newUser);
			
			timelineList.push_back(timeline(db.timeline(i)));
		}
		serverCounter = db.servercounter();
	}
	return 0;
}

// Save after receiving a shutdown signal
void signalHandler(int n){
	static pthread_once_t semaphore = PTHREAD_ONCE_INIT;
	pthread_once(&semaphore, TimelinesToDisk);
	exit(0);
}

void startServer(string portNumber) {
    // create facebookServer object
    chatServiceServer chatServiceServer;
    
    // gRPC class object to create a gRPC server
    // man page for ServerBuilder: http://www.grpc.io/grpc/cpp/classgrpc_1_1_server_builder.html#a0b06b5828b892feeb6541c8eeae2d542
    ServerBuilder serverBuilder;
    
    // binds server to localhost address
    serverBuilder.AddListeningPort("localhost:" + portNumber, grpc::InsecureServerCredentials());
    
    serverBuilder.RegisterService(&chatServiceServer);
    
    // return a running server that is ready to process calls
    unique_ptr<Server> server(serverBuilder.BuildAndStart());
    
		cout << "Use ctrl-c to save to disk and shutdown\n";
    cout << "Server is running on: " << "localhost:" + portNumber << endl;
    server->Wait();
}

int main(int argc, char* argv[]) {
		// Used for saving server on shutdown
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);

    string portNumber = "5056";
    if (argc >= 2) {
        portNumber = argv[1];
    }
		TimelinesFromDisk(timelineList);
	  startServer(portNumber);
    
    return 0;
}
