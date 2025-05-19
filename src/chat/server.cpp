#include "helper.h"
#include <arpa/inet.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <mutex>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>

#define MESSAGE_LEN 1024

std::mutex stdout_mutex;

void handleClient(int clientSocket, sockaddr_in clientAddress) {
  std::string clientIP = inet_ntoa(clientAddress.sin_addr);
  int clientPort = ntohs(clientAddress.sin_port);

  char buffer[MESSAGE_LEN]{0};
  while (true) {
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    // on client leave
    // WARN: also runs on terminated client connection! careful
    if (bytesReceived <= 0) {
      std::lock_guard<std::mutex> lock(stdout_mutex);
      fmt::print(stdout, fmt::emphasis::bold | fg(fmt::color::yellow),
                 "{} left!\n", clientIP);
      break;
    }

    buffer[bytesReceived] = '\0'; // nullterm buf ALWAYS before print
    std::lock_guard<std::mutex> lock(stdout_mutex);
    fmt::print(stdout, fmt::emphasis::bold | fg(fmt::color::yellow),
               "[{}:{}]: ", clientIP, clientPort);
    fmt::print(stdout, "{}\n", buffer);
  }

  close(clientSocket);
}

int main() {
  const int PORT = 8080;

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  serverAddress.sin_addr.s_addr =
      INADDR_ANY; // listen on all addresses this machine goes by

  if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) <
      0) {
    helper::exitWithError("Failed to bind socket!");
  }

  fmt::print(stdout, fmt::emphasis::bold | fg(fmt::color::green),
             "Listening on port {}:\n", PORT);

  while (true) {
    // allocate
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    // write to
    int clientSocket =
        accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressLen);

    // thread if ok
    if (clientSocket >= 0) {
      std::thread(handleClient, clientSocket, clientAddress);
    }
  }
}
