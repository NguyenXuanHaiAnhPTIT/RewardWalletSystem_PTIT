#include "../include/Transaction.h"
#include <iostream>

// Constructor mặc định
Transaction::Transaction() :
    transactionID(""),
    fromUser(""),
    toUser(""),
    amount(0),
    timeStamp(""),
    status(""),
    note("")
{
}

// Constructor đầy đủ
Transaction::Transaction(const std::string& transactionID,
    const std::string& fromUser,
    const std::string& toUser,
    long long amount,
    const std::string& timeStamp,
    const std::string& status,
    const std::string& note)
    : transactionID(transactionID),
    fromUser(fromUser),
    toUser(toUser),
    amount(amount),
    timeStamp(timeStamp),
    status(status),
    note(note)
{
}

// Hiển thị thông tin giao dịch ra console
void Transaction::display() const {
    std::cout << "Transaction ID : " << transactionID << std::endl;
    std::cout << "From User      : " << fromUser << std::endl;
    std::cout << "To User        : " << toUser << std::endl;
    std::cout << "Amount         : " << amount << std::endl;
    std::cout << "Time           : " << timeStamp << std::endl;
    std::cout << "Status         : " << status << std::endl;
    std::cout << "Note           : " << note << std::endl;
}
