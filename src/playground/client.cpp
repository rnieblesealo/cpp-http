#include "helper.h"
#include <atomic>
#include <fmt/color.h>
#include <fmt/core.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

std::atomic<bool> running{true}; // atomic = thread-safe

// one thread to get input
void handleInput(int clientSocket) {
  while (running.load()) { // NOTE:
                           // {atomicval}.load() to get,
                           // {atomicval}.store({new}) to set
    fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow), "> ");

    std::string input;

    // input obtain fail
    if (!std::getline(std::cin, input)) {
      close(clientSocket);
      helper::exitWithError("input stream error/eof");
    }

    // exiting
    if (input == "/quit") {
      fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Exiting...\n");
      running.store(false);
      break;
    }

    // normal case; we send to server
    ssize_t bytesSent = send(clientSocket, input.c_str(), input.size(), 0);
    if (bytesSent < 0) {
      close(clientSocket);
      helper::exitWithError("failed to send message");
    }
  }
}

// one thread to listen for close
void checkTerminate(int clientSocket) {
  // check for conn. terminate
  while (running.load()) {
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == 0) {
      fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                 "Server closed connection! Exiting...\n");

      // close client socket
      close(clientSocket);

      // exit
      exit(EXIT_SUCCESS);
    } else if (bytesReceived < 0) {
      helper::exitWithError("recv() failed");
    }
  }
}

int main() {
  // make client socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    helper::exitWithError("failed to create socket");
  }

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

  // start threads
  std::thread input(handleInput, clientSocket);
  std::thread check(checkTerminate, clientSocket);

  // terminate program when both threads finish
  // ergo, both threads exit their loops
  // if one ends, the other ends too, really; they both depend on bool
  // running
  input.join();
  check.join();

  // close client socket
  close(clientSocket);

  return 0;
}
