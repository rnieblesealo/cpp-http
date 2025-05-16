#include "helper.h"
#include <arpa/inet.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

std::mutex log_mutex;

void handleClient(int clientSocket, sockaddr_in clientAddress) {
  // get client ip, port in human-readable form
  std::string clientIP = inet_ntoa(clientAddress.sin_addr);
  int clientPort = ntohs(clientAddress.sin_port);

  char buffer[1024]{0};
  while (true) {
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    // if fail to receive or client connection terminates...
    if (bytesReceived <= 0) {
      // lock so that only one thread can write to stdout at a time...
      std::lock_guard<std::mutex> lock(log_mutex);

      // ...actually write to stdout...
      fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                 "{} connection terminated!\n", clientIP);

      // ...and voila! the unlock will happen automatically when this block ends
      break;
    }

    buffer[bytesReceived] = '\0'; // null-terminate received string

    std::lock_guard<std::mutex> lock(log_mutex); //

    // print the received message
    fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
               "[ {} : {} ]: ", clientIP, clientPort);
    fmt::print("{}\n", buffer);
  }

  close(clientSocket);
}

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

  // listen for connections, keep as many as possible in queue
  listen(serverSocket, SOMAXCONN);

  fmt::print(fmt::emphasis::bold | fg(fmt::color::green),
             "Listening on port {}:\n", 8080); // TODO: dont hardcode port

  while (true) {
    // declare client address
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    // accept connection and write client address
    int clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddress,
               &clientAddressLen); // NOTE: returns file descriptor for this
                                   // talkback socket!

    // if we accepted something (got a file descriptor)
    if (clientSocket >= 0) {
      // make new thread to handle this connection
      std::thread(handleClient, clientSocket, clientAddress)
          .detach(); // NOTE: detach makes it independent of the main thread
    }
  }

  close(serverSocket);

  // NOTE: RESUME HERE: wiping threads on connection close

  return 0;
}
