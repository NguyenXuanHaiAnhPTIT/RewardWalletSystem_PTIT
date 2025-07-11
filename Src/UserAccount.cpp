﻿#include "../include/UserAccount.h"
#include <iostream>
#include "../sha256/Sha256.h"
#include "../Include/Wallet.h"
#include "../Include/FileManager.h"

// Constructor mặc định
UserAccount::UserAccount()
    : username(""), hashedPassword(""), fullName(""), email(""), phone(""),
    isAdmin(false), isLocked(false), isPasswordAutoGenerated(false)
{
}

// Constructor đầy đủ
UserAccount::UserAccount(const std::string& username,
    const std::string& hashedPassword,
    const std::string& fullName,
    const std::string& email,
    const std::string& phone,
    bool isAdmin,
    bool isLocked,
    bool isPasswordAutoGenerated)
    : username(username),
    hashedPassword(hashedPassword),
    fullName(fullName),
    email(email),
    phone(phone),
    isAdmin(isAdmin),
    isLocked(isLocked),
    isPasswordAutoGenerated(isPasswordAutoGenerated)
{
}

void UserAccount::displayInfo() const {
    std::cout << "------ User Information ------\n";
    std::cout << "Username    : " << username << "\n";
    std::cout << "Full Name   : " << fullName << "\n";
    std::cout << "Email       : " << email << "\n";
    std::cout << "Phone       : " << phone << "\n";
    std::cout << "Role        : " << (isAdmin ? "Admin" : "User") << "\n";
    std::cout << "Status      : " << (isLocked ? "Locked" : "Active") << "\n";
    std::cout << "Password Type : " << (isPasswordAutoGenerated ? "Auto" : "Custom") << "\n";
    // Hiển thị số dư ví
    Wallet w;
    if (FileManager().loadWallet(username, w)) {
        std::cout << "Wallet Balance: " << w.balance << " diem\n";
    }
    else {
        std::cout << "Wallet Balance: [Khong the tai thong tin vi]\n";
    }
}

bool UserAccount::checkPassword(const std::string& inputPassword) const {
    return hashedPassword == Sha256(inputPassword);
}