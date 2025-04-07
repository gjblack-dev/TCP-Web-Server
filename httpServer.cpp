
/*
 Project 2: HTTPServer and HTTPClient 
 COP4635 Systems and Networks II
 02/14/2025
 Created by Gabriel Black & Andrew Pierpoint

 This program is a simple implementation of a http server
 using TCP socket programming and HTTP request parsing
 this program was created using the Windows subsystem linux(WSL) server 
 */



#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>

#define PORT 60001
#define BUFFER_SIZE 4096

//funtion prototypes
void handleError(const std::string&, int);
int setupServerSocket();
int acceptClient(int, sockaddr_in&);
void handleRequest(int, int);

//main first for better readability
int main(){

	std::cout << "Starting Server...\n" << std::endl;

    //create the socket
    int socket = setupServerSocket();

    
    while(true){
        sockaddr_in client;
        int clientSocket = acceptClient(socket, client);
        handleRequest(clientSocket, socket);
    }
    
    close(socket);
    std::cout << "Server Closed.\n";
    return 0;
}

// handles error messages
void handleError(const std::string& msg, int socket){
    if (socket < 0 ){
        std::cerr << "ERROR: " << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

// function that creates the socket binds and listens for the client socket
int setupServerSocket(){

    //create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) handleError("Can't connect to socket!", sock);
    
    //Bind the socket to a IP / port
    sockaddr_in hint; 
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    int binding = bind(sock, (sockaddr*)&hint, sizeof(hint));
    if(binding == -1) handleError("Can't bind to IP/Port!", binding);
    
    //mark the socket for listening in
    int listening =listen(sock, SOMAXCONN);
    if (listening == -1) handleError("Can't Listen", listening);
    
    std::cout << "Server started listing on PORT: " << PORT << std::endl;
    return sock;
}

//function to create and accpept the client
int acceptClient(int sock, sockaddr_in& client){
    //accept a call
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(sock,(sockaddr*)&client,  &clientSize);
    if(clientSocket == -1) handleError("Can't connect to client!", clientSocket);
   

    //initialize host and svc with 0's
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0 , NI_MAXSERV);

    //getnameinfo finds the host and service names and returns an integer
    int result = getnameinfo((sockaddr*)&client, 
                             sizeof(client),
                             host, 
                             NI_MAXHOST,
                             svc,
                             NI_MAXSERV,
                             0);



    if(result)
    {
        std::cout << host << "connected on " << svc << std::endl;
    } else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << "connected on " << client.sin_port << std::endl;
    }
    return clientSocket;
}

//function to parse the http request and send data to client
void handleRequest(int clientSocket, int sock){
    char buffer[BUFFER_SIZE];
    
    while (true) {

        // initialize buffer with 0's
        memset(buffer, 0, BUFFER_SIZE);

        //recieve bytes from http server
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        
        if (bytesReceived == -1){
            handleError("connection error with client", bytesReceived);
        }else if(bytesReceived <= 0){
            std::cout << "Client Disconnected..." << std::endl;
            break;
        }
       
        //create a string that initializes to the size of the message sent from clienta
        std::string request(buffer, bytesReceived);
        std::cout << "Request Received:\n" << request << std::endl;
        

        // If client sends "exit", the server will close the connection
        if (request.find("shutdown") != std::string::npos){
            std::cout << "Shutdown request recieved. Stopping server.\n";
            close(sock);
            exit(EXIT_SUCCESS);
        } 

        if(request.substr(0,5) == "POST "){
            if(request.find("POST /message") != std::string::npos){

                //extract message body
                size_t bodyStart = request.find("\r\n\r\n") + 4;
                std::string messageBody = request.substr(bodyStart);

                std::cout << "Client message: " << messageBody << std::endl;

                // send acknowledgement response
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 26\r\n\r\n"
                    "SERVER ACK: Message received."; 

                send(clientSocket, response.c_str(), response.size(), 0);
                continue;
            }
        } else if(request.substr(0, 4) != "GET ") {
            std::string errorResponse = 
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 23\r\n\r\n"
                "405 Method Not Allowed";
            send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
            continue;
        }



        // Extract requested file name
        size_t start = request.find("GET /") + 5;
        size_t end = request.find(" ", start);
        std::string fileName = request.substr(start, end - start);
        
        if (fileName.empty() || fileName == "/") fileName = "index.html";
        std::cout << "Requested file: " << fileName << std::endl;

        // Serve the requested file
        std::ifstream file(fileName);
        if (!file.is_open()) {
            std::string errorResponse = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n\r\n"
                "404 Not Found";
            send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
            continue;
        }

        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        std::string httpResponse = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(fileContent.size()) + "\r\n\r\n" + fileContent;

        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);


        // Check if request contains "connection: close", close connection
        if(request.find("Connection: close") != std::string::npos){
            std::cout << "Client requested to close connection.\n";
            break;
        }
    }

    close(clientSocket);

}
