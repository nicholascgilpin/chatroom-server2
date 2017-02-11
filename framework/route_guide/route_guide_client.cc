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

bool chatMode = false; //client starts in commandMode

ChatMsg makeMessage(const std::string& username, const std::string& message) {
	ChatMsg m;
	m.set_name(username);
	m.set_msg(message);
	return m;
}

class chatServiceClient {
    private: 
    unique_ptr<commandService::Stub> stub;
    
    public:
			string userinput;
    chatServiceClient(string address, string name) {
        // create a new channel to server
        shared_ptr<Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
        
        cout << "Client "+ userinput +" is connected on: " << address << "\n";
        
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

    void join(string subscribe){
        Requests userRequest;

        userRequest.set_loginrequest(userinput);
        userRequest.set_joinrequest(subscribe);
        cout<<"User: " + userinput + " Attempting to Join: " + subscribe + "\n";
        
        ClientContext context;
        Status status = stub->Join(&context, userRequest, &userRequest);
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot Join.\n";
        }
        else {
            // print server's reply
           // cout << "You've subscribed to: " + subscribe + "\n";
            cout << userRequest.joinreply();
        }
    
    }

    void list(){
        Requests listRequest;

        listRequest.set_loginrequest(userinput);
        ClientContext context;

        Status status = stub->List(&context, listRequest, &listRequest);
        if (!status.ok()) {
            cout << "Error Occured: Server Cannot List.\n";
        }
        else {
            // print server's reply
            cout << listRequest.listreply();
        }
    }
    
	void chat() {
        ClientContext context;
		static string name = this->userinput;
	    std::shared_ptr<ClientReaderWriter<ChatMsg, ChatMsg> > stream(
	        stub->chat(&context));
			
			// Thread; Takes keyboard input and sends to server
	    std::thread writer([stream]() {
	      std::vector<ChatMsg> notes;
				ChatMsg m = ChatMsg();
				string tempMessage = "";
				while (true){
					cout << name + ":\n";
					getline(cin, tempMessage);
					m.set_name(name);
					m.set_msg(tempMessage);
					stream->Write(m);
				}
					// The following is used for bulk sending messages:
					// notes.push_back(m);
					// for (const ChatMsg& note : notes) {
					// 	std::cout << "Sending:\n" << note.msg() << std::endl;
					// 	stream->Write(note);
					// }
					
				// Close the thread/connection and stop chatting
	      stream->WritesDone();
	    });
			
			// The current thread continues 
	    ChatMsg server_note;
	    while (stream->Read(&server_note)) {
	      std::cout << server_note.name() << ":" << server_note.msg() << std::endl;
	    }
	    writer.join();
	    Status status = stream->Finish();
	    if (!status.ok()) {
	      std::cout << "chat rpc failed." << std::endl;
	    }
	  }

};

bool commandMode(chatServiceClient* client) {
    string input;
    string delimiter = " ";
    getline(cin, input);
		int isInChatMode = false;
		
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
        client->list();
    }
    else if (tokens[0] == "JOIN" && tokens.size() == 2) {
        string chatRoom = tokens[1];
        cout<<"Subscribing to: " + chatRoom + "\n";
        client->join(chatRoom);

    }
    else if (tokens[0] == "LEAVE" && tokens.size() == 2) {
        string chatRoom = tokens[1];
        cout<<"Leaving: " + chatRoom + "\n";
       // client->leave(chatRoom);
    }
    else if (tokens[0] == "CHAT") {
        cout<<"Going to chat.\n";
      // client->chat();
		//	 isInChatMode = true;
    }
    else {
        cout << tokens[0] << " is not a valid command! Please enter LIST, JOIN, LEAVE, or CHAT: \n";
    }
		return isInChatMode;
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
        cerr << "Usage: hostName portNumber userName \n" << endl;
        return 0;
    }

    // create facebook chat client
    chatServiceClient client(hostName + ":" + portNumber, name);
    
    client.user();
    
    while (!chatMode) {
      chatMode = commandMode(&client);
    }
		// The chat threads will handle chatting
		// The main thread will wait untill closing time
    while(true){}
    return 0;
}
