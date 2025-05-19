Goal: Many clients can enter the same chat and send messages
Server handles simultaneous connections:
    - Message stack
        - New messages appended to it
    - Client connections
        - Many threads for diff. clients
    - Since we're keeping messages in stack, we only need to inform clients on updates to it
    - Ergo, if a client posts a new message, we would:
        - Post it to stack
        - Tell all connected clients there's something new in the stack
        - They will all then re-fetch the stack
            - Ideally, though, we would only send them the changes! Stretch goal?
Client messaging:
    - Join on binary launch
    - Receive and display all messages sent before join 
    - Receive and display any newly sent messages
    - Can leave if enter command OR quit via ^C 
    - Handle server/connection drop + input in non-blocking way
