// user.cpp
#include <iostream>
#include <vector>
#include "user.h"
#include "connect_sql.h"

User::User(const std::string &username, const std::string &password, const std::string &email)
    : username(username), password(password), email(email)
{
    // Khởi tạo đối tượng User với thông tin đầy đủ
}

bool User::loginUser(const std::string &enteredUsername, const std::string &enteredPassword) const
{
    return (enteredUsername == username && enteredPassword == password);
}

User User::signupUser(const std::string &username, const std::string &password, const std::string &email)
{
    // Tạo đối tượng User với thông tin đầy đủ và trả về
    return User(username, password, email);
}

void User::displayUserInfo() const
{
    std::cout << "Username: " << username << ", Email: " << email << std::endl;
}

std::string User::getPassword() const
{
    return password;
}

std::string User::getUsername() const
{
    return username;
}

std::string User::getEmail() const
{
    return email;
}

// Đọc thông tin người dùng từ database SQL Server
std::vector<User> readUsersFromSQLServer()
{
    std::vector<User> users;

    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;

    // Kết nối đến cơ sở dữ liệu
    henv = ConnectToDatabase();
    if (henv == SQL_NULL_HENV)
    {
        // Xử lý lỗi kết nối
        std::cout << "Error connecting to the database." << std::endl;
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

    // Thực hiện truy vấn SQL để đọc thông tin người dùng
    const char *selectQuery = "SELECT UserName, Password, Email FROM Users";
    SQLExecDirect(hstmt, (SQLCHAR *)selectQuery, SQL_NTS);

    SQLCHAR username[255];
    SQLCHAR password[255];
    SQLCHAR email[255];

    SQLLEN cbUsername, cbPassword, cbEmail;

    while (SQL_SUCCEEDED(SQLFetch(hstmt)))
    {
        SQLGetData(hstmt, 1, SQL_C_CHAR, username, sizeof(username), &cbUsername);
        SQLGetData(hstmt, 2, SQL_C_CHAR, password, sizeof(password), &cbPassword);
        SQLGetData(hstmt, 3, SQL_C_CHAR, email, sizeof(email), &cbEmail);

        users.emplace_back(std::string(reinterpret_cast<char *>(username)),
                           std::string(reinterpret_cast<char *>(password)),
                           std::string(reinterpret_cast<char *>(email)));
    }

    // Giải phóng tài nguyên và ngắt kết nối
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    DisconnectFromDatabase(hdbc, henv);

    // Ghi log
    std::cout << "Number of users read from the database: " << users.size() << std::endl;

    return users;
}

// Hàm để ghi thông tin người dùng vào SQL Server
void writeUsersToSQLServer(const std::vector<User> &users)
{
    // Thiết lập kết nối đến SQL Server
    SQLHENV henv = ConnectToDatabase(); // Gọi hàm ConnectToDatabase để nhận SQLHENV
    if (henv == SQL_NULL_HENV)
    {
        // Xử lý lỗi kết nối
        std::cerr << "Error connecting to the database." << std::endl;
        return;
    }

    SQLHDBC hdbc;
    SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    // Thay thế chuỗi kết nối với thông tin kết nối thực tế của bạn
    const char *connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=192.168.16.1;DATABASE=OnlineAuction;UID=admin1;PWD=15251111";
    SQLDriverConnect(hdbc, NULL, (SQLCHAR *)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    // Lặp qua danh sách người dùng và chèn thông tin của họ vào cơ sở dữ liệu SQL Server
    for (const auto &user : users)
    {
        // Bạn cần thay thế câu lệnh INSERT với câu lệnh SQL thực tế của bạn
        const char *insertStatement = "INSERT INTO Users (UserName, Password, Email) VALUES (?, ?, ?)";

        SQLHSTMT hstmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

        // Lưu trữ dữ liệu trong mảng để đảm bảo nó không bị giải phóng trước
        std::vector<char> usernameData(user.getUsername().begin(), user.getUsername().end());
        usernameData.push_back('\0'); // Kết thúc chuỗi null

        std::vector<char> passwordData(user.getPassword().begin(), user.getPassword().end());
        passwordData.push_back('\0'); // Kết thúc chuỗi null

        std::vector<char> emailData(user.getEmail().begin(), user.getEmail().end());
        emailData.push_back('\0'); // Kết thúc chuỗi null

        // Liên kết các tham số
        SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, usernameData.data(), 0, NULL);
        SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, passwordData.data(), 0, NULL);
        SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, emailData.data(), 0, NULL);

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
