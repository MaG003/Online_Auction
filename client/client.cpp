// client.cpp

#include "client.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client()
{
    // Tạo socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
    }
}

Client::~Client()
{
    // Đóng socket khi kết thúc
    close(clientSocket);
}

bool Client::connectToServer(const std::string &serverIP, int port)
{
    // Cài đặt địa chỉ và cổng cho server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    serverAddr.sin_port = htons(port);

    // Kết nối đến server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Error connecting to the server" << std::endl;
        return false;
    }

    std::cout << "Connected to the server" << std::endl;
    return true;
}

bool Client::sendData(const std::string &data) const
{
    ssize_t bytesSent = send(clientSocket, data.c_str(), data.length(), 0);
    if (bytesSent == -1 || static_cast<size_t>(bytesSent) != data.length())
    {
        std::cerr << "Error sending data" << std::endl;
        return false;
    }
    return true;
}

std::string Client::receiveData() const
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == -1)
    {
        perror("Error receiving data");
        return std::string(); // Trả về một chuỗi rỗng để chỉ ra lỗi
    }
    else if (bytesReceived == 0)
    {
        std::cerr << "Connection closed by the server" << std::endl;
        return std::string(); // Trả về một chuỗi rỗng để chỉ ra kết nối đã đóng
    }

    return std::string(buffer, bytesReceived);
}
