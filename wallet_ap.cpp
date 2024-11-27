#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

class Wallet {

private:
	string wallet_name;
	// Helper function to send a transaction
	void sendTransaction(int socket, const string& transaction) {
		// Send transaction to node
		send(socket, transaction.c_str(), transaction.size(), 0);

		// Receive response from node
		char buffer[1024] = {0};
		int bytes_received = read(socket, buffer, 1024);
		if (bytes_received > 0) {
			cout << "Node Response: " << buffer << endl;
		} else {
			cout << "No response from node.\n";
		}
	}


public:
	//constructor
	Wallet(const string& name): wallet_name(name) {
	}

	void connectToNode(const string& node_ip, int node_port) {
		int client_socket;
		struct sockaddr_in server_address;

		// Create socket
		if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			cerr << "Socket creation failed\n";
			return;
		}

		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(node_port);

		// Convert IP address to binary form
		if (inet_pton(AF_INET, node_ip.c_str(), &server_address.sin_addr) <= 0) {
			cerr << "Invalid IP address\n";
			return;
		}

		// Connect to node
		if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
			cerr << "Connection to node failed\n";
			return;
		}

		cout << "Connected to node at " << node_ip << ":" << node_port << endl;

		// Wallet interaction loop
		string receiver;
		int amount;

		while (true) {
			cout << "Enter receiver name (or 'exit' to quit): ";
			cin >> receiver;
			if (receiver == "exit") break;

			cout << "Enter amount to send: ";
			cin >> amount;

			// Construct transaction
			string transaction = "TRANSACTION " + wallet_name + " " + receiver + " " + to_string(amount);
			sendTransaction(client_socket, transaction);
		}

		// Close the socket
		close(client_socket);
		cout << "Disconnected from node.\n";
	}

};
//main function
int main(int argc, char* argv[]) {
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " <wallet_name> <node_ip> <node_port>\n";
		return EXIT_FAILURE;
	}

	string wallet_name = argv[1];
	string node_ip = argv[2];
	int node_port = stoi(argv[3]);

	Wallet wallet(wallet_name);

	//connects to backbone node.
	wallet.connectToNode(node_ip, node_port);

	return 0;
}

