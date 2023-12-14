// user.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "user.h"

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

// Đọc thông tin người dùng
std::vector<User> readUsersFromTxt(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    std::vector<User> users;
    std::string username, password, email;

    while (file >> username >> password >> email)
    {
        users.emplace_back(username, password, email);
    }

    file.close();

    return users;
}

// Ghi thông tin người dùng
void writeUsersToTxt(const std::string &filename, const std::vector<User> &users)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    for (const auto &user : users)
    {
        file << user.getUsername() << " " << user.getPassword() << " " << user.getEmail() << std::endl;
    }

    file.close();
}
