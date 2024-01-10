#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "signin.h"
#include "connect_sql.h"
#include "auction_room.h"
#include <sstream>
#include <iomanip>

using namespace std;

int main()
{
    AuctionRoom room;

    // Khởi tạo socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
    {
        cerr << "Error creating socket." << endl;
        return -1;
    }

    // Cấu hình địa chỉ và cổng cho server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888); // Chọn một cổng khác nếu cần

    // Liên kết địa chỉ và cổng với socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        cerr << "Error binding socket." << endl;
        close(serverSocket);
        return -1;
    }

    // Lắng nghe kết nối
    if (listen(serverSocket, 10) == -1)
    {
        cerr << "Error listening on socket." << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Server listening on port 8888..." << endl;

    // Chấp nhận kết nối từ client
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);

    if (clientSocket == -1)
    {
        cerr << "Error accepting connection." << endl;
        close(serverSocket);
        return -1;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    cout << "Connection accepted from " << clientIP << endl;

    while (true)
    {
        // Nhận dữ liệu từ client
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1)
        {
            cerr << "Error receiving data." << endl;
            break;
        }
        else if (bytesRead == 0)
        {
            cout << "Client disconnected." << endl;
            break;
        }
        else
        {
            buffer[bytesRead] = '\0'; // Kết thúc chuỗi
            cout << "Received from client: " << buffer << endl;

            // Kiểm tra nếu là chuỗi "QUIT" thì đóng kết nối và thoát khỏi vòng lặp
            if (strcmp(buffer, "QUIT") == 0)
            {
                cout << "Closing connection..." << endl;
                break;
            }

            // Kiểm tra nếu là chuỗi "SIGNIN" thì thực hiện xác thực đăng nhập
            if (strncmp(buffer, "SIGNIN ", 7) == 0)
            {
                // Lấy username và password từ chuỗi nhận được
                char *signInData = buffer + 7; // Bỏ qua "SIGNIN "
                char *username = strtok(signInData, " ");
                char *password = strtok(nullptr, " ");

                if (username != nullptr && password != nullptr)
                {
                    SQLHDBC hdbc;
                    SQLHENV henv;

                    // Connect to the database
                    if (ConnectToDatabase(hdbc, henv))
                    {
                        cout << "Connected to the database." << endl;

                        // Thực hiện xác thực đăng nhập và nhận thông tin người dùng
                        UserInfo userInfo;
                        if (authenticateUser(hdbc, username, password, userInfo))
                        {
                            // Gửi phản hồi đăng nhập thành công về client
                            string successMessage = "SIGNINCR " + userInfo.userid + " " + userInfo.username;
                            send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
                            cout << "Sent to client: " << successMessage << endl;
                        }
                        else
                        {
                            // Gửi phản hồi đăng nhập thất bại về client
                            const char *failureMessage = "SIGNINICR";
                            send(clientSocket, failureMessage, strlen(failureMessage), 0);
                            cout << "Sent to client: " << failureMessage << endl;
                        }

                        // Disconnect from the database
                        DisconnectFromDatabase(hdbc, henv);
                        cout << "Disconnected from the database." << endl;
                    }
                    else
                    {
                        cerr << "Failed to connect to the database." << endl;
                    }
                }
            }

            // Kiểm tra nếu là chuỗi "CREATEROOM" thì thực hiện tạo phòng
            else if (strncmp(buffer, "CREATEROOM ", 11) == 0)
            {
                // Lấy userid, roomname, và roomdetail từ chuỗi nhận được
                char *createRoomData = buffer + 11; // Bỏ qua "CREATEROOM "
                char *userid = strtok(createRoomData, " ");
                char *roomname = strtok(nullptr, " ");
                char *roomdetail = strtok(nullptr, " ");

                if (userid != nullptr && roomname != nullptr && roomdetail != nullptr)
                {
                    // Tạo phòng và thêm vào cơ sở dữ liệu
                    SQLHDBC hdbc;
                    SQLHENV henv;

                    if (ConnectToDatabase(hdbc, henv))
                    {
                        cout << "Connected to the database." << endl;

                        // Thực hiện tạo phòng và kiểm tra kết quả
                        if (room.createRoom(hdbc, userid, roomname, roomdetail))
                        {
                            // // Gửi phản hồi tạo phòng thành công về client
                            // string roomid = room.getRoomIDAsString(hdbc, roomname);

                            // Sử dụng phương thức c_str() để chuyển đổi std::string thành const char *
                            const char *successMessage = "CREATEROOMCR ";
                            send(clientSocket, successMessage, strlen(successMessage), 0);
                            cout << "Sent to client: " << successMessage << endl;
                        }

                        else
                        {
                            // Gửi phản hồi tạo phòng thất bại về client
                            const char *failureMessage = "CREATEROOMICR";
                            send(clientSocket, failureMessage, strlen(failureMessage), 0);
                            cout << "Sent to client: " << failureMessage << endl;
                        }

                        DisconnectFromDatabase(hdbc, henv);
                        cout << "Disconnected from the database." << endl;
                    }
                    else
                    {
                        cerr << "Failed to connect to the database." << endl;
                    }
                }
            }

            else if (strncmp(buffer, "ADDITEM ", 8) == 0)
            {
                // Lấy RoomID, itemname, startingPrice, AuctionStartTime, AuctionEndTime, và BuyItNowPrice từ chuỗi nhận được
                char *addItemData = buffer + 8; // Bỏ qua "ADDITEM "
                char *roomid = strtok(addItemData, " ");
                char *itemname = strtok(nullptr, " ");
                double startingPrice = atof(strtok(nullptr, " "));
                char *auctionStartTime = strtok(nullptr, " ");
                char *auctionEndTime = strtok(nullptr, " ");
                double buyItNowPrice = atof(strtok(nullptr, " "));

                if (roomid != nullptr && itemname != nullptr && auctionStartTime != nullptr && auctionEndTime != nullptr)
                {
                    // Thêm mục vào phòng đấu giá và kiểm tra kết quả
                    SQLHDBC hdbc;
                    SQLHENV henv;

                    if (ConnectToDatabase(hdbc, henv))
                    {
                        cout << "Connected to the database." << endl;

                        // Thực hiện thêm mục và kiểm tra kết quả
                        if (room.addItem(hdbc, roomid, itemname, startingPrice, auctionStartTime, auctionEndTime, buyItNowPrice))
                        {
                            // Gửi phản hồi thêm mục thành công về client
                            const char *successMessage = "ADDITEMCR";
                            send(clientSocket, successMessage, strlen(successMessage), 0);
                            cout << "Sent to client: " << successMessage << endl;
                        }
                        else
                        {
                            // Gửi phản hồi thêm mục thất bại về client
                            const char *failureMessage = "ADDITEMICR";
                            send(clientSocket, failureMessage, strlen(failureMessage), 0);
                            cout << "Sent to client: " << failureMessage << endl;
                        }

                        DisconnectFromDatabase(hdbc, henv);
                        cout << "Disconnected from the database." << endl;
                    }
                    else
                    {
                        cerr << "Failed to connect to the database." << endl;
                    }
                }
            }

            // Gửi dữ liệu trả về client (hoặc thông báo đến client)
            send(clientSocket, buffer, strlen(buffer), 0);

            // Kiểm tra nếu là chuỗi "QUIT" thì đóng kết nối và thoát khỏi vòng lặp
            if (strcmp(buffer, "QUIT") == 0)
            {
                cout << "Closing connection..." << endl;
                break;
            }
        }
    }

    // Đóng socket
    close(clientSocket);
    close(serverSocket);

    return 0;
}
