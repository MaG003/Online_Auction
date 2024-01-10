// auction_room.cpp

#include "auction_room.h"
#include <iostream>
#include <cstring>
#include <iconv.h>

AuctionRoom::AuctionRoom()
{
    // Khởi tạo
}

AuctionRoom::~AuctionRoom()
{
    // Hủy
}

// Tao phong dau gia
bool AuctionRoom::createRoom(SQLHDBC hdbc, const char *userid, const char *roomname, const char *roomdetail)
{
    SQLHSTMT hstmt;

    // Allocate statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // Query string
    const char *query = "INSERT INTO AuctionRooms (RoomName, RoomDetails, RoomAdmin) VALUES (?, ?, ?);";

    // Bind parameters
    SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)roomname, 0, nullptr);
    SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)roomdetail, 0, nullptr);
    SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)userid, 0, nullptr);

    // Prepare statement
    SQLPrepare(hstmt, (SQLCHAR *)query, SQL_NTS);

    // Execute the query
    if (SQLExecute(hstmt) == SQL_SUCCESS)
    {
        // Room creation succeeded
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return true;
    }
    else
    {
        // Room creation failed, print error information
        SQLSMALLINT recordNumber;
        SQLSMALLINT sqlStateLength;
        SQLINTEGER nativeError;
        SQLCHAR sqlState[6];
        SQLCHAR messageText[SQL_MAX_MESSAGE_LENGTH];

        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &sqlStateLength);

        std::cerr << "SQL Error: " << messageText << " (SQLState: " << sqlState << ", NativeError: " << nativeError << ")" << std::endl;

        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }
}

// std::string convertUTF16toUTF8(const char16_t *utf16Data, size_t utf16Length)
// {
//     iconv_t iconvInstance = iconv_open("UTF-8", "UTF-16LE");
//     if (iconvInstance == (iconv_t)-1)
//     {
//         std::cerr << "Failed to initialize iconv" << std::endl;
//         return "";
//     }

//     // Use a reasonable initial buffer size
//     size_t bufferSize = utf16Length * 4;
//     char *outBuffer = new char[bufferSize];
//     char *outPointer = outBuffer;
//     size_t outBytesLeft = bufferSize;

//     const char *inBuffer = reinterpret_cast<const char *>(utf16Data);
//     size_t inBytesLeft = utf16Length * sizeof(char16_t);

//     if (iconv(iconvInstance, &inBuffer, &inBytesLeft, &outPointer, &outBytesLeft) == (size_t)-1)
//     {
//         std::cerr << "Conversion from UTF-16 to UTF-8 failed" << std::endl;
//         iconv_close(iconvInstance);
//         delete[] outBuffer;
//         return "";
//     }

//     iconv_close(iconvInstance);

//     std::string utf8Data(outBuffer, bufferSize - outBytesLeft);
//     delete[] outBuffer;

//     return utf8Data;
// }

// std::string AuctionRoom::getRoomIDAsString(SQLHDBC hdbc, const char *roomname)
// {
//     SQLHSTMT hstmt;

//     // Allocate statement handle
//     SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

//     // Query string
//     const char *query = "SELECT CONVERT(VARCHAR(36), RoomID) AS ConvertedString FROM AuctionRooms WHERE RoomName = ?;";

//     // Bind parameters
//     SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)roomname, 0, nullptr);

//     // Prepare statement
//     SQLPrepare(hstmt, (SQLCHAR *)query, SQL_NTS);

//     // Execute the query
//     if (SQLExecute(hstmt) == SQL_SUCCESS)
//     {
//         // Fetch the result set
//         if (SQLFetch(hstmt) == SQL_SUCCESS)
//         {
//             SQLCHAR roomidBuffer[37]; // Buffer to store the converted RoomID
//             SQLLEN roomidIndicator;

//             SQLBindCol(hstmt, 1, SQL_C_CHAR, roomidBuffer, sizeof(roomidBuffer), &roomidIndicator);

//             if (roomidIndicator != SQL_NULL_DATA)
//             {
//                 // RoomID retrieval succeeded

//                 // Chuyển đổi từ UTF-16 (SQLCHAR) sang UTF-8 (std::string)
//                 std::u16string utf16Data(reinterpret_cast<char16_t *>(roomidBuffer));
//                 std::string utf8Data = convertUTF16toUTF8(utf16Data.c_str());

//                 SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
//                 return utf8Data;
//             }
//         }
//     }
//     else
//     {
//         // Error in SQL execution
//         SQLSMALLINT recordNumber;
//         SQLSMALLINT sqlStateLength;
//         SQLINTEGER nativeError;
//         SQLCHAR sqlState[6];
//         SQLCHAR messageText[SQL_MAX_MESSAGE_LENGTH];

//         SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &sqlStateLength);

//         std::cerr << "SQL Error: " << messageText << " (SQLState: " << sqlState << ", NativeError: " << nativeError << ")" << std::endl;
//     }

//     // Clean up in case of failure
//     SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

//     // Return an empty string in case of failure
//     return "";
// }


// Them item vao phong dau gia
bool AuctionRoom::addItem(SQLHDBC hdbc, const char *roomid, const char *itemname, double startingPrice, const char *auctionStartTime, const char *auctionEndTime, double buyItNowPrice)
{
    SQLHSTMT hstmt;

    // Allocate statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

    // Query string
    const char *query = "INSERT INTO AuctionItems (RoomID, ItemName, StartingPrice, AuctionStartTime, AuctionEndTime, BuyItNowPrice) VALUES (?, ?, ?, ?, ?, ?);";

    // Bind parameters
    SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)roomid, 0, nullptr);
    SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)itemname, 0, nullptr);
    SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &startingPrice, 0, nullptr);
    SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_TIMESTAMP, 0, 0, (SQLPOINTER)auctionStartTime, 0, nullptr);
    SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_TIMESTAMP, 0, 0, (SQLPOINTER)auctionEndTime, 0, nullptr);
    SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &buyItNowPrice, 0, nullptr);

    // Prepare statement
    SQLPrepare(hstmt, (SQLCHAR *)query, SQL_NTS);

    // Execute the query
    if (SQLExecute(hstmt) == SQL_SUCCESS)
    {
        // Item addition succeeded
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return true;
    }
    else
    {
        // Item addition failed, print error information
        SQLSMALLINT recordNumber;
        SQLSMALLINT sqlStateLength;
        SQLINTEGER nativeError;
        SQLCHAR sqlState[6];
        SQLCHAR messageText[SQL_MAX_MESSAGE_LENGTH];

        SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &sqlStateLength);

        std::cerr << "SQL Error: " << messageText << " (SQLState: " << sqlState << ", NativeError: " << nativeError << ")" << std::endl;

        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }
}
