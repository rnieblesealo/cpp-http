Notes for barebones C++ HTTP server project

# Sockets/TCP/Connection/etc.

**HTTP:** Structures data itself; *what* we send (headers, body, etc.)

**TCP:** Determines *how* data is sent (packets, ACK, etc.)

**Port:** Identifies specific service on system
- IP is street address
- Port is apartment number

**Socket**: Connection endpoint, a socket is defined as `<ip>:<port>` 

**Address Family:** *What kind of addresses are we working with?* Tells us what IP looks like.
- `AF_INET` = IPv4 = 32-bit IP, e.g. `192.168.0.1`
- `AF_INET6` = IPv6 = 128-bit IP, e.g. `2001:0db8::1`
> Bitness difference is scale; IPv4 supports ~4.3 billion addresses which seemed like plenty back then! IPv6 allows for many, many more

### How a Connection Works

```
<local ip>:<local port> <--> <remote ip>:<remote port>
```

These are 2 sockets in a pair, a *socket pair*

A socket pair forms a **connection!**

1. Server listens for connections on a port
2. Client needs the server's IP (unique identifier) and the port number to attempt connection
    - *Public IP* is assigned by ISP; centrally managed, no dupes; bound to your router
    - *Private IP* is assigned to devices by the router using DHCP
3. Client must also identify itself for server to know socket pair 
4. On connection OK, new socket in server is created to talk back to client
5. At this point we have 2 socket pair which are inverses of each other:
```
following the <IP>:<PORT> format,

if client's conn sock is:
    LOCAL = B:2
    REMOTE = A:1

then server's talkback sock is:
    LOCAL = A:1
    REMOTE = B:2
```
6. A socket can then be written to using a file descriptor (kinda like how pipes work!)
- Sockets are places where data exchange is meant to happen, this is why they have an associated fd
- Calling `socket()` creates this place, but we need to `bind()` it to an addr. so any exchange can begin to occur
- In other words, a blank socket is like a pipe that isn't hooked up yet 
- Port tells TCP what app to send data to

### Why Connecting Client Doesn't Need to Bind

Server must listen on well-known IP and port **so client knows where to reach it.**

Client isn't listening, it's just reaching out.

So while client does need an address to come from, this address' details aren't as important as the server's.

As such, client's address can be *ephemeral.*

`connect()` automatically binds the client socket to a random address
> The bound-to address belongs to the client!

Now that the client has its address, we have a socket pair, and thus, a connection!

# C++ 

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

```
-- header.h ----------

#ifndef HEADER
#define HEADER

#include <something.h>

namespace something {
    void doSomething();
}

...

#endif
```

Include guard avoids processing the same header twice 
- If we didn't do that, including the file twice would throw redefinition errors 

### Include Brackets or Quotes?

`#include ""` looks for header in project dir first, then system

`#include <>` looks in system only 

### Initializer Lists

Belong to constructors; they initialize class members before constructor body runs
```
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

### `std::osstringstream`

In-memory string builder, call `ss.str()` to convert to printable string.
```
  std::ostringstream ss;

  ss << "\n*** Listening on ADDR: " 
     << inet_ntoa(m_socketAddress.sin_addr)
     << " PORT: " << ntohs(m_socketAddress.sin_port) 
     << " ***\n\n";

  std::cout << ss.str();
```

## Networked C++

### `sockaddr_in`

Defines an address.
```
-- definition -------------------------------------

struct sockaddr_in {
    short           sin_family    // address family
    unsigned short  sin_port      // port, use hton() for network byte order
    struct          in_addr       // the IP, see below...
    char            sin_zero[8]   // padding
}

struct in_addr {
    unsigned long   s_addr        // IPv4 for AF_INET
}

-- usage ------------------------------------------

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

### String Literals w/`const char*`

String literals (any strings we hardcode) are ALWAYS of type `const char*`
> `const` in the param says "this value should NOT be modified inside the function!"

### `recv()`

