// **************************************************************************************
// * Echo Strings (echo_s.cc)
// * -- Accepts TCP connections and then echos back each string sent.
// **************************************************************************************
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "echo_s.h"
#include "logging.h"  

#define DEFAULT_PORT 12345
#define BUFFER_SIZE 1024

// **************************************************************************************
// * processConnection()
// * - Handles reading the line from the network and sending it back to the client.
// * - Returns true if the client sends "QUIT" command, false if the client sends "CLOSE".
// **************************************************************************************
int processConnection(int sockFd) {
    INFO << "Processing new connection" << ENDL;

    bool quitProgram = false;
    bool keepGoing = true;

    while (keepGoing) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // Call read() to get a buffer/line from the client.
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            perror("Read error");
            ERROR << "Failed to read from socket" << ENDL;
            return false;
        }

        INFO << "Received data: " << buffer << ENDL;

        // Check for one of the commands
        if (strncmp(buffer, "QUIT", 4) == 0) {
            INFO << "Received QUIT command, exiting connection" << ENDL;
            quitProgram = true;
            keepGoing = false;
        } else if (strncmp(buffer, "CLOSE", 5) == 0) {
            INFO << "Received CLOSE command, closing connection" << ENDL;
            keepGoing = false;
        } else {
            // Call write() to send line back to the client.
            INFO << "Echoing data back to client" << ENDL;
            write(sockFd, buffer, bytesRead);
        }
    }

    INFO << "Closing connection" << ENDL;
    return quitProgram;
}


// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
// **************************************************************************************

int main(int argc, char *argv[]) {

    // ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    int opt = 0;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                break;
            case ':':
            case '?':
            default:
                std::cout << "Usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    INFO << "Starting Echo Server" << ENDL;

    // *******************************************************************
    // * Creating the initial socket is the same as in a client.
    // ********************************************************************
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        perror("Socket creation failed");
        FATAL << "Socket creation failed" << ENDL;
        exit(EXIT_FAILURE);
    }

    INFO << "Socket created successfully" << ENDL;

    // ********************************************************************
    // * The bind() and calls take a structure that specifies the
    // * address to be used for the connection. On the client it contains
    // * the address of the server to connect to. On the server it specifies
    // * which IP address and port to listen for connections.
    // ********************************************************************
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(DEFAULT_PORT);

    bool bindSuccessful = false;
    while (!bindSuccessful) {
        if (bind(listenFd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0) {
            bindSuccessful = true;
            INFO << "Socket bound to port " << ntohs(servaddr.sin_port) << ENDL;
        } else {
            perror("Bind failed, retrying on a different port");
            WARNING << "Bind failed on port " << ntohs(servaddr.sin_port) << ", retrying on next port" << ENDL;
            servaddr.sin_port = htons(ntohs(servaddr.sin_port) + 1);  // Increment port
        }
    }

    printf("Using port: %d\n", ntohs(servaddr.sin_port));

    // ********************************************************************
    // * Setting the socket to the listening state is the second step
    // * needed to begin accepting connections. This creates a queue for
    // * connections and starts the kernel listening for connections.
    // ********************************************************************
    int listenQueueLength = 1;
    if (listen(listenFd, listenQueueLength) < 0) {
        perror("Listen failed");
        FATAL << "Failed to set socket to listening state" << ENDL;
        exit(EXIT_FAILURE);
    }

    INFO << "Listening for connections..." << ENDL;

    // ********************************************************************
    // * The accept call will sleep, waiting for a connection. When
    // * a connection request comes in, the accept() call creates a NEW
    // * socket with a new fd that will be used for the communication.
    // ********************************************************************
    bool quitProgram = false;
    while (!quitProgram) {
        INFO << "Waiting for new connection..." << ENDL;
        int connFd = accept(listenFd, NULL, NULL);
        if (connFd < 0) {
            perror("Accept failed");
            ERROR << "Failed to accept connection" << ENDL;
            continue;
        }

        INFO << "Accepted new connection" << ENDL;
        quitProgram = processConnection(connFd);
        close(connFd);
        INFO << "Connection closed" << ENDL;
    }

    INFO << "Shutting down server" << ENDL;
    close(listenFd);
    return 0;
}
