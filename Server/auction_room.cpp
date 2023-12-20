#include <iostream>
#include <fstream>
#include <vector>
#include "auction_room.h"
#include "connect_sql.h"

AuctionRoom::AuctionRoom(const std::string &name, const std::string &details) : name(name), details(details)
{
    // Định nghĩa constructor với tham số nếu cần thiết
}

std::string AuctionRoom::getName() const
{
    return name;
}

std::string AuctionRoom::getDetails() const
{
    return details;
}

void AuctionRoom::startAuction()
{
    std::cout << "Auction started in room: " << name << std::endl;
    // Logic bắt đầu phiên đấu giá
    // Các bước khác để quản lý đấu giá
}

AuctionRoom AuctionRoom::createAuctionRoom(const std::string &Roomname, const std::string &Roomdetails)
{
    return AuctionRoom(Roomname, Roomdetails);
}

// Hàm để đọc thông tin phòng đấu giá từ database SQL Server
std::vector<AuctionRoom> readAuctionRoomsFromSQLServer()
{
    std::vector<AuctionRoom> rooms;

    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;

    // Kết nối đến cơ sở dữ liệu
    henv = ConnectToDatabase();
    if (henv == SQL_NULL_HENV)
    {
        // Xử lý lỗi kết nối
        std::cerr << "Error connecting to the database." << std::endl;
        return {};
    }

    // Mở một kết nối SQL
    SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    // Chuỗi kết nối
    const char *connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=192.168.16.1;DATABASE=OnlineAuction;UID=admin1;PWD=15251111";

    // Thực hiện kết nối
    SQLDriverConnect(hdbc, NULL, (SQLCHAR *)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    // Tạo một tay quản lý câu lệnh SQL
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // Thực hiện truy vấn SQL để đọc thông tin phòng đấu giá
    const char *selectQuery = "SELECT RoomName, RoomDetails FROM AuctionRooms";
    SQLExecDirect(hstmt, (SQLCHAR *)selectQuery, SQL_NTS);

    SQLCHAR roomName[255];
    SQLCHAR roomDetails[255];

    SQLLEN cbRoomName, cbRoomDetails;

    while (SQL_SUCCEEDED(SQLFetch(hstmt)))
    {
        SQLGetData(hstmt, 1, SQL_C_CHAR, roomName, sizeof(roomName), &cbRoomName);
        SQLGetData(hstmt, 2, SQL_C_CHAR, roomDetails, sizeof(roomDetails), &cbRoomDetails);

        rooms.emplace_back(std::string(reinterpret_cast<char *>(roomName)),
                           std::string(reinterpret_cast<char *>(roomDetails)));
    }

    // Giải phóng tài nguyên và ngắt kết nối
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    DisconnectFromDatabase(hdbc, henv);

    // Ghi log
    std::cout << "Number of auction rooms read from the database: " << rooms.size() << std::endl;

    return rooms;
}

// Hàm để ghi thông tin phòng đấu giá vào database SQL Server
void writeAuctionRoomsToSQLServer(const std::vector<AuctionRoom> &rooms)
{
    // Thiết lập kết nối đến SQL Server
    SQLHENV henv = ConnectToDatabase();
    if (henv == SQL_NULL_HENV)
    {
        // Xử lý lỗi kết nối
        std::cerr << "Error connecting to the database." << std::endl;
        return;
    }

    SQLHDBC hdbc;
    SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    // Chuỗi kết nối
    const char *connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=192.168.16.1;DATABASE=OnlineAuction;UID=admin1;PWD=15251111";
    SQLDriverConnect(hdbc, NULL, (SQLCHAR *)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    // Lặp qua danh sách phòng đấu giá và chèn thông tin của chúng vào cơ sở dữ liệu SQL Server
    for (const auto &room : rooms)
    {
        // Bạn cần thay thế câu lệnh INSERT với câu lệnh SQL thực tế của bạn
        const char *insertStatement = "INSERT INTO AuctionRooms (RoomName, RoomDetails) VALUES (?, ?)";

        SQLHSTMT hstmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

        // Lưu trữ dữ liệu trong mảng để đảm bảo nó không bị giải phóng trước
        std::vector<char> roomnameData(room.getName().begin(), room.getName().end());
        roomnameData.push_back('\0'); // Kết thúc chuỗi null

        std::vector<char> roomdetailsData(room.getDetails().begin(), room.getDetails().end());
        roomdetailsData.push_back('\0'); // Kết thúc chuỗi null

        // Liên kết các tham số
        SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, roomnameData.data(), 0, NULL);
        SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, roomdetailsData.data(), 0, NULL);

        // Thực hiện câu lệnh SQL
        SQLExecDirect(hstmt, (SQLCHAR *)insertStatement, SQL_NTS);

        // Giải phóng tay quản lý câu lệnh
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }

    // Ngắt kết nối và giải phóng các tay quản lý
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
}
