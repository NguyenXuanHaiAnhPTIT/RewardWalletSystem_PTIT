#pragma once
#include <string>

// Đại diện cho ví điểm thưởng của user hoặc admin
class Wallet {
public:
    std::string walletID;    // Hash từ username hoặc 1 mã duy nhất
    std::string owner;       // username sở hữu ví
    long long balance;       // Số điểm hiện tại

    Wallet();
    Wallet(const std::string& walletID, const std::string& owner, long long balance = 0);

    void display() const;
};