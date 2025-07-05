#pragma once
#include <string>

// Giao dịch chuyển điểm giữa các ví
class Transaction {
public:
    std::string transactionID;   // Sinh bằng timestamp + random
    std::string fromUser;
    std::string toUser;
    long long amount;
    std::string timeStamp;       // Lưu dưới dạng string
    std::string status;          // "Success" / "Failed"
    std::string note;            // Lý do thất bại...

    Transaction();
    Transaction(const std::string& transactionID,
        const std::string& fromUser,
        const std::string& toUser,
        long long amount,
        const std::string& timeStamp,
        const std::string& status,
        const std::string& note);

    void display() const;
};