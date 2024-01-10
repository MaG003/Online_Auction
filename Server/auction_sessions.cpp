#include "auction_sessions.h"
#include <iostream>
#include <thread>

using namespace std; 
AuctionSession::AuctionSession() : timeRemaining(0), elapsedTime(0), highestBid(0), highestBidder(""), auctionInProgress(false) {}

AuctionSession::~AuctionSession() {}

void AuctionSession::startAuction() {
    // Bắt đầu đồng hồ đếm ngược
    timeRemaining = chrono::seconds(600); // 10 phút đầu giá
    elapsedTime = chrono::seconds(0);
    auctionInProgress = true;

    while (timeRemaining.count() > 0) {
        this_thread::sleep_for(chrono::seconds(1));
        elapsedTime += chrono::seconds(1);
        timeRemaining -= chrono::seconds(1);

        // Cập nhật đồng hồ và giá mỗi 30 giây
        if (elapsedTime.count() % 30 == 0) {
            updateAuction();
        }
    }

    // Hết thời gian, đặt giá bán và lưu vào cơ sở dữ liệu
    auctionInProgress = false;
    cout << "Auction ended. Final price: " << highestBid << " - Winner: " << highestBidder << endl;
    // Lưu vào cơ sở dữ liệu ở đây
    
}

void AuctionSession::placeBid(const string& bidder, double bidAmount) {
    if (!auctionInProgress) {
        cout << "Auction not in progress." << endl;
        return;
    }

    if (bidAmount > highestBid) {
        highestBid = bidAmount;
        highestBidder = bidder;
        elapsedTime = chrono::seconds(0); // Reset đồng hồ khi có người đặt giá cao hơn
        cout << "Bid placed by " << bidder << " - Current highest bid: " << highestBid << endl;
    } else {
        cout << "Bid too low." << endl;
    }
}

void AuctionSession::updateAuction() {
    // Cập nhật đồng hồ và giá
    cout << "Time remaining: " << timeRemaining.count() << "s - Current highest bid: " << highestBid << endl;
}
