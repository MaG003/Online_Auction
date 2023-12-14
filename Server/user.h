// user.h
#ifndef USER_H
#define USER_H

#include <string>

class User
{
public:
    User(const std::string &username, const std::string &password, const std::string &email);

    std::string getUsername() const;
    std::string getPassword() const;
    std::string getEmail() const;

    bool loginUser(const std::string &enteredUsername, const std::string &enteredPassword) const;
    static User signupUser(const std::string &newUsername, const std::string &newPassword, const std::string &newEmail);
    void displayUserInfo() const;

private:
    std::string username;
    std::string password;
    std::string email;
};

#endif // USER_H
