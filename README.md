# socket program requirement


1) When the Server starts to run, it initializes the settings of the client-server system, e.g., IP address and port number, through (i) command window arguments, e.g., header file, arguments to main(), (ii) keyboard input, or (iii) input from a configuration file, which is a pure text file.


3) Every three seconds, the Server sends the Client a command, e.g., through a single letter ‘R’ or ‘r’ (request) to request data from the Client.


3) After receiving the command from the Server, the Client responds to the Server with an ACK message consisting of:


a) A string “ACK at” followed by a timestamp. The timestamp has the format hh:mm:ss:ddd, meaning two digits (hh) for hours, two digits (mm) for minutes, two digits (ss) for seconds, and three digits (ddd) for milliseconds (For example, 18:03:54:793, in which 793 means 793 ms); and

b) A random integer in [0, 1000] with a uniform distribution. The Client may also display some useful information on its monitor. You may use a random generator function (available from C library) to generate such random numbers.

4) The Server gets the ACK message from the Client, and calculates the round trip time (RTT) from its communications with the Client, and displays the result on its monitor. If you use the same computer to test the Server and Client programs, or if the Server and Client are very close, the RTT may be less than 1 ms. In this case, you will need a higher time resolution, e.g., in microseconds or even sub-microseconds.


5) Without interruptions to the client-server communication, the Server reads keyboard input of various commands or instructions. An obvious command is to terminate the Server program, e.g., using a single letter ‘E’ or ‘e’ (exit) for termination. A three-way handshaking is required for terminating the client-server system:


i) When the Sever is to be terminated, it should notify the Client of the Server’s intention of termination so that the Client also terminates properly.

ii) When the Client receives the single letter ‘E’ or ‘e’ (exit) from the Server, it sends back to the Server a command using a single letter ‘O’ or ‘o’ (OK).
iii) Then, the Server informs the Client of the receipt of the ‘O’ or ‘o’ command.

After this three-way handshaking, both the Server and Client are safe to terminate.
