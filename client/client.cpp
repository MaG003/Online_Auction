// client.cpp

#include "client.h"

Client::Client()
{
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock" << std::endl;
    }

    // Tạo socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
    }
}

Client::~Client()
{
    // Đóng socket và giải phóng Winsock khi kết thúc
    closesocket(clientSocket);
    WSACleanup();
}

bool Client::connectToServer(const std::string &serverIP, int port)
{
    // Cài đặt địa chỉ và cổng cho server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    serverAddr.sin_port = htons(port);

    // Kết nối đến server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error connecting to the server" << std::endl;
        return false;
    }

    std::cout << "Connected to the server" << std::endl;
    return true;
}

bool Client::sendData(const std::string &data) const
{
    int bytesSent = send(clientSocket, data.c_str(), data.length(), 0);
    if (bytesSent == SOCKET_ERROR || bytesSent != static_cast<int>(data.length()))
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

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        std::cerr << "Error receiving data. Error code: " << errorCode << std::endl;
        return std::string(); // Trả về một chuỗi rỗng để chỉ ra lỗi
    }
    else if (bytesReceived == 0)
    {
        std::cerr << "Connection closed by the server" << std::endl;
        return std::string(); // Trả về một chuỗi rỗng để chỉ ra kết nối đã đóng
    }

    return std::string(buffer, bytesReceived);
}
