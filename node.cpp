#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <cstring>
#include <sstream>
#include <cstdlib> // malloc
#include "ledger.h" //Ledger/Transaction classes
using namespace std;

int *my_port;
static int bootstrap_port = 8080; // port of the bootstrap node.

class Node {

private:
	string node_ip; //running instance's ip
	int port;	//running instance's port
	int total_peers;
	map <int, pair<string, int>> peers; //Map of peer sockets (ip,port)
	int server_fd;
	Ledger ledger; //local copy of the Ledger in the node.
	

	void register_to_bootstrap(void)
	{

		string cmd = "REGISTER";
		string my_reg = cmd + " " + node_ip + " " + to_string(port);		
		cout << "Registering to the Bootstrap node as: " << my_reg << endl;

	}

	void handleConnection(int client_socket, Ledger &ledger)
	{
		cout << "handleConnection() client_socket: " << client_socket << endl;
				
		char buffer[1024] = {0};
		while(true) {
			memset(buffer, 0, sizeof(buffer)); // zeroing the reception buffer.

			int bytes_read = read(client_socket, buffer, 1024); //perform read.

			if(bytes_read <= 0)
				break;
			cout << "Received: " << buffer << endl;
			
			// BOOTSTRAP CODE
			if(*my_port == bootstrap_port) {
				string register_cmd = "REGISTER";
			//parse the data.
				string command, peer_node_ip;
				int peer_node_port;
				//create a string stream for quick parsing.	
				istringstream ss(buffer);
				ss >> command >> peer_node_ip >> peer_node_port;
				if( command.compare(register_cmd) == 0)
				{
					total_peers +=1;
					peers[total_peers] = make_pair(peer_node_ip, peer_node_port);
					cout << "Added backbone peer node: " << peers[total_peers].first << " , " << peers[total_peers].second << endl;

				} else {
					cout << "received non-REGISTER command" << endl;
					break; //ignore otherwise	
				}	
			}		

		}	

		close(client_socket);	
	}

public:
	//constructor
	Node(const string& ip,int port) : node_ip(ip), port(port) {
		cout << "node() constructor port: " <<port<<endl;
		total_peers = 0; //initalization on 0. Bootstrap node, increments on REGISTER command.
				 // backbone nodes, increment when receiving PeerInfo from Bootstrap node.
	}

	int start()
	{
		cout << "test start()" << endl;


		if(*my_port != bootstrap_port)
			register_to_bootstrap();			

		struct sockaddr_in address;
		int addrlen = sizeof(address);


		if( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
			cout << "socket creation failed" << endl;
			cout << "retval: " << server_fd << endl;
			return -1;
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr(node_ip.c_str());
		address.sin_port = htons(port);

		//bind socket
		if( bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
			cout << "bind failed" << endl;
			return -1;
		}

		//listen
		if(listen(server_fd, 10) < 0) {
			cout << "Listen failed" << endl;
			return -1;
		}
		cout << "node running on port " << port << endl;

		if(port == bootstrap_port)
			cout << "BOOTSTRAP NODE operation" << endl;
		else
			cout << "Backbone node operation" <<  endl;

		//accept connections
		while(true) {
			int client_socket = accept(server_fd, (struct sockaddr *) & address, (socklen_t*)&addrlen);
			if(client_socket < 0) {
				cout << "Accept failed" << endl;
				continue; // just continue on failure.
			}
			//handle the new connection.
			//detach so it runs on parallel without 
			//blocking new connections etc.
			thread([this, client_socket]() { 
					this->handleConnection(client_socket, this->ledger);	
					}).detach();
		}
	}

};

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " <port>" << endl;
		return -1;
	}

	string ip = argv[1];	
	int port = stoi(argv[2]);
	my_port = (int *)malloc(sizeof(int));
	if(my_port == NULL)
	{
		cout << "malloc failed" << endl;
		return -1;
	}
	*my_port = port;
	cout << "running on port: " << *my_port << endl;

	
	Node node(ip,port);
	thread node_thread(&Node::start, &node);

	node_thread.join();	

	return (0);
}
