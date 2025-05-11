Notes for barebones C++ HTTP server project

# Connection Stuff

**HTTP:** Structures data itself; *what* we send (headers, body, etc.)

**TCP:** Determines *how* data is sent (packets, ACK, etc.)

**Port:** Identifies specific service on system
- IP is street address
- Port is apartment number

**Socket**: Connection endpoint, a socket pair is defined by this combination:
```
<local ip>:<local port> <--> <remote ip>:<remote port>
```
> Individual connections are identified by this combo!
1. Server listens for connections on a port
2. Client needs the server's IP (unique identifier) and the port number to attempt connection
    - *Public IP* is assigned by ISP; centrally managed, no dupes; bound to your router
    - *Private IP* is assigned to devices by the router using DHCP
3. Client must also identify itself for server to know socket pair 
4. On connection OK, new socket in server is created to talk back to client
5. At this point we have 2 socket pair which are inverses of each other:
6. A socket can then be written to using a file descriptor (kinda like how pipes work!)
```
following the <IP>:<PORT> format,

if client's conn sock is:
    LOCAL = B:2
    REMOTE = A:1

then server's talkback sock is:
    LOCAL = A:1
    REMOTE = B:2
```
- Port tells TCP what app to send data to

**Address Family:** *What kind of addresses are we working with?* Tells us what IP looks like.
- `AF_INET` = IPv4 = 32-bit IP, e.g. `192.168.0.1`
- `AF_INET6` = IPv6 = 128-bit IP, e.g. `2001:0db8::1`
> Bitness difference is scale; IPv4 supports ~4.3 billion addresses which seemed like plenty back then! IPv6 allows for many, many more

# CPP

### Namespaces

`namespace` groups related code
```
namespace Audio {
    void log(){
        std::cout << "Audio!\n";
    }
}

namespace Video {
    void log(){
        std::cout << "Video!\n";
    }
}

Audio::log();   // prints "Audio"
Video::log();   // prints "Video"
```
- `using namespace <namespace>` allows us to use things in a given namespace without prefixing
    - So for prev. example, it'd be `log()` instead of `Audio::log()`
    - **This can create ambiguity conflicts though!** Use wisely.

### Include Guards

`#ifndef <header> #define <header>` is an include guard that avoids processing the same header twice 
- If we didn't do that, including the file twice would throw something like `redefinition of ...` referring to stuff inside the header

### Include Brackets or Quotes?

`#include ""` looks for header in project dir first, then system

`#include <>` looks in system only 

### Initializer Lists

Belong to constructors; they initialize class members before constructor body runs
```
e.g.

-- tcpServer.hpp (definition) ---------------------------

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

-- tcpServer.cpp (implementation) ----------------------

TcpServer::TcpServer(std::string ip_address, int port)
    : m_ip_address(ip_address), m_port(port), m_socket(),
      m_socketAddress_len(sizeof(m_socketAddress)),
      m_serverMessage(buildResponse()) {
  // this is the actual body!
}
```
> All the `TcpServer` class' members are being initialized here
- Initializer list is required for const members
- They avoid default construction (which sets stuff to garbage/undefined values) only to then (possibly) assign a new one in the constructor body
    - *We avoid the unnecessary extra memory/calls with these!*

### `sockaddr_in` Caveats

```
definition of one IP + port endpoint (a full socket connection consists of 2 such endpoints):

struct sockaddr_in {
    short           sin_family    // address family
    unsigned short  sin_port      // port, use hton() for network byte order
    struct          in_addr       // the IP, see below...
    char            sin_zero[8]   // padding
}

struct in_addr {
    unsigned long   s_addr        // IPv4 for AF_INET
}
```
- `in_addr`, the IP, is its own struct mainly for self-documenting code 
    - IPs are dinstinct enough that we treat them as their own type 
- **Padding** = insert extra bytes to align things 
    - Usually for performance
    - *Not in this case!*
        - `struct sockaddr` (diff. from `sockaddr_in`) is a more generic endpoint structure 
        - To safely use `sockaddr_in` where `sockaddr` is expected, we use padding so their sizes and layouts match

# References

- [Medium C++ HTTP server tutorial](https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7)
- [Oracle socket docs](https://docs.oracle.com/javase/tutorial/networking/sockets/definition.html)
