---
sidebar_position: 0
---

# Sockets & Connectivity 

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
Following the <IP>:<PORT> format,

If client's connection socket is:
    LOCAL = B:2
    REMOTE = A:1

Then server's talkback socket is:
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
