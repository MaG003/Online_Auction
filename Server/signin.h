// signin.h
#pragma once

#include <sql.h>
#include <sqlext.h>
#include <string>

struct UserInfo {
    std::string userid;
    std::string username;
};

bool authenticateUser(SQLHDBC hdbc, const char *username, const char *password, UserInfo &userInfo);
