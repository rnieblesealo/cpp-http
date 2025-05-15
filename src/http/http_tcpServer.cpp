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
    exit(1);
  }

  // configure this server's socket address (endpoint)
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port =
      htons(m_port); // htons to put in network byte order
  m_socketAddress.sin_addr.s_addr = inet_addr(
      m_ip_address.c_str()); // convert string ip to 32-bit numeric IP address

  // bind the socket address to the socket
  // note we need to pass the size of the address object
  if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0) {
    std::cerr << "failed to bind socket" << std::endl;
    exit(1);
  }

  return 0;
}

void TcpServer::closeServer() {
  // close sockets by getting rid of its file descriptor
  close(m_socket);
  close(m_new_socket);
  exit(0); // immediately terminates program and returns exit code
}

void TcpServer::startListen() {
  // listen for connections on our socket
  // 20 is the max number of connection requests in queue that haven't been
  // accepted we accept a conn. request with accept()
  if (listen(m_socket, 20) < 0) {
    std::cerr << "failed to bind socket" << std::endl;
    exit(1);
  }

  // log!
  std::ostringstream ss;
  ss << "\n*** Listening on ADDR: " << inet_ntoa(m_socketAddress.sin_addr)
     << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
  std::cout << ss.str();

  /*
   * inet_ntoa() -> 32-bit IPv4 to string
   * ntohs() -> network byte order port to host byte order
   */
}

void TcpServer::acceptConnection(SOCKET &new_socket) {
  // accept connection and store its endpoint (socket) address
  // this is the talkback socket!
  new_socket =
      accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss << "server failed to accept connection from ADDR: "
       << inet_ntoa(m_socketAddress.sin_addr)
       << "; PORT: " << ntohs(m_socketAddress.sin_port) << "\n";
    std::cerr << ss.str() << std::endl;
    exit(1);
  }

  const int BUFFER_SIZE = 30720;

  char buffer[BUFFER_SIZE]{0};
  auto bytesReceived = read(new_socket, buffer, BUFFER_SIZE);
  if (bytesReceived < 0) {
    std::cerr << "failed to read bytes from client socket" << std::endl;
    exit(1);
  }
}

} // namespace http
