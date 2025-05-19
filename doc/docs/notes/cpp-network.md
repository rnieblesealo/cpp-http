---
sidebar_position: 2
---

# CPP - Networking

### `sockaddr_in`

Defines an address.
```cpp title="address-definition.cpp"
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

```cpp title="address-usage.cpp"
sockaddr_in serverAddress;

serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(8080);           // put port in network byte order
serverAddress.sin_addr.s_addr = INADDR_ANY;     // listen for connections on any of this machine's IPs
```

- `in_addr`, the IP, is its own struct mainly for self-documenting code 
    - IPs are dinstinct enough that we treat them as their own type 
- Padding allows us to align struct bytes for compat. with `sockaddr` 
    - `sockaddr_in` extensively defines IPv4 address; `sockaddr` generically defines an address from any family and is what is ultimately passed to `bind()`, `connect()`, etc.
    - To safely use `sockaddr_in` where `sockaddr` is expected, we use padding so their sizes and layouts match

### `recv()`

Receives bytes from the server; 99% of time we want to put this in buffer

```cpp
char buffer[1024];
ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
```

- Behaves like `accept()`; what we haven't `recv()`'d will be buffered by the kernel:
    1. TCP accepts and acknowledges incoming packets
    2. Packets are stored in kernel buffer
    3. Data waits there until we call `recv()`
- If OK, returns number of bytes received
- If server gracefully closed connection, returns `0`
- If server connection gets terminated, returns `< 0`
