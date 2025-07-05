#include "../include/OTPManager.h"
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <cctype>

// Constructor mặc định
OTPManager::OTPManager() : currentOTP(""), createdTime(0), timeoutSeconds(60) {}

std::string OTPManager::generateOTP(int length) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::mt19937 rng(static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> distrib(0, 35);

    std::string otp;
    for (int i = 0; i < length; ++i) {
        otp += alphanum[distrib(rng)];
    }
    currentOTP = otp;
    createdTime = std::time(nullptr);
    return otp;
}

bool OTPManager::validateOTP(const std::string& userInput) const {
    std::time_t now = std::time(nullptr);
    // So sánh không phân biệt hoa thường
    if ((now - createdTime) > timeoutSeconds) return false;
    if (userInput.length() != currentOTP.length()) return false;
    for (size_t i = 0; i < currentOTP.length(); ++i) {
        if (std::toupper(userInput[i]) != std::toupper(currentOTP[i])) return false;
    }
    return true;
}

void OTPManager::startCountdown() const {
    int remain = timeoutSeconds;
    while (remain > 0) {
        std::cout << "   (OTP con " << remain << " giay)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        remain -= 10;
        if (remain <= 0) {
            std::cout << "OTP da het han!" << std::endl;
        }
    }
}

bool OTPManager::requestOTPWithTimeout() {
    generateOTP();

    std::cout << "Ma OTP cua ban la: " << currentOTP
        << " (co hieu luc trong " << timeoutSeconds << " giay)" << std::endl;

    auto start = std::chrono::steady_clock::now();
    std::string userOTP;
    int printedSecond = 0;

    while (true) {
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        int remain = timeoutSeconds - elapsed;

        if (remain > 0 && remain != timeoutSeconds && remain % 10 == 0 && remain != printedSecond) {
            std::cout << "OTP con hieu luc trong " << remain << " giay...\n";
            printedSecond = remain;
        }
        if (remain <= 0) {
            std::cout << "OTP da het hieu luc!\n";
            std::cout << "Ban co muon nhan ma OTP moi? (Y/N): ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                return requestOTPWithTimeout();
            }
            else {
                std::cout << "Tac vu bi huy.\n";
                return false;
            }
        }

        std::cout << "Nhap ma OTP: ";
        auto inputStart = std::chrono::steady_clock::now();
        std::getline(std::cin, userOTP);
        auto inputNow = std::chrono::steady_clock::now();

        int inputElapsed = std::chrono::duration_cast<std::chrono::seconds>(inputNow - start).count();
        if (inputElapsed > timeoutSeconds) {
            std::cout << "Ban da nhap cham, OTP da het hieu luc!\n";
            std::cout << "Ban co muon nhan ma OTP moi? (Y/N): ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                return requestOTPWithTimeout();
            }
            else {
                std::cout << "Tac vu bi huy.\n";
                return false;
            }
        }

        std::transform(userOTP.begin(), userOTP.end(), userOTP.begin(), ::toupper);

        if (userOTP == currentOTP) {
            std::cout << "Xac thuc thanh cong!\n";
            return true;
        }
        else {
            std::cout << "Ma OTP sai! OTP da het hieu luc.\n";
            std::cout << "Ban co muon nhan ma OTP moi? (Y/N): ";
            std::string ans;
            std::getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                return requestOTPWithTimeout();
            }
            else {
                std::cout << "Tac vu bi huy.\n";
                return false;
            }
        }
    }
}
void OTPManager::resetOTP() {
    currentOTP = "";
    createdTime = 0;
}

std::string OTPManager::getCurrentOTP() const {
    return currentOTP;
}

std::time_t OTPManager::getCreatedTime() const {
    return createdTime;
}

void OTPManager::setTimeoutSeconds(int seconds) {
    timeoutSeconds = seconds;
}

int OTPManager::getTimeoutSeconds() const {
    return timeoutSeconds;
}
