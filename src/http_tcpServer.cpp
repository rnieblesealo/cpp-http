#include "http_tcpServer.h"

namespace {
// WARN: temp definition
std::string buildResponse() { return "..."; }
} // namespace

namespace http {

// initializer list helps set up members!
TcpServer::TcpServer(std::string ip_address, int port)
    : m_ip_address(ip_address), m_port(port), m_socket(),
      m_socketAddress_len(sizeof(m_socketAddress)),
      m_serverMessage(buildResponse()) {
  startServer();
}

TcpServer::~TcpServer() { closeServer(); }

int TcpServer::startServer() {
  // create socket and store file descriptor
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0) {
    std::cerr << "could not create socket" << std::endl;
    return 1;
  }

  return 0;
}

void TcpServer::closeServer() {
  // close sockets by getting rid of its file descriptor
  close(m_socket);
  close(m_new_socket);
  exit(0); // immediately terminates program and returns exit code
}

} // namespace http
