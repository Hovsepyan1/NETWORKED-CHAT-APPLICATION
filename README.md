Networked Chat Application

This project is a simple networked chat application written in C. It allows multiple clients to connect to a server and exchange messages in real-time.
Features

    Multi-client support: Allows multiple clients to connect and chat simultaneously.
    Real-time messaging: Instant message delivery between clients.
    Server-client architecture: Central server to handle message routing and client management.
    User-friendly interface: Simple command-line interface for ease of use.

Requirements

    GCC (GNU Compiler Collection)
    POSIX compliant operating system (e.g., Linux, macOS)

Installation

    Clone the repository:

    bash

git clone https://github.com/Hovsepyan1/NETWORKED-CHAT-APPLICATION.git
cd NETWORKED-CHAT-APPLICATION

Compile the program:

bash

    gcc main.c functions.c -o chat_server

Usage
Starting the Server

    Navigate to the project directory.

    Run the server executable:

    bash

    ./chat_server <port>

    Replace <port> with the desired port number.

Connecting a Client

    Navigate to the project directory.

    Run the client executable:

    bash

    ./client <server_ip> <port>

    Replace <server_ip> with the IP address of the server and <port> with the port number the server is listening on.

Example

    Start the server:

    bash

./chat_server 12345

Connect clients:

bash

    ./client 127.0.0.1 12345

    Each client can now send messages which will be broadcast to all connected clients.

File Structure

    main.c: The main server code.
    functions.c: Supporting functions for the server.
    client.c: The client-side code.
    README.md: This README file.

Contributing

Contributions are welcome! Please fork this repository and submit pull requests.
License

This project is licensed under the MIT License - see the LICENSE file for details.
Acknowledgments

    Inspired by various network programming examples 
