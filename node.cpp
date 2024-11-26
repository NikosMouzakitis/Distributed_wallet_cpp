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
#include <cstdlib> // malloc
#include "ledger.h" //Ledger/Transaction classes
using namespace std;

int *my_port;




class Node {

private:
	string node_ip;
	int port;
	map <int, pair<string, int>> peers; //Map of peer sockets (ip,port)
	int server_fd;
	Ledger ledger; //local copy of the Ledger in the node.

	static void handleConnection(int client_socket, Ledger &ledger)
	{
		cout << "handleConnection() client_socket: " << client_socket << endl;

		

		close(client_socket);	
	}

public:
	//constructor
	Node(const string& ip,int port) : node_ip(ip), port(port) {
		cout << "node() constructor port: " <<port<<endl;
	}

	int start()
	{
		cout << "test start()" << endl;
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
			thread(handleConnection, client_socket, ref(ledger)).detach();
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





