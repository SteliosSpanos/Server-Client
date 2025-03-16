#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

class Server{
private:
	int serverSocket, clientSocket;
	sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	void receiveMessages();
public:
	Server();
	~Server();
	void runServer();
	void sendMessage();
};

Server::Server() : clientSocket(-1){
	// Create Socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == -1){
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		exit(101);
	}

	// Bind Socket
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;
	if(bind(serverSocket, (sockaddr*)&address, addrlen) == -1){
		std::cerr << "bind() failed: " << strerror(errno) << std::endl;
		close(serverSocket);
		exit(102);
	}

	// Listen for Connection
	if(listen(serverSocket, 3) == -1){
		std::cerr << "listen() failed: " << std::endl;
		close(serverSocket);
		exit(103);
	}
}

Server::~Server(){
	if(clientSocket != -1){
		close(clientSocket);
	}
	close(serverSocket);
}

void Server::runServer(){
	std::cout << "Waiting for connections..." << std::endl;

	// Accept Client
	clientSocket = accept(serverSocket, (sockaddr*)&address, &addrlen);
	if(clientSocket == -1){
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		close(serverSocket);
		exit(104);
	}

	std::cout << "Connection Established!" << std::endl;

	// Start a Thread
	std::thread(&Server::receiveMessages, this).detach();
}

void Server::receiveMessages(){
	char buffer[BUFFER_SIZE];
	while(true){
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if(bytesRead == -1){
			std::cerr << "recv() failed: " << strerror(errno) << std::endl;
			break;
		}
		else if(bytesRead == 0){
			std::cout << "Client Disconnected" << std::endl;
			break;
		}
		buffer[bytesRead] = '\0';
		std::cout << "Message from client: " << buffer << std::endl;
	}
	close(clientSocket);
	clientSocket = -1;
}

void Server::sendMessage(){
	if(clientSocket != -1){
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
	}
	else{
		std::cerr << "Client Not Found" << std::endl;
	}
}

int main(){
	Server server;
	server.runServer();
	server.sendMessage();
	return 0;
}
