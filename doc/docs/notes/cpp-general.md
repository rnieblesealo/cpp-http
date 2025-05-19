---
sidebar_position: 1
---

# CPP - Cool Features

### Namespaces

`namespace` groups related code

```cpp
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

```cpp
#ifndef HEADER
#define HEADER

#include <something.h>

namespace something {
    void doSomething();
}

#endif
```

Include guard avoids processing the same header twice 
- If we didn't do that, including the file twice would throw redefinition errors 

### Include Brackets or Quotes?

`#include ""` looks for header in project dir first, then system

`#include <>` looks in system only 

### Initializer Lists

Belong to constructors; they initialize class members before constructor body runs

```cpp title="tcpServer.h"
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
```

```cpp title="tcpServer.cpp"
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

### `std::osstringstream`

In-memory string builder, call `ss.str()` to convert to printable string.

```cpp
  std::ostringstream ss;

  ss << "\n*** Listening on ADDR: " 
     << inet_ntoa(m_socketAddress.sin_addr)
     << " PORT: " << ntohs(m_socketAddress.sin_port) 
     << " ***\n\n";

  std::cout << ss.str();
```

### String Literals w/`const char*`

String literals (any strings we hardcode) are ALWAYS of type `const char*`
> `const` in the param says "this value should NOT be modified inside the function!"
