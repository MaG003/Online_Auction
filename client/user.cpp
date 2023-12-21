// user.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "user.h"


User::User()
{
    // Thực hiện khởi tạo mặc định nếu cần
}
User::User(const std::string &username, const std::string &password, const std::string &email)
    : username(username), password(password), email(email)
{
    // Khởi tạo đối tượng User với thông tin đầy đủ
}

bool User::isValidLogin(const std::string &enteredUsername, const std::string &enteredPassword) const
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
