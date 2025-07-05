#include "../include/Wallet.h"
#include <iostream>

// Constructor mặc định
Wallet::Wallet()
    : walletID(""), owner(""), balance(0)
{
}

// Constructor đầy đủ
Wallet::Wallet(const std::string& walletID, const std::string& owner, long long balance)
    : walletID(walletID), owner(owner), balance(balance)
{
}

void Wallet::display() const {
    std::cout << "----- Wallet Info -----" << std::endl;
    std::cout << "Wallet ID : " << walletID << std::endl;
    std::cout << "Owner     : " << owner << std::endl;
    std::cout << "Balance   : " << balance << std::endl;
}