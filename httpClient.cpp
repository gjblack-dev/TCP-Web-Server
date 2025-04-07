/*

 Project 2: HTTPServer and HTTPClient 
 COP4635 Systems and Networks II
 02/14/2025
 Created by Gabriel Black & Andrew Pierpoint

 This is a simple client application for the terminal that connects
 to the httpServer via TCP sockets.
 users can display a file in the terminal or send a message to the server.
 this program was created using the Windows subsystem linux(WSL) server 

*/




#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using std::cout;
using std::endl;
using std::cerr;
using std::string;

#define PORT 60001
#define BUFFER_SIZE 4096

// error handling function
void handleError(const string &msg){
    cerr << "ERROR: " << msg << endl;
    exit(EXIT_FAILURE);
}

// Create client side socket.
int createSocket(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) handleError("Can't create socket!");
    return sock;
}

// Connect to the server via sockets.
void connectToServer(int sock, const string& address){
    
    // define IPv4 address and ports
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    
    // converts the address string into a network addresss structure.
    if(inet_pton(AF_INET, address.c_str(), &server.sin_addr) <= 0){
         handleError("Invalid host name!");
    }

    // connect the client side to the server side.
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1){
        handleError("Connection failed!");
    }

    cout << "Connect to server: " << address  << " on port " << PORT << endl;
    cout << "\n\n";

}

// function that sends GET or POST request to the server.
void sendRequest(int sock, const string& content){
    string request;

    // sends a POST /message to the server when <msg:> is input on the client side.
    // or sends the GET /filename .
    if (content.substr(0,4) == "msg:")
    {
        string messageBody = content.substr(4);
        request = "POST /message HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(messageBody.length()) + "\r\n"
        "Connection: keep-alive\r\n\r\n" +
        messageBody;
    }else{

        request =  "GET /" + content + 
        " HTTP/1.1\r\nHOST: localhost\r\nConnection: keep-alive\r\n\r\n";

    }
    
    if(send(sock, request.c_str(), request.length(),0) == -1){
        handleError("Failed to send request!");
    }
}

// Receive the server acknowledgments.
void receiveResponse(int sock){
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
    if(bytesReceived == -1){
        handleError("Failed to receive response form server");
    } else if(bytesReceived == 0){
        handleError("Server closed the connection");
    } else {
        cout << "\nServer Response:\n" << string(buffer,bytesReceived) << endl;
    }
}




int main(){
    std::string serverAddress, fileName;

    // Get the server address once
    std::cout << "\nEnter server address: ";
    std::getline(std::cin, serverAddress);

    // Create and connect socket ONCE
    int sock = createSocket();
    connectToServer(sock, serverAddress);

    // First request before loop
    std::cout << "~ Enter file name to send to server\n"
                 "~ Send a message to the server with \"msg:\"\n"
                 "~ Close the client with \"exit\"\n"
                 "~ Shutdown the server with \"shutdown\"\n"
                 "Enter option - ";
    std::getline(std::cin, fileName);
    
    sendRequest(sock, fileName);
    receiveResponse(sock);

    // Loop for additional requests without reconnecting
    while (true) {

        std::cout << "\n~ Enter file name to send to server\n"
                     "~ Send a message to the server with \"msg:\"\n"
                     "~ Close the client with \"exit\"\n"
                     "~ Shutdown the server with \"shutdown\"\n"
                     "Enter option - ";
        std::getline(std::cin, fileName);

        if (fileName == "exit") break;  // Exit loop when user types "exit"

        sendRequest(sock, fileName);
        receiveResponse(sock);
    }

    // Close socket AFTER all requests are done
    close(sock);
    std::cout << "Client terminated." << std::endl;
    return 0;
}
