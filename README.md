# Socket Program Requirements

## Server

### Initialise client/server system settings 
    [IP, Port] via: Command line args (server.h) || (User:/socket$ IP Port)  - or
                    Keyboard input (scanf) || (fgets)
                    Config file (IP-Port.config)
### Request data 
    Send 'R' or 'r' to client every three seconds

### Receive ACK 
    Calculates round trip time (RTT)
    Displays RTT on its monitor
    Time resolution must be micro or submicro seconds

### Terminate - Three Way Handshake:
    Without interruption...
    Recieve 'E' or 'e' from keyboard input
    Send 'E' or 'e' to client
    Recieve 'O' or 'o' from client
    Send 'O' or 'o' to client
    Exit 
    
## Client 
### Respond to data requests - ACK:
    "ACK at hh:mm:ss:dd" - hours, minutes, seconds, milliseconds
    Random number(0,100)
    Display some useful information on its monitor.

###  Terminate - Three Way Handshake:
    Without interruption...
    Recieve 'E' or 'e' from server
    Send 'O' or 'o' to server
    Recieve 'O' or 'o' from server
    Exit
