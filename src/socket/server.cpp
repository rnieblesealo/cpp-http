#include "helper.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080); // put port in network byte order
  serverAddress.sin_addr.s_addr =
      INADDR_ANY; // listen for connections on any of this machine's IPs

  // bind socket to address
  if (bind(serverSocket, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) < 0) {
    helper::exitWithError("failed to bind socket!");
  }

  // listen for connections, max 5 in queue
  listen(serverSocket, 5);

  // accept connection
  /* params:
   * 1. listening socket
   * 2. where to write self-identifying info about client; is a sockaddr struct
   * 3. size of the passed sockaddr struct
   */
  int clientSocket = accept(serverSocket, nullptr, nullptr);

  // receive client data, writing it to a buffer
  char buf[1024]{0};
  recv(clientSocket, buf, sizeof(buf), 0);

  // print it
  std::cout << "[client] \"" << buf << "\"" << std::endl;

  // close the server socket fd
  close(serverSocket);

  return 0;
}
