# simple Makefile to compile server & client
# Made by Gabriel Black & Andrew Pierpoint
# For Project 2: HTTP Client and Server

all: httpServer httpClient

# compiles with necessary flags -g -Wall
server: server.cpp
	g++ -g -Wall -std=c++17 -o httpServer httpServer.cpp

client: client.cpp
	g++ -g -Wall -std=c++17 -o httpClient httpClient.cpp

# simple clean 
clean:
	rm -f httpServer httpClient