Receives bytes from the server; 99% of time we want to put this in buffer
```
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

## Threaded C++

We create a thread like this: `std::thread(func, arg1, arg2, arg3, ...)`
> First arg is a function, subsequent args are params we pass to that function

### `.detach()` and `.join()`

When doing `std::thread()` we must also call either `detach()` or `join()` to specify whether the thread will async or block 

```
std::thread(...).detach() 
std::thread(...).join()
```

`detach()` makes the thread asynchronous
- It is now unmanaged; *its own thing*

`join()` makes the thread blocking
- It ensures that our main thread waits for the `.join()`ed thread to finish before moving on   
```
e.g.

void foo();

std::thread t(foo);     // execute foo immediately

...                     // do other stuff

t.join();               // even if the other stuff is done first, 
                        // we can't go past here unless foo also finishes
```

**Bear in mind that a new thread begins running immediately after creating it, not when we call `.detach()` or `.join()`!**

If we never call `.detach()` or `.join()`, C++ will call `std::terminate()`, crashing our program
- C++ doesn't know what the f**k to do with the thread
> Do I wait for it to finish? Do I let it do its own thing?
- So as a response, it blows everything up because we weren't explicit

### Mutex Locks

Threads may share variables

One of these is `stdout`

This means that many threads running in parallel may write to `stdout` at the same time

```
if thread 1 tries to write "foo",
and thread 2 tries to write "bar",

and both happen to try and write at the same time,

we may not get "foobar",
but instead "fboaor" in stdout
```

To prevent this from happening, we can use `std::mutex()`:

```
-- main ---------------

std::mutex m;

void safePrint(const std::string& s){
    m.lock();       // lock all other threads from running the following 
    fmt::print(s);
    m.unlock();     // unlock
}

std::thread(safePrint, "foo").detach();
std::thread(safePrint, "bar").detach();

-----------------------

stdout: "foobar" or "barfoo" depending on which runs first
```

- When a thread acquires a `.lock()` on a `std::mutex`, it blocks other threads from acquiring that mutex 
    - The other threads will have to wait at the same `.lock()` call until the mutex is unlocked by its owning thread
    - As a result, code enclosed by `.lock()` and `.unlock()` is held off from running!

But what if something goes wrong before we get to `.unlock()`? 

We're f**ked!

Unless we use `std::lock_guard`

```
-- main ---------------

std::mutex m;

void safePrint(const std::string& s){
    std::lock_guard<std::mutex> lock(m); // using this instead!
    fmt::print(s);
}

std::thread(safePrint, "foo").detach();
std::thread(safePrint, "bar").detach();

-----------------------

stdout: same effect; "foobar" or "barfoo"
```

- This is a safer version of the lock unlock pattern
- It automatically unlocks mutex when it goes out of scope 
    - If we early return, except, etc. the lock is still released
    - **BUT if something hard-crashes in the mutex owner thread, we're f\*\*ked; it'll never get released**

# CMake

### Get `clangd` Working

Ensure we do `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
- This exports `compile_commands.json`, a compilation DB
- Symbols, include paths can be known to LSP using this

### Adding a Dependency

e.g. `fmt`:
```
# enable fetchcontent module
include(FetchContent)

# declare dependency; specify git repo + version
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)

# download, build, and add to project
FetchContent_MakeAvailable(fmt)

...

# link it to exe!
target_link_libraries(MyProject PRIVATE fmt::fmt)
```
- `PRIVATE` in `target_link` links the dep. to our project, but doesn't expose it to anything that includes our project
- We don't always target link like `fmt::fmt`; read the docs to know how!

## POSIX C++

### `poll()`

Waits for a file descriptor to be ready for a certain operation

# References

- [Medium C++ HTTP server tutorial](https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7)
- [Oracle socket docs](https://docs.oracle.com/javase/tutorial/networking/sockets/definition.html)

---
Questions I have:
- Handle server connection terminate
- Handle async
- Behavior of input loop 
