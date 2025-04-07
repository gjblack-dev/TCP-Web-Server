# Project 2: HTTPClient & HTTPServer

This project was created by **Gabriel Black** and **Andrew Pierpoint**. It is a lightweight HTTP client and server implemented in **C++** using TCP sockets. The client and server was developed and tested on **WSL (Windows Subsystem for Linux)**.

## Description

This project is a simple implementation of an HTTP client and server that follows the **TCP/IP and HTTP** protocols. The server listens for incoming client connections, processes **GET** requests, and serves static content.

## How to Compile and Run

This project uses a **Makefile** for easy compilation.

### Compilation:
1. Open a terminal and navigate to the project directory.
2. Run the following command to compile the program:
   ```sh
   make

3. To run the server use:
   ```sh
   ./httpServer

4. To run the client use:
   ```sh
   ./httpClient
   
   
## Accessing the Server

Once the server is running, you can use any web browser or HTTP client to send requests.

### Using the client app:
Once the client is running, you will be prompted to input an IP address.
If you are unsure of what your IP address is type `ipconfig` into the CMD terminal.
alternatively, you can use `127.0.0.1` which is the localhost IP address of your computer.
You will then be asked to supply a file to print to the terminal or you can send a message to the server using `msg:` followed by a message.

### In a Web Browser:
1. Open your preferred browser.
2. Enter one of the following URLs in the address bar: `http://localhost:60001` OR `http://<your_ip_address>:60001`

## Accomplishments
We successfully implemented an HTTP client and server using TCP/IP. Our implementation involves creating a socket, binding it to an IP address and port, and setting it up to listen for incoming connections. Once a client connects, we establish a client socket to handle communication. The server then processes incoming HTTP requests, specifically parsing for "GET /" and "POST /" requests to serve the appropriate HTML file and send messages to the server.

## Features

- Handles basic HTTP GET and HTTP POST requests
- Serves static HTML files
- Uses TCP sockets for communication
- Can communicate from client to server

## Dependencies

Make sure your system has the following installed:

- g++ (GCC C++ Compiler)
- Make
- WSL (if running on Windows)

## Authors

- Gabriel Black  
- Andrew Pierpoint

## License

This project was developed for educational purposes as part of a class assignment.  
It is open for personal and academic use but is not intended for commercial distribution.