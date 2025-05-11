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

  /* definition of one IP + port endpoint
   * (a full socket connection consists of 2 such endpoints):
   *
   * struct sockaddr_in {
   *  short           sin_family    // address family
   *  unsigned short  sin_port      // port, use hton() for network byte order
   *  struct          in_addr       // the IP, see below...
   *  char            sin_zero[8]   // padding
   * }
   *
   * struct in_addr {
   *  unsigned long   s_addr        // IPv4 for AF_INET
   * }
   */

  unsigned int m_socketAddress_len;
  std::string m_serverMessage;

  int startServer();
  void closeServer();
};

} // namespace http

#endif
