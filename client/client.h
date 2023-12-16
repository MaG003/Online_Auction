// Trong client.h

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class Client
{
public:
    Client();
    ~Client();

    bool connectToServer(const std::string &serverIP, int port);
    bool sendData(const std::string &data) const;
    std::string receiveData() const;

private:
    int clientSocket;
};

#endif // CLIENT_H
