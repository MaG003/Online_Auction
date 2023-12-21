#include <iostream>
#include <fstream>
#include <vector>
#include "auction_room.h"
#include "randomID.h"

AuctionRoom::AuctionRoom()
{
}

AuctionRoom::AuctionRoom(const std::string &roomId, const std::string &name, const std::string &details)
    : roomId(roomId), name(name), details(details)
{
    // Định nghĩa constructor với tham số nếu cần thiết
}

void AuctionRoom::startAuction()
{
    std::cout << "Auction started in room: " << name << std::endl;
    // Logic bắt đầu phiên đấu giá
    // Các bước khác để quản lý đấu giá
}

AuctionRoom AuctionRoom::createAuctionRoom(const std::string &roomId, const std::string &roomName, const std::string &roomDetails)
{
    return AuctionRoom(roomId, roomName, roomDetails);
}

std::string AuctionRoom::getRoomId() const
{
    return roomId;
}

std::string AuctionRoom::getName() const
{
    return name;
}

std::string AuctionRoom::getDetails() const
{
    return details;
}
