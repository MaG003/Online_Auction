// user.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "user.h"
#include "randomID.h"

User::User()
{

}

User::User(const std::string &userId, const std::string &username, const std::string &password, const std::string &email)
    : userId(userId), username(username), password(password), email(email)
{
    // Khởi tạo đối tượng User với thông tin đầy đủ
}

bool User::isValidLogin(const std::string &enteredUsername, const std::string &enteredPassword) const
{
    return (enteredUsername == username && enteredPassword == password);
}

User User::signupUser(const std::string &userId, const std::string &username, const std::string &password, const std::string &email)
{
    return User(userId, username, password, email);
}

void User::displayUserInfo() const
{
    std::cout << "Username: " << username << std::endl;
    std::cout << "Password: " << password << std::endl;
    std::cout << "Email: " << email << std::endl;
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

std::string User::getUserId() const
{
    return userId;
}
