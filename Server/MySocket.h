
#ifndef ROBOTSOCKET_H
#define ROBOTSOCKET_H

#include <cstdint>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET socket_t;
#else
#include <netinet/in.h>
    typedef int socket_t;
#endif

enum SocketType {
    CLIENT,
    SERVER
};

enum ConnectionType {
    UDP,
    TCP
};

class MySocket {
private:
    char* Buffer = nullptr;

    socket_t WelcomeSocket = -1;
    socket_t ConnectionSocket = -1;
    sockaddr_in SvrAddr{};
    SocketType mySocket;
    std::string IPAddr;
    int Port =  -1;
    ConnectionType connectionType;
    bool bTCPConnect = false;
    int MaxSize{};

    void handleTCPServerInit();//Initialize as TCP server
    void handleTCPClientInit();//Initialize as TCP client

    void handleUDPServerInit();//Initialize as UDP server
    void handleUDPClientInit();//Initialize as UDP client

    int sendTCP(const char* data, int size);
    int sendUDP(const char* data, int size);

    int recvTCP(char* data);
    int recvUDP(char* data);

    void SetIPAddr(const std::string &);
    void SetPort(int port);
    void SetType(SocketType);

public:
    MySocket(SocketType socketType, const std::string& iPAddr, int port, ConnectionType connectionType, unsigned int bufferSize);
    ~MySocket();

    bool SetSocketTimeout(int timeoutSeconds);

    void ConnectTCP();
    void DisconnectTCP();
    int SendData(const char* data, int size);//Send data regardless of config
    int GetData(char* data);//Get data regardless of config
    std::string GetIPAddr();
    int GetPort();
    SocketType GetType();
    ConnectionType GetConnectionType();
    int GetBufferSize();
    bool isConnected();//Confirm socket is connected to a TCP server
};



#endif //ROBOTSOCKET_H
