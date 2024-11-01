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
    bool quitProgram = false;
    bool keepGoing = true;

    while (keepGoing) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));

        // Call read() to get a buffer/line from the client.
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            perror("Read error");
            return false;
        }

        // Check for one of the commands
        if (strncmp(buffer, "QUIT", 4) == 0) {
            quitProgram = true;
            keepGoing = false;
        } else if (strncmp(buffer, "CLOSE", 5) == 0) {
            keepGoing = false;
        } else {
            // Call write() to send line back to the client.
            write(sockFd, buffer, bytesRead);
        }
    }

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

    // *******************************************************************
    // * Creating the initial socket is the same as in a client.
    // ********************************************************************
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

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
        } else {
            perror("Bind failed, retrying on a different port");
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
        exit(EXIT_FAILURE);
    }

    // ********************************************************************
    // * The accept call will sleep, waiting for a connection. When
    // * a connection request comes in, the accept() call creates a NEW
    // * socket with a new fd that will be used for the communication.
    // ********************************************************************
    bool quitProgram = false;
    while (!quitProgram) {
        int connFd = accept(listenFd, NULL, NULL);
        if (connFd < 0) {
            perror("Accept failed");
            continue;
        }

        quitProgram = processConnection(connFd);
        close(connFd);
    }

    close(listenFd);
    return 0;
}
