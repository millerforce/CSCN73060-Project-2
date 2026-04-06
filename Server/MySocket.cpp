
#include <cstring>

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")

    #define CLOSE(s) closesocket(s)
    #define IS_SOCKET_ERROR(s) (s == INVALID_SOCKET)
    #define GET_LAST_SOCKET_ERROR() WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define CLOSE(s) close(s)
    #define IS_SOCKET_ERROR(s) (s < 0)
    #define GET_LAST_SOCKET_ERROR() errno
#endif

#include <algorithm>
#include "MySocket.h"

#include "InvalidConnectionType.h"
#include "InvalidConnectionState.h"

//Configure socket as a TCP server
void MySocket::handleTCPServerInit() {

    //Create socket
    this->WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (IS_SOCKET_ERROR(WelcomeSocket)) {
        throw InvalidConnectionStateException("Error when attempting to initialize WelcomeSocket");
    }

    //Initialize server to listen for any address
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(Port);

    //Bind socket
    if (bind(WelcomeSocket, reinterpret_cast<sockaddr *>(&SvrAddr), sizeof(SvrAddr))) {
        CLOSE(WelcomeSocket);
        throw InvalidConnectionStateException("Error occurred when binding WelcomeSocket");
    }

    //Listen on the socket
    if (listen(WelcomeSocket, 1)) {
        CLOSE(WelcomeSocket);
        throw InvalidConnectionStateException("Error occurred when listening on WelcomeSocket");
    }

    sockaddr_in clientAddress = {};
    socklen_t clientAddressLength = sizeof(clientAddress);

    //Accept connection with client
    ConnectionSocket = accept(WelcomeSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
    if (IS_SOCKET_ERROR(ConnectionSocket)) {
        throw InvalidConnectionStateException("Failed to accept client connection");
    }
    bTCPConnect = true;
}

//configure socket as TCP client
void MySocket::handleTCPClientInit() {
    //Create the socket
    ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (IS_SOCKET_ERROR(ConnectionSocket)) {
        throw InvalidConnectionStateException("Failed to create TCP client socket.");
    }

    //Prepares socket for connection
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);
}

//configure the socket as a UDP server
void MySocket::handleUDPServerInit() {
    //Create the socket
    this->ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (IS_SOCKET_ERROR(ConnectionSocket)) {
        throw InvalidConnectionStateException("Error when attempting to initialize ConnectionSocket");
    }

    //Configure to accept requests from any address
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(Port);

    //Bind the socket
    if (bind(ConnectionSocket, reinterpret_cast<sockaddr *>(&SvrAddr), sizeof(SvrAddr))) {
        CLOSE(ConnectionSocket);
        throw InvalidConnectionStateException("Error occurred when binding ConnectionSocket");
    }
}

//configure the socket as UDP client
void MySocket::handleUDPClientInit() {
    //Create the socket
    this->ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (IS_SOCKET_ERROR(ConnectionSocket)) {
        throw InvalidConnectionStateException("Error when attempting to initialize ConnectionSocket");
    }

    //Configure the socket to timeout if it doesn't receive a response in 5 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(ConnectionSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        CLOSE(ConnectionSocket);
        throw InvalidConnectionStateException("Error setting socket options");
    }

    //Set servers address
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
    SvrAddr.sin_port = htons(Port);
}

//Initialise the socket
MySocket::MySocket(const SocketType socketType, const std::string& iPAddr, int port, const ConnectionType connectionType, const unsigned int size)
    : mySocket(socketType),
    IPAddr(iPAddr),
    Port(port),
    connectionType(connectionType),
    MaxSize((size < BASELENGTH) ? DEFAULT_SIZE : size) // This needs to be adjusted
{
    Buffer = new char[MaxSize];

    if (socketType == SERVER) {
        if (connectionType == TCP) {
            handleTCPServerInit();
        } else if (connectionType == UDP) {
            handleUDPServerInit();
        }
    } else {
        if (connectionType == TCP) {
            handleTCPClientInit();
        } else if (connectionType == UDP) {
            handleUDPClientInit();
        }
    }
}

MySocket::~MySocket() {
    if (Buffer) {
        delete[] Buffer;
    }
    if (WelcomeSocket) {
        CLOSE(WelcomeSocket);
    }
    if (ConnectionSocket) {
        CLOSE(ConnectionSocket);
    }
}

