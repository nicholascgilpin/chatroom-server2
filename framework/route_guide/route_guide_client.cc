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

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <sstream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "chatservice.grpc.pb.h"

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using chatserver::ChatMsg;
using chatserver::timeline;
using chatserver::Stats;
using chatserver::JoinRequest;
using chatserver::clientUser;
using chatserver::commandService;
using chatserver::Requests;
//using chatserver::commandService;
//using chatserver::chatStream;

bool chatMode = false; //client starts in commandMode

class chatServiceClient {
    private: 
    unique_ptr<commandService::Stub> stub;
    string userinput;
    
    public:
    chatServiceClient(string address, string name) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client "+userinput+" is connected on: " << address << endl;
        
        stub = commandService::NewStub(channel);

        userinput = name;
    }

    void user(){
        Requests userRequest;

        userRequest.set_loginrequest(userinput);
        ClientContext context;
        Status status = stub->User(&context, userRequest, &userRequest);
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Login.\n";
        }
        else {
            // print server's reply
            cout << userRequest.loginreply();
        }
    }
    
};

void commandMode(chatServiceClient* client) {
    string input;
    string delimiter = " ";
    getline(cin, input);

    size_t pos = 0;
    vector<string> tokens;

    while ((pos = input.find(delimiter)) != string::npos) {
        tokens.push_back(input.substr(0, pos));
        input.erase(0, pos + delimiter.length());
    }
    tokens.push_back(input);

    cout<< "In commandmode. \n";
    cout<<"Got your command: " + tokens[0] + "\n";

    if (tokens[0] == "LIST") {
        cout<<"Printing List.\n";
       //client->list();
    }
    else if (tokens[0] == "JOIN" && tokens.size() == 2) {
        string chatRoom = tokens[1];
        cout<<"Subscribing to: " + chatRoom + "\n";
       // client->join(chatRoom);
    }
    else if (tokens[0] == "LEAVE" && tokens.size() == 2) {
        string chatRoom = tokens[1];
        cout<<"Leaving: " + chatRoom + "\n";
       // client->leave(chatRoom);
    }
    else if (tokens[0] == "CHAT") {
        cout<<"Going to chat.\n";
       // client->chat();
    }
    else {
        cout << tokens[0] << " is not a valid command! Please enter LIST, JOIN, LEAVE, or CHAT: \n";
    }
}

int main(int argc, char* argv[]) {
    
    string hostName = "localhost";
    string portNumber = "5056";
    string name;
    
    if (argc >= 4) {
        hostName = argv[1];
        portNumber = argv[2];
        name = argv[3];
    }
    else{
        cerr << "Usage: hostName portNumber userName " << endl;
        return 0;
    }

    // create facebook chat client
    chatServiceClient client(hostName + ":" + portNumber, name);
    
    client.user();
    
    while (!chatMode) {
        commandMode(&client);
    }
    
    return 0;
}
