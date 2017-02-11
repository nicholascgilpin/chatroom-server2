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
// Create a test timeline list (Only simulates create/diskIO)
void initTimelineList(vector<timeline>* tl){
	
	timeline t1 = timeline();
	timeline t2 = timeline();
	timeline t3 = timeline();

	
	string* sub1 = t1.add_subscribed();
	string* sub2 = t1.add_subscribed();
	string* sub3 = t3.add_subscribed();
	
	*sub1 = "b";
	*sub2 = "c";
	*sub3 = "a";
	
	t1.set_name("a");
	t2.set_name("b");
	
	
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
    if(userList.size() == NULL){
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

    Status User(ServerContext* context, const Requests* request, Requests* reply) override {
        cout << "Server in User function\n";
        printUserList();
        if(checkUserList(request->loginrequest()) == false){
            clientUser newUser = clientUser();
            timeline newUserTimeline = timeline();

            newUser.set_name(request->loginrequest());
            newUserTimeline.set_name(request->loginrequest());

            cout<<"\nPushing back new user: " + newUser.name() + "\n";

            userList.push_back(newUser);
            timelineList.push_back(newUserTimeline);

            printUserList();
            //printAllTimelines();

            cout<<"Size of list: " << userList.size() << "\n";
            reply->set_loginreply("Welcome, " + request->loginrequest() + "\n");
            return Status::OK;
        }
        else{
            cout << "\nAcessing userlist\n";
            printUserList();
            reply->set_loginreply("Welcome, " + request->loginrequest() + "\n");
            return Status::OK;
        }
    }

    Status List(ServerContext* context, const Requests* listRequest,
                    Requests* reply) override {
        cout << "Server in List\n";
        reply->set_listreply(processListRequest(listRequest->loginrequest()));
            //"Joined Chat Room " + statusPost->name() + "\n");
        return Status::OK;
    }
		
		Status chat(ServerContext* context,
                 ServerReaderWriter<Stats, Stats>* stream) override {
		  std::vector<Stats> received_log;
		  Stats recved;
			Stats reply;

			recved.set_timestamp(stamp());
			
			// Read in a message, reply with some messages, repeat
		  while (stream->Read(&recved)) {
				// @TODO: We can respond with subscriptions in future versions
				// untill we can gather messages, this rpc will simply echo back messages
				stream->Write(recved);
				sleep(2); // Keep the terminals readable by not replying like a maniac 
			}
  	return Status::OK;
		}
};

// Write each person's chatroom/timeline to the disk in a binary format
int TimelinesToDisk(vector<timeline> tl){
	cout << "Serializing timelines...\n";
	TimelineDB db = TimelineDB();
	for (size_t i = 0; i < tl.size(); i++) {
		timeline* t = db.add_timeline();
		*t = tl[i];
	}
	
	serverCounter += 10; // Ensure the server time is later than any message
	db.set_servercounter(serverCounter);
	
	std::string  fileName("Error in function TimelinesToDisk");
	fileName = "db.bin";
	fstream fs(fileName, ios::out | ios::trunc | ios::binary);
	if (!db.SerializeToOstream(&fs)) {
		cerr << "Failed to write to disk." << endl;
		return -1;
	}
	fs.close();
	return 0;
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
		cout << db.DebugString();
		for (size_t i = 0; i < db.timeline_size(); i++) {
			tl.push_back(timeline(db.timeline(i)));
		}
		serverCounter = db.servercounter();
	}
	return 0;
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
    
    cout << "Server is running on: " << "localhost:" + portNumber << endl;
    cout << "Server waiting for shutdown signal\n";
    server->Wait();
}

int main(int argc, char* argv[]) {
    string portNumber = "5056";
    if (argc >= 2) {
        portNumber = argv[1];
    }
		initTimelineList(&timelineList);
		TimelinesFromDisk(timelineList);
		cout << getTimelinePointer("a")->name() << endl;
		TimelinesToDisk(timelineList);
    startServer(portNumber);
    
    return 0;
}