//If socket is TCP client this triggers it to connect to the server
void MySocket::ConnectTCP() {
    //Confirm correct type
    if (connectionType != TCP) {
        throw InvalidConnectionTypeException("Socket type is not set to TCP. Unable to create a connection.");
    }

    //Create the socket
    ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (IS_SOCKET_ERROR(ConnectionSocket)) {
        bTCPConnect = false;
        throw InvalidConnectionStateException("Error when creating ConnectionSocket");
    }

    //Prepare to connect to specified server address
    SvrAddr = {};
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);

    //Attempt to connect to the server
    if (connect(ConnectionSocket, reinterpret_cast<sockaddr *>(&SvrAddr), sizeof(SvrAddr))) {
        CLOSE(ConnectionSocket);
        throw InvalidConnectionStateException("Unable to connect to server");
    }

    bTCPConnect = true;
}

//Disconnect TCP from server
void MySocket::DisconnectTCP() {
    if (connectionType != TCP) {
        throw InvalidConnectionTypeException("Socket type is not set to TCP. Unable to disconnect.");
    }

    if (!bTCPConnect) {
        throw InvalidConnectionStateException("Cannot disconnect from a socket that is not connected.");
    }

    CLOSE(ConnectionSocket);
    bTCPConnect = false;
}

//Send a TCP packet
int MySocket::sendTCP(const char* data, int size) {
    return send(ConnectionSocket, data, size , 0);
}

//Send a UDP packet
int MySocket::sendUDP(const char* data, int size) {
    return sendto(
        ConnectionSocket,
        data,
        size,
        0,
        (struct sockaddr*)&SvrAddr,
        sizeof(SvrAddr)
    );
}

//Generic data send that directs outgoing response depending on configuration
int MySocket::SendData(const char *data, int size) {
    if (connectionType == TCP) {
        if (!bTCPConnect) {
            throw InvalidConnectionStateException("Cannot send data over TCP if not connected.");
        }
        return sendTCP(data, size);
    }
    else if (connectionType == UDP)
        return sendUDP(data, size);
    return -1;
}

//Receive a TCP packet
int MySocket::recvTCP(char* data) {
    return recv(ConnectionSocket, Buffer, MaxSize, 0);
}

//Recieve a UDP packet
int MySocket::recvUDP(char* data) {
    socklen_t addrLen = sizeof(SvrAddr);
    return recvfrom(
        ConnectionSocket,
        Buffer,
        MaxSize,
        0,
        (struct sockaddr*)&SvrAddr,
        &addrLen
    );
}

//Retrieve data depending on configuration
int MySocket::GetData(char *data) {
    int recvBytes = -1;
    if (connectionType == TCP) {
        if (!bTCPConnect) {
            throw InvalidConnectionStateException("Cannot receive data over TCP if not connected.");
        }
        recvBytes = recvTCP(data);
    }
    else if (connectionType == UDP) {
        recvBytes = recvUDP(data);
    }

    if (recvBytes < 0) {
        return recvBytes;
    }

    memcpy(data, Buffer, std::min(recvBytes, MaxSize));
    return recvBytes;
}

std::string MySocket::GetIPAddr() {
    return this->IPAddr;
}

void MySocket::SetIPAddr(const std::string &iPAddr) {
    this->IPAddr = iPAddr;
}

void MySocket::SetPort(const int port) {
    this->Port = port;
}

int MySocket::GetPort() {
    return this->Port;
}

SocketType MySocket::GetType() {
    return this->mySocket;
}

ConnectionType MySocket::GetConnectionType(){
    return this->connectionType;
}

int MySocket::GetBufferSize() {
    if (Buffer) {
        return sizeof(Buffer);
    }
    return -1;
}

bool MySocket::isConnected() {
    return this->bTCPConnect;
}

void MySocket::SetType(SocketType type) {
    this->mySocket = type;
}

// We set socket timeouts so that if we don't receive a response from the robot in a certain amount of time, we can assume the robot is not going to respond.
bool MySocket::SetSocketTimeout(int timeoutSeconds) {
    if (ConnectionSocket < 0) {
        return false;
    }
    if (timeoutSeconds < 0) {
        return false;
    }
    struct timeval timeout{};
    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 0;
    if (setsockopt(ConnectionSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        return false;
    }
    return true;
}
