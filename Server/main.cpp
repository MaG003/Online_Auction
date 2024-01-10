// main.cpp
#include "connect_sql.h"
#include <iostream>

void ExecuteSampleQuery(SQLHDBC hdbc)
{
    SQLHSTMT hstmt;

    // Allocate statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // Sample query (replace with your actual query)
    const char *query = "SELECT * FROM Users";

    // Execute the query
    SQLExecDirect(hstmt, (SQLCHAR *)query, SQL_NTS);

    // Fetch and print results
    SQLCHAR username[256];
    while (SQLFetch(hstmt) == SQL_SUCCESS)
    {
        SQLGetData(hstmt, 2, SQL_C_CHAR, username, sizeof(username), nullptr);
        std::cout << "Username: " << username << std::endl;
    }

    // Free statement handle
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

int main()
{
    SQLHDBC hdbc;
    SQLHENV henv;

    // Connect to the database
    if (ConnectToDatabase(hdbc, henv))
    {
        std::cout << "Connected to the database." << std::endl;

        // Execute sample query
        ExecuteSampleQuery(hdbc);

        // Disconnect from the database
        DisconnectFromDatabase(hdbc, henv);
        std::cout << "Disconnected from the database." << std::endl;
    }
    else
    {
        std::cerr << "Failed to connect to the database." << std::endl;
    }

    return 0;
}
