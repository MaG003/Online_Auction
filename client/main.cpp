#include <iostream>
#include <sstream>
#include <vector>
#include "user.h"
#include "auction_room.h"
#include "client.h"
#include "randomID.h"

std::vector<User> readUsersFromServer(Client &client);

AuctionRoom createAuctionRoom();

std::string generateRandomString();

// Quản lý người dùng
// Hàm để đọc thông tin người dùng từ server
std::vector<User> readUsersFromServer(Client &client)
{
    // Gửi yêu cầu đọc thông tin người dùng từ server
    client.sendData("READ_USERS");

    // Nhận dữ liệu từ server
    std::string receivedData = client.receiveData();

    std::vector<User> users;
    if (!receivedData.empty())
    {
        std::istringstream iss(receivedData);
        std::string username, password, email;
        while (iss >> username >> password >> email)
        {
            users.emplace_back(username, password, email);
        }
    }
    return users;
}

// Hàm để kiểm tra đăng nhập
bool loginUser(Client &client, const std::string &enteredUsername, const std::string &enteredPassword, User &loggedInUser)
{
    // Đọc thông tin người dùng từ server
    std::vector<User> users = readUsersFromServer(client);

    if (!users.empty())
    {
        for (const auto &user : users)
        {
            if (user.isValidLogin(enteredUsername, enteredPassword))
            {
                std::cout << "Login successful!" << std::endl;
                loggedInUser = user; // Lưu thông tin người dùng đã đăng nhập
                return true;
            }
        }

        // Nếu không tìm thấy người dùng hợp lệ
        std::cout << "Login failed. Invalid username or password." << std::endl;
        return false;
    }
    else
    {
        std::cout << "No existing user. Please register." << std::endl;
        return false;
    }
}

// Hàm để ghi thông tin đăng ký người dùng lên server
void registerUserToServer(Client &client, const User &newUser)
{
    // Gửi yêu cầu đăng ký và thông tin người dùng lên server
    client.sendData("REGISTER " + newUser.getUserId() + " " + newUser.getUsername() + " " + newUser.getPassword() + " " + newUser.getEmail());

    // Nhận dữ liệu từ server (thông báo đăng ký thành công hoặc lỗi)
    std::string receivedData = client.receiveData();
    std::cout << "Server response: " << receivedData << std::endl;
}

// Quản lý phòng đấu giá
// Hàm để ghi thông tin phòng đấu giá lên server
void writeAuctionRoomToServer(Client &client, const AuctionRoom &room)
{
    // Gửi yêu cầu ghi thông tin phòng đấu giá lên server
    client.sendData("CREATE_AUCTION_ROOM " + room.getRoomId() + " " + room.getName() + " " + room.getDetails());

    // Nhận dữ liệu từ server (thông báo đăng ký thành công hoặc lỗi)
    std::string receivedData = client.receiveData();
    std::cout << "Server response: " << receivedData << std::endl;
}

// Hàm để đọc thông tin phòng đấu giá từ server
std::vector<AuctionRoom> readAuctionRoomsFromServer(Client &client)
{
    // Gửi yêu cầu đọc thông tin phòng đấu giá từ server
    client.sendData("READ_AUCTION_ROOMS");

    // Nhận dữ liệu từ server
    std::string receivedData = client.receiveData();

    std::vector<AuctionRoom> auctionRooms;
    if (!receivedData.empty())
    {
        std::istringstream iss(receivedData);
        std::string roomName, details;

        while (iss >> roomName >> details)
        {
            auctionRooms.emplace_back(roomName, details);
        }
    }

    return auctionRooms;
}

