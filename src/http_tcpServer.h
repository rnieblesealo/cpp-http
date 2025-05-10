/*
 * THE SERVER:
 * 1. listens for conn reqs and puts them on queue
 * 2. accepts network conns 1 at a time and puts them on queue
 * 3. reads request data sent from client
 * 4. sends response back to client
 */

#ifndef HTTP_TCPSERVER
#define HTTP_TCPSERVER

#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace http {

class TcpServer {
public:
  TcpServer(std::string ip_address, int port);
  ~TcpServer();

private:
  std::string m_ip_address;
  int m_port;
  int m_socket;
  int m_new_socket;
  long m_incomingMessage;
  struct sockaddr_in m_socketAddress;
  unsigned int m_socketAddress_len;
  std::string m_serverMessage;

  int startServer();
  void closeServer();
};

} // namespace http

#endif
