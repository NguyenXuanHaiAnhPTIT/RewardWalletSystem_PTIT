#pragma once
#include <string>
#include <vector>
#include "UserAccount.h"
#include "Wallet.h"
#include "Transaction.h"
using namespace std;

/*
    Lớp quản lý lưu trữ và truy xuất dữ liệu hệ thống:
    - Đọc/ghi dữ liệu tài khoản, ví, giao dịch
    - Backup và phục hồi dữ liệu hệ thống
    - Tạo thư mục tự động khi chưa tồn tại
*/
class FileManager {
public:
    FileManager();
    // ---- Quản lý User ----
    bool saveUser(const UserAccount& user);               // Lưu thông tin 1 user vào file
    bool loadUser(const string& username, UserAccount& user); // Đọc thông tin 1 user từ file
    vector<UserAccount> loadAllUsers();                   // Đọc toàn bộ danh sách user

    // ---- Quản lý Wallet ----
    void initMasterWalletIfNotExists();
    bool saveWallet(const Wallet& wallet);                // Lưu thông tin 1 ví
    bool loadWallet(const string& username, Wallet& wallet); // Đọc ví theo username

    // ---- Quản lý Transaction ----
    bool saveTransaction(const Transaction& t);           // Lưu 1 giao dịch vào log
    vector<Transaction> loadAllTransactions();            // Đọc toàn bộ lịch sử giao dịch

    // ---- Backup & Restore ----
    bool backupData();                                    // Sao lưu dữ liệu
    bool restoreBackup();                                 // Phục hồi dữ liệu từ bản backup

    // ---- Tiện ích ----
    static void createFolderIfNotExists(const string& path); // Tạo folder nếu chưa có
};
