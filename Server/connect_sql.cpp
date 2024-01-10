// connect_sql.cpp
#include "connect_sql.h"

// Function to connect to the database
SQLHENV ConnectToDatabase(SQLHDBC &hdbc, SQLHENV &henv)
{
    // Allocate environment handle
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    // Set the ODBC version environment attribute
    SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);

    // Allocate connection handle
    SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    // Connection string (replace placeholders with your actual connection details)
    const char *connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=192.168.16.1;DATABASE=OnlineAuction;UID=admin1;PWD=15251111";

    // Connect to the database
    SQLDriverConnect(hdbc, nullptr, (SQLCHAR *)connStr, SQL_NTS, nullptr, 0, nullptr, SQL_DRIVER_COMPLETE);

    return henv;
}

// Function to disconnect from the database
void DisconnectFromDatabase(SQLHDBC hdbc, SQLHENV henv)
{
    // Disconnect
    SQLDisconnect(hdbc);

    // Free handles
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
}