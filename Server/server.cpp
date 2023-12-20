#include <iostream>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <algorithm>
#include <vector>
#include "user.h"
#include "auction_room.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sql.h>
#include <sqlext.h>

std::vector<User> readUsersFromSQLServer();
std::vector<AuctionRoom> readAuctionRoomsFromSQLServer();

void writeAuctionRoomsToSQLServer(const std::vector<AuctionRoom> &rooms);
void writeUsersToSQLServer(const std::vector<User> &users);

std::vector<std::string> requestClient(int clientSocket, const std::vector<User> &users)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received registration request from client: " << buffer << std::endl;

    std::string clientData(buffer);

    std::vector<std::string> requestDataParts;
    size_t pos = 0;
    while ((pos = clientData.find(' ')) != std::string::npos)
    {
        std::string part = clientData.substr(0, pos);
        requestDataParts.push_back(part);
        clientData.erase(0, pos + 1);
    }
    if (!clientData.empty())
    {
        requestDataParts.push_back(clientData);
    }
    for (const auto &part : requestDataParts)
    {
        std::cout << "Part: " << part << std::endl;
    }
    return requestDataParts;
}

// Thêm hàm xử lý yêu cầu đăng ký người dùng
void handleRegistrationRequest(int clientSocket, std::vector<User> &users, std::vector<std::string> requestDataParts)
{
    if (requestDataParts.size() == 4)
    {
        std::string newUsername = requestDataParts[1];
        std::string newPassword = requestDataParts[2];
        std::string newEmail = requestDataParts[3];

        // Kiểm tra xem người dùng có tồn tại hay chưa
        bool userExists = false;
        for (const auto &user : users)
        {
            if (user.getUsername() == newUsername)
            {
                userExists = true;
                break;
            }
        }

        if (!userExists)
        {
            // Tạo người dùng mới và thêm vào danh sách
            User newUser = User::signupUser(newUsername, newPassword, newEmail);
            users.push_back(newUser);
            
            // Ghi danh sách người dùng mới vào tệp
            writeUsersToSQLServer(users);

            // Gửi thông báo thành công về cho client
            const char *successMessage = "User registered successfully!";
            send(clientSocket, successMessage, strlen(successMessage), 0);
            std::cout << "Message sent to client" << std::endl;
        }
        else
        {
            // Gửi thông báo lỗi về cho client nếu người dùng đã tồn tại
            const char *errorMessage = "Username already exists. Please choose a different username.";
            send(clientSocket, errorMessage, strlen(errorMessage), 0);
            std::cout << "Error message sent to client" << std::endl;
        }
    }
    else
    {
        // Gửi thông báo lỗi về cho client nếu định dạng dữ liệu không hợp lệ
        const char *errorMessage = "Invalid data format for user registration";
        send(clientSocket, errorMessage, strlen(errorMessage), 0);
        std::cout << "Error message sent to client" << std::endl;
    }
}

// Hàm xử lý yêu cầu đọc thông tin người dùng và trả về client
void handleReadUserRequest(int clientSocket, const std::vector<User> &users)
{
    // Tạo một chuỗi để lưu thông tin người dùng
    std::string userData;

    // Kiểm tra xem danh sách người dùng có rỗng hay không
    if (users.empty())
    {
        // Gửi thông báo về cho client nếu danh sách người dùng rỗng
        const char *message = "No users to display";
        send(clientSocket, message, strlen(message), 0);
        std::cout << "Message sent to client" << std::endl;
    }
    else
    {
        // Lặp qua danh sách người dùng và thêm thông tin vào chuỗi
        for (const auto &user : users)
        {
            // Thêm thông tin người dùng vào chuỗi (có thể định dạng chuỗi theo ý muốn)
            userData += user.getUsername() + " " + user.getPassword() + " " + user.getEmail() + "\n";
        }

        // Gửi thông tin người dùng về cho client
        send(clientSocket, userData.c_str(), userData.length(), 0);
        std::cout << "User data sent to client" << std::endl;
    }
}

