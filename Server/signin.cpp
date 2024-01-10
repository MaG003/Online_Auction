// signin.cpp
#include "signin.h"

bool authenticateUser(SQLHDBC hdbc, const char *username, const char *password, UserInfo &userInfo)
{
    SQLHSTMT hstmt;

    // Allocate statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // Query string
    const char *query = "SELECT UserID, UserName FROM Users WHERE UserName = ? AND Password = ?";

    // Prepare statement
    SQLPrepare(hstmt, (SQLCHAR *)query, SQL_NTS);

    // Bind parameters
    SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)username, 0, nullptr);
    SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)password, 0, nullptr);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, userInfo.userid.data(), userInfo.userid.size(), nullptr);
    SQLBindCol(hstmt, 4, SQL_C_CHAR, userInfo.username.data(), userInfo.username.size(), nullptr);

    // Execute the query
    if (SQLExecute(hstmt) == SQL_SUCCESS)
    {
        // Fetch and check if any rows are returned
        if (SQLFetch(hstmt) == SQL_SUCCESS)
        {
            // User authenticated, get userid and username
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
            return true;
        }
    }

    // // User not authenticated, set userid and username to empty
    // userInfo.userid = "";
    // userInfo.username = "";

    // User not authenticated
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return false;
}
