#pragma once
#include <string>
#include <ctime>

// Sinh, quản lý và xác thực OTP
class OTPManager {
private:
    std::string currentOTP;
    std::time_t createdTime;
    int timeoutSeconds;
public:
    OTPManager();
    std::string generateOTP(int length = 6);   // Sinh OTP gồm chữ HOA + số
    bool validateOTP(const std::string& userInput) const;
    void startCountdown() const;               // In ra console đếm ngược (mỗi 10s)
    void resetOTP();                           // Xoá OTP hiện tại
    std::string getCurrentOTP() const;
    std::time_t getCreatedTime() const;
    void setTimeoutSeconds(int seconds);
    int getTimeoutSeconds() const;
    bool requestOTPWithTimeout();
};