// Hàm để gửi thông báo về client
void sendMessageToClient(int clientSocket, const char *message)
{
    send(clientSocket, message, strlen(message), 0);
    std::cout << "Message sent to client" << std::endl;
}

// Hàm xử lý yêu cầu tạo phòng đấu giá từ client
void handleCreateAuctionRoomRequest(int clientSocket, std::vector<AuctionRoom> &rooms, const std::vector<std::string> &requestDataParts)
{
    if (requestDataParts.size() == 3)
    {
        std::string roomName = requestDataParts[1];
        std::string roomDetails = requestDataParts[2];

        // Kiểm tra xem phòng đấu giá có tồn tại hay không
        auto existingRoom = std::find_if(rooms.begin(), rooms.end(), [&](const AuctionRoom &room)
                                         { return room.getName() == roomName; });

        if (existingRoom == rooms.end())
        {
            // Tạo phòng đấu giá mới và thêm vào danh sách
            AuctionRoom newRoom = AuctionRoom::createAuctionRoom(roomName, roomDetails);
            rooms.push_back(newRoom);

            // Ghi danh sách phòng đấu giá mới vào tệp
            writeAuctionRoomsToSQLServer(rooms);

            // Gửi thông báo thành công về cho client
            sendMessageToClient(clientSocket, "Auction Room created successfully!");
        }
        else
        {
            // Gửi thông báo lỗi về cho client nếu phòng đấu giá đã tồn tại
            sendMessageToClient(clientSocket, "Auction Room already exists. Please choose a different name.");
        }
    }
    else
    {
        // Gửi thông báo lỗi về cho client nếu định dạng dữ liệu không hợp lệ
        sendMessageToClient(clientSocket, "Invalid data format for Auction Room creation");
    }
}

// Hàm xử lý yêu cầu đọc thông tin phòng đấu giá và trả về client
void handleReadAuctionRoomsRequest(int clientSocket, const std::vector<AuctionRoom> &rooms)
{
    // Tạo một chuỗi để lưu thông tin phòng đấu giá
    std::string roomData;

    // Kiểm tra xem danh sách phòng đấu giá có rỗng hay không
    if (rooms.empty())
    {
        // Gửi thông báo về cho client nếu danh sách phòng đấu giá rỗng
        sendMessageToClient(clientSocket, "No auction rooms available");
    }
    else
    {
        // Lặp qua danh sách phòng đấu giá và thêm thông tin vào chuỗi
        for (const auto &room : rooms)
        {
            // Thêm thông tin phòng đấu giá vào chuỗi (có thể định dạng chuỗi theo ý muốn)
            roomData += room.getName() + " " + room.getDetails() + "\n";
        }

        // Gửi thông tin phòng đấu giá về cho client
        send(clientSocket, roomData.c_str(), roomData.length(), 0);
        std::cout << "Auction room data sent to client" << std::endl;
    }
}

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind failed with error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        std::cerr << "Listen failed with error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server is listening for connections..." << std::endl;

    std::vector<User> users = readUsersFromSQLServer();
    std::vector<AuctionRoom> rooms = readAuctionRoomsFromSQLServer();

    while (true)
    {
        int clientSocket;
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == -1)
        {
            std::cerr << "Accept failed with error: " << strerror(errno) << std::endl;
            close(serverSocket);
            return -1;
        }

        std::cout << "Client connected." << std::endl;

        int check = 0;
        while (check == 0)
        {
            std::vector<std::string> requestDataParts = requestClient(clientSocket, users);
            std::string requestType = requestDataParts[0];

            if (requestType == "REGISTER")
            {
                handleRegistrationRequest(clientSocket, users, requestDataParts);
            }
            else if (requestType == "READ_USERS")
            {
                handleReadUserRequest(clientSocket, users);
            }
            else if (requestType == "CREATE_AUCTION_ROOM")
            {
                handleCreateAuctionRoomRequest(clientSocket, rooms, requestDataParts);
            }
            else if (requestType == "READ_AUCTION_ROOMS")
            {
                handleReadAuctionRoomsRequest(clientSocket, rooms);
            }
        }

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}
