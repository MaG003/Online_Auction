#include <iostream>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <algorithm>
#include <winsock2.h>
#include <vector>
#include "user.h"
#include "auction_room.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<User> readUsersFromTxt(const std::string &filename);
std::vector<AuctionRoom> readAuctionRoomsFromTxt(const std::string &filename);

void writeUsersToTxt(const std::string &filename, const std::vector<User> &users);
void writeAuctionRoomsToTxt(const std::string &filename, const std::vector<AuctionRoom> &rooms);

std::vector<std::string> requestClient(SOCKET clientSocket, const std::vector<User> &users)
{
    // Nhận dữ liệu từ client
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received registration request from client: " << buffer << std::endl;

    // Xử lý dữ liệu từ client (ví dụ: tạo thông tin người dùng)
    std::string clientData(buffer);

    // Phân tách yêu cầu từ client thành các thành phần (ví dụ: "REGISTER username password email")
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
void handleRegistrationRequest(SOCKET clientSocket, std::vector<User> &users, std::vector<std::string> requestDataParts)
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
            writeUsersToTxt("user_data.txt", users);

            // Gửi thông báo thành công về cho client
            int bytesSent = send(clientSocket, "User registered successfully!", strlen("User registered successfully!"), 0);
            if (bytesSent == SOCKET_ERROR || bytesSent != strlen("User registered successfully!"))
            {
                int errorCode = WSAGetLastError();
                std::cerr << "Error sending message to client. Error code: " << errorCode << std::endl;

                // Xem thêm thông tin về mã lỗi
                switch (errorCode)
                {
                case WSAECONNRESET:
                    std::cerr << "Connection reset by peer" << std::endl;
                    break;
                    // Thêm các trường hợp xử lý khác tùy thuộc vào mã lỗi cụ thể
                }
            }
            else
            {
                std::cout << "Message sent to client" << std::endl;
            }

            if (send(clientSocket, "User registered successfully!", strlen("User registered successfully!"), 0) == SOCKET_ERROR)
            {
                std::cerr << "Error sending message to client. Connection closed." << std::endl;
            }
        }
        else
        {
            // Gửi thông báo lỗi về cho client nếu người dùng đã tồn tại
            int bytesSent = send(clientSocket, "Username already exists. Please choose a different username.", strlen("Username already exists. Please choose a different username."), 0);
            if (bytesSent == SOCKET_ERROR || bytesSent != strlen("Username already exists. Please choose a different username."))
            {
                std::cerr << "Error sending error message to client, Error code: " << WSAGetLastError() << std::endl;
            }
            else
            {
                std::cout << "Error message sent to client" << std::endl;
            }
        }
    }
    else
    {
        // Gửi thông báo lỗi về cho client nếu định dạng dữ liệu không hợp lệ
        int bytesSent = send(clientSocket, "Invalid data format for user registration", strlen("Invalid data format for user registration"), 0);
        if (bytesSent == SOCKET_ERROR || bytesSent != strlen("Invalid data format for user registration"))
        {
            std::cerr << "Error sending error message to client, Error code: " << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "Error message sent to client" << std::endl;
        }
    }
}

// Hàm xử lý yêu cầu đọc thông tin người dùng và trả về client
void handleReadUserRequest(SOCKET clientSocket, const std::vector<User> &users)
{
    // Tạo một chuỗi để lưu thông tin người dùng
    std::string userData;

    // Kiểm tra xem danh sách người dùng có rỗng hay không
    if (users.empty())
    {
        // Gửi thông báo về cho client nếu danh sách người dùng rỗng
        int bytesSent = send(clientSocket, "No users to display", strlen("No users to display"), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "Error sending message to client. Error code: " << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "Message sent to client" << std::endl;
        }
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
        int bytesSent = send(clientSocket, userData.c_str(), userData.length(), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "Error sending user data to client. Error code: " << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "User data sent to client" << std::endl;
        }
    }
}

// Hàm để gửi thông báo về client
void sendMessageToClient(SOCKET clientSocket, const char *message)
{
    int bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == SOCKET_ERROR || bytesSent != strlen(message))
    {
        int errorCode = WSAGetLastError();
        std::cerr << "Error sending message to client. Error code: " << errorCode << std::endl;
        // Xử lý các trường hợp mã lỗi khác nếu cần
    }
    else
    {
        std::cout << "Message sent to client" << std::endl;
    }
}

// Hàm xử lý yêu cầu tạo phòng đấu giá từ client
void handleCreateAuctionRoomRequest(SOCKET clientSocket, std::vector<AuctionRoom> &rooms, const std::vector<std::string> &requestDataParts)
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
            writeAuctionRoomsToTxt("auction_rooms.txt", rooms);

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
void handleReadAuctionRoomsRequest(SOCKET clientSocket, const std::vector<AuctionRoom> &rooms)
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
        int bytesSent = send(clientSocket, roomData.c_str(), roomData.length(), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "Error sending auction room data to client. Error code: " << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "Auction room data sent to client" << std::endl;
        }
    }
}

int main()
{
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return -1;
    }

    // Tạo socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket" << std::endl;
        WSACleanup();
        return -1;
    }

    // Cài đặt địa chỉ và cổng cho server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Lắng nghe kết nối đến socket
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is listening for connections..." << std::endl;

    // Đọc thông tin người dùng và phòng đấu giá từ tệp
    std::vector<User> users = readUsersFromTxt("user_data.txt");
    std::vector<AuctionRoom> rooms = readAuctionRoomsFromTxt("auction_rooms.txt");

    while (true)
    {
        // Chấp nhận kết nối từ client
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        std::cout << "Client connected." << std::endl;

        // Xử lý lựa chọn của client
        int check = 0;
        while (check == 0)
        {
            std::vector<std::string> requestDataParts = requestClient(clientSocket, users);
            std::string requestType = requestDataParts[0];
            //  đăng ký
            if (requestType == "REGISTER")
            {
                handleRegistrationRequest(clientSocket, users, requestDataParts);
                // Không đóng kết nối ở đây để tiếp tục nhận yêu cầu từ client
            }
            // Đăng nhập
            else if (requestType == "READ_USERS")
            {
                handleReadUserRequest(clientSocket, users);
                // Không đóng kết nối ở đây để tiếp tục nhận yêu cầu từ client
            }
            // Tạo phòng đấu giá
            else if (requestType == "CREATE_AUCTION_ROOM")
            {
                handleCreateAuctionRoomRequest(clientSocket, rooms, requestDataParts);
                // Không đóng kết nối ở đây để tiếp tục nhận yêu cầu từ client
            }
            else if (requestType == "READ_AUCTION_ROOMS")
            {
                handleReadAuctionRoomsRequest(clientSocket, rooms);
                // Không đóng kết nối ở đây để tiếp tục nhận yêu cầu từ client
            }
        }

        // Đóng kết nối
        closesocket(clientSocket);
    }

    // Đóng kết nối socket của server
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
