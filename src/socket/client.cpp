#include "helper.h"
#include <fmt/color.h>
#include <fmt/core.h>
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

  std::string input;
  fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow), "Enter message:\n");

  while (true) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow), "> ");

    // fail to capture
    if (!std::getline(std::cin, input)) {
      helper::exitWithError("input stream error/eof");
      break;
    }

    // exiting
    if (input == "/quit") {
      fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Exiting...\n");
      break;
    }

    // normal case

    // send to server
    ssize_t bytesSent = send(clientSocket, input.c_str(), input.size(), 0);
    if (bytesSent < 0) {
      helper::exitWithError("failed to send message");
    }

    // check for conn. terminate
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == 0) {
      fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                 "Server closed connection! Exiting...\n");
      break;
    } else if (bytesReceived < 0) {
      helper::exitWithError("recv() failed");
    }
  }

  // close client socket
  close(clientSocket);

  return 0;
}
