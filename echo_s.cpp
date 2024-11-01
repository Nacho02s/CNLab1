// **************************************************************************************
// * Echo Strings (echo_s.cc)
// * -- Accepts TCP connections and then echos back each string sent.
// **************************************************************************************
#include "echo_s.h"


// **************************************************************************************
// * processConnection()
// * - Handles reading the line from the network and sending it back to the client.
// * - Returns true if the client sends "QUIT" command, false if the client sends "CLOSE".
// **************************************************************************************
int processConnection(int sockFd) {

  bool quitProgram = true;
  bool keepGoing = true;
  while (keepGoing) {

    //
    // Call read() call to get a buffer/line from the client.
    // Hint - don't forget to zero out the buffer each time you use it.
    //

    //
    // Check for one of the commands
    //

    //
    // Call write() to send line back to the client.
    //

  }

  return quitProgram;
}
    


// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
// **************************************************************************************

int main (int argc, char *argv[]) {

  // ********************************************************************
  // * Process the command line arguments
  // ********************************************************************
  int opt = 0;
  while ((opt = getopt(argc,argv,"d:")) != -1) {
    
    switch (opt) {
    case 'd':
      LOG_LEVEL = std::stoi(optarg);;
      break;
    case ':':
    case '?':
    default:
      std::cout << "useage: " << argv[0] << " -d <num>" << std::endl;
      exit(-1);
    }
  }

  // *******************************************************************
  // * Creating the inital socket is the same as in a client.
  // ********************************************************************
  int     listenFd = -1;
  // Call socket() to create the socket you will use for lisening.

  
  // ********************************************************************
  // * The bind() and calls take a structure that specifies the
  // * address to be used for the connection. On the cient it contains
  // * the address of the server to connect to. On the server it specifies
  // * which IP address and port to lisen for connections.
  // ********************************************************************
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  // *** assign 3 fields in the servadd struct sin_family, sin_addr.s_addr and sin_port
  // *** the value your port can be any value > 1024.


  // ********************************************************************
  // * Binding configures the socket with the parameters we have
  // * specified in the servaddr structure.  This step is implicit in
  // * the connect() call, but must be explicitly listed for servers.
  // ********************************************************************
  bool bindSuccesful = false;
  while (!bindSuccesful) {
    // ** Call bind()
    // You may have to call bind multiple times if another process is already using the port
    // your program selects.
  }
  // *** DON'T FORGET TO PRINT OUT WHAT PORT YOUR SERVER PICKED SO YOU KNOW HOW TO CONNECT.


  // ********************************************************************
  // * Setting the socket to the listening state is the second step
  // * needed to being accepting connections.  This creates a queue for
  // * connections and starts the kernel listening for connections.
  // ********************************************************************
  int listenQueueLength = 1;
  // ** Cal listen()

  // ********************************************************************
  // * The accept call will sleep, waiting for a connection.  When 
  // * a connection request comes in the accept() call creates a NEW
  // * socket with a new fd that will be used for the communication.
  // ********************************************************************
  bool quitProgram = false;
  while (!quitProgram) {
    int connFd = 0;

    // Call the accept() call to check the listening queue for connection requests.
    // If a client has already tried to connect accept() will complete the
    // connection and return a file descriptor that you can read from and
    // write to. If there is no connection waiting accept() will block and
    // not return until there is a connection.
    // ** call accept() 
    
    // Now we have a connection, so you can call processConnection() to do the work.
    quitProgram = processConnection(connFd);
   
    close(connFd);
  }

  close(listenFd);

}
