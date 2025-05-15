#include "helper.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  // make client socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  // define server addr
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(8080);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  // connect to server
  if (connect(clientSocket, (struct sockaddr *)&serverAddress,
              sizeof(serverAddress)) < 0) {
    helper::exitWithError("failed to connect to server");
  }

  std::string message =
      "lutron ftw"; // msg to server, NOTE: use of cpp strings!

  // send msg to server
  send(clientSocket, message.c_str(), message.size(),
       0); // NOTE: how we convert cpp string -> c string!

  // close client socket
  close(clientSocket);

  return 0;
}
