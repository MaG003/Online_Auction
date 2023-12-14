#include <iostream>
#include <fstream>
#include <vector>
#include "auction_room.h"

AuctionRoom::AuctionRoom(const std::string &name, const std::string &details) : name(name), details(details)
{
    // Định nghĩa constructor với tham số nếu cần thiết
}

std::string AuctionRoom::getName() const
{
    return name;
}

std::string AuctionRoom::getDetails() const
{
    return details;
}

void AuctionRoom::startAuction()
{
    std::cout << "Auction started in room: " << name << std::endl;
    // Logic bắt đầu phiên đấu giá
    // Các bước khác để quản lý đấu giá
}

AuctionRoom AuctionRoom::createAuctionRoom(const std::string &Roomname, const std::string &Roomdetails)
{
    return AuctionRoom(Roomname, Roomdetails);
}

// Đọc thông tin phòng đấu giá
std::vector<AuctionRoom> readAuctionRoomsFromTxt(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    std::vector<AuctionRoom> rooms;
    std::string roomName, roomDetails;

    while (std::getline(file, roomName) && std::getline(file, roomDetails))
    {
        rooms.emplace_back(roomName, roomDetails);
    }

    file.close();

    return rooms;
}

// Ghi thông tin phòng đấu giá
void writeAuctionRoomsToTxt(const std::string &filename, const std::vector<AuctionRoom> &rooms)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    for (const auto &room : rooms)
    {
        file << room.getName() << " " << room.getDetails() << std::endl;
    }

    file.close();
}

