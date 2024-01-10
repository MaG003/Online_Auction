#ifndef AUCTION_SESSION_H
#define AUCTION_SESSION_H

#include <chrono>
#include <string>

class AuctionSession {
public:
    AuctionSession();
    ~AuctionSession();

    // Bắt đầu phiên đấu giá
    void startAuction();

    // Tra giá
    void placeBid(const std::string& bidder, double bidAmount);

private:
    // Cập nhật đồng hồ và giá
    void updateAuction();

    // Lưu giữ thời gian còn lại
    std::chrono::seconds timeRemaining;

    // Lưu giữ thời gian đồng hồ đã chạy
    std::chrono::seconds elapsedTime;

    // Lưu giữ giá cao nhất và người đặt giá cao nhất
    double highestBid;
    std::string highestBidder;

    // Kiểm tra xem đang trong quá trình đấu giá hay không
    bool auctionInProgress;
};

#endif  // AUCTION_SESSION_H
