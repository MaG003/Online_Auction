#ifndef AUCTION_ROOM_H
#define AUCTION_ROOM_H

#include <string>

class AuctionRoom
{
public:
    AuctionRoom();
    AuctionRoom(const std::string &name, const std::string &details);

    std::string getName() const;
    std::string getDetails() const;

    void startAuction();
    static AuctionRoom createAuctionRoom(const std::string &newRoomname, const std::string &newRoomdetails);
    static void viewAuctionRooms(const std::string &filename);

private:
    std::string name;
    std::string details; 
};

#endif // AUCTION_ROOM_H