int main()
{
    Client client;
    std::string serverIP = "127.0.0.1"; // Thay bằng địa chỉ IP thực tế của server
    int serverPort = 12345;             // Thay bằng cổng mà server đang lắng nghe

    if (!client.connectToServer(serverIP, serverPort))
    {
        std::cerr << "Error connecting to the server" << std::endl;
        return EXIT_FAILURE;
    }

    User loggedInUser;
    AuctionRoom newRoom;
    std::vector<AuctionRoom> auctionRooms;

    while (true)
    {
        std::cout << "Welcome to the Auction System!" << std::endl;

        // Hiển thị menu login
        std::cout << "1. Login\n2. Signup\n3. Exit\n";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Yêu cầu nhập thông tin đăng nhập
            std::string enteredUsername, enteredPassword;
            std::cout << "Enter username: ";
            std::cin >> enteredUsername;

            std::cout << "Enter password: ";
            std::cin >> enteredPassword;

            // Thực hiện đăng nhập
            if (loginUser(client, enteredUsername, enteredPassword, loggedInUser))
            {
                // Nếu đăng nhập thành công, thực hiện các thao tác sau đăng nhập
                while (true)
                {
                    // Hiển thị menu chính sau khi đăng nhập
                    std::cout << "Menu" << std::endl;
                    std::cout << "4. Create Auction Room\n5. View Auction Rooms\n6. Logout\n";
                    std::cin >> choice;
                    std::string newRoomId, newRoomname, newDetails;

                    switch (choice)
                    {
                    case 4:
                        // Thêm chức năng tạo mới phòng đấu giá sau khi đăng nhập thành công

                        newRoomId = generateRandomString();

                        std::cout << "Enter new roomname: ";
                        std::cin >> newRoomname;

                        std::cout << "Enter new Roomdetails: ";
                        std::cin >> newDetails;

                        newRoom = AuctionRoom::createAuctionRoom(newRoomId, newRoomname, newDetails);

                        writeAuctionRoomToServer(client, newRoom);

                        std::cout << "Auction Room created: " << newRoom.getName() << std::endl;
                        break;

                    case 5:
                        // Thêm chức năng xem danh sách phòng đấu giá
                        std::cout << "Auction Rooms:" << std::endl;

                        // Gọi hàm để đọc danh sách phòng đấu giá từ server
                        auctionRooms = readAuctionRoomsFromServer(client);

                        if (auctionRooms.empty())
                        {
                            std::cout << "No auction rooms available." << std::endl;
                        }
                        else
                        {
                            // Hiển thị thông tin về các phòng đấu giá
                            for (const auto &room : auctionRooms)
                            {
                                std::cout << "Name: " << room.getName() << std::endl;
                                std::cout << "Details: " << room.getDetails() << std::endl;
                                std::cout << "---------------------------" << std::endl;
                            }

                            while (true)
                            {
                                std::cout << "Auction choice" << std::endl;
                                std::cout << "7. Join in auction rooms\n8. Back\n";
                                std::cin >> choice;
                                switch (choice)
                                {
                                case 7:
                                    std::cout << "Join in auction rooms" << std::endl;
                                    break;
                                case 8:
                                    std::cout << "Back" << std::endl;
                                    break;
                                default:
                                    std::cout << "Invalid choice. Please try again." << std::endl;
                                    break;
                                }
                                if (choice == 8)
                                {
                                    break;
                                }
                            }
                        }
                        break;
                    case 6:
                        std::cout << "Logging out. Goodbye!" << std::endl;
                        break;
                    default:
                        std::cout << "Invalid choice. Please try again." << std::endl;
                        break;
                    }
                    if (choice == 6)
                    {
                        break;
                    }
                }
            }
            break;
        }
        case 2:
        {
            // Đăng ký người dùng mới và gửi thông tin lên server
            std::string newUserId, newUsername, newPassword, newEmail;

            newUserId = generateRandomString();

            std::cout << "Enter new username: ";
            std::cin >> newUsername;

            std::cout << "Enter new password: ";
            std::cin >> newPassword;

            std::cout << "Enter email: ";
            std::cin >> newEmail;

            User newUser = User::signupUser(newUserId, newUsername, newPassword, newEmail);

            // Gọi hàm để đăng ký người dùng mới
            registerUserToServer(client, newUser);
            break;
        }
        case 3:
            std::cout << "Exiting the Auction System. Goodbye!" << std::endl;
            return 0;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
    }

    return 0;
}