#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

class Client{
private:
	int clientSocket;
	sockaddr_in serverAddress;
	socklen_t addrlen = sizeof(serverAddress);
	void receiveMessages();
public:
	Client();
	~Client();
	void runClient();
	void sendMessage();
};

Client::Client(){
	// Create Socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket == -1){
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		exit(101);
	}

	// Converting IPv4 and IPv6 addresses from text to binary
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0){
		std::cerr << "Invalid Address: " << strerror(errno) << std::endl;
		close(clientSocket);
		exit(102);
	}
}

Client::~Client(){
	close(clientSocket);
}

void Client::runClient(){
	// Connectin to Server
	if(connect(clientSocket, (sockaddr*)&serverAddress, addrlen) == -1){
		std::cerr << "connect() failed: " << strerror(errno) << std::endl;
		close(clientSocket);
		exit(103);
	}

	std::cout << "Connected Successfuly!" << std::endl;

	// Start a Thread
	std::thread(&Client::receiveMessages, this).detach();
}

void Client::receiveMessages(){
	char buffer[BUFFER_SIZE];
	while(true){
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if(bytesRead == -1){
			std::cerr << "recv() failed: " << strerror(errno) << std::endl;
			break;
		}
		else if(bytesRead == 0){
			std::cout << "Server Closed Connection." << std::endl;
			break;
		}
		buffer[bytesRead] == '\0';
		std::cout << "Message from server: " << buffer << std::endl;
	}
	close(clientSocket);
}

void Client::sendMessage(){
	std::string message;
	while(true){
		std::cout << "Enter message(type 'exit' to exit): " << std::endl;
		std::getline(std::cin, message);
		if(message == "exit"){
			break;
		}
		if(send(clientSocket, message.c_str(), message.length(), 0) == -1){
			std::cerr << "send() failed: " << strerror(errno) << std::endl;
			break;
		}
		std::cout << "Message Sent!" << std::endl;
	}
	close(clientSocket);
}

int main(){
	Client client;
	client.runClient();
	client.sendMessage();
	return 0;
}
