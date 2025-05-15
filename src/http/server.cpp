#include "http_tcpServer.h"

int main() {
  using namespace http;
  // note IP "0.0.0.0" tells OS to listen on any network interface (localhost,
  // wifi, ethernet, etc.)
  TcpServer server = TcpServer("0.0.0.0", 8080);
  return 0;
}
