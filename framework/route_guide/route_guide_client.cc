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
using chatserver::User;
using chatserver::commandService;
//using chatserver::commandService;
//using chatserver::chatStream;

bool chatMode = false; //client starts in commandMode

class chatServiceClient {
    private: 
    unique_ptr<commandService::Stub> stub;
    
    public:
    chatServiceClient(string address) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client is connected on: " << address << endl;
        
        stub = commandService::NewStub(channel);
    }
    
    void testJoin() {
        // create join request and reply objects
        JoinRequest statusPost;
        Stats statusGet;
        
        statusPost.set_name("test");
        
        // send join request to server
        ClientContext context;
        Status status = stub->Join(&context, statusPost, &statusGet); 
        
        // check if request was successful
        if (!status.ok()) {
            cout << "Error Happened\n";
            cout << "Join statusGet: " << statusGet.name() << endl;
        }
        else {
            cout << "Join statusGet: " << statusGet.name();
        }
    }
};

//string split functions below
/*
void split(const string &s, char delim, vector<string> &elems) {
  stringstream ss;
  ss.str(s);
  string item;
  while (getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, elems);
  return elems;
}
*/

int main(int argc, char* argv[]) {
    
    string hostName = "localhost";
    string portNumber = "5056";
    
    if (argc >= 3) {
        hostName = argv[1];
        portNumber = argv[2];
    }
    
    // create facebook chat client
    chatServiceClient client(hostName + ":" + portNumber);
    
    client.testJoin();
    
    return 0;
}