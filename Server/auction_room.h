// auction_room.h

#ifndef AUCTION_ROOM_H
#define AUCTION_ROOM_H

#include <sql.h>
#include <sqlext.h>
#include <string>

class AuctionRoom
{
public:
    AuctionRoom();
    ~AuctionRoom();

    bool createRoom(SQLHDBC hdbc, const char *userid, const char *roomname, const char *roomdetail);
    std::string getRoomIDAsString(SQLHDBC hdbc, const char *roomname);
    bool addItem(SQLHDBC hdbc, const char *roomid, const char *itemname, double startingPrice, const char *auctionStartTime, const char *auctionEndTime, double buyItNowPrice);

private:
    // Các thành viên khác cần thiết
};

#endif // AUCTION_ROOM_H
