#pragma once
#include <string>
#include <functional>
#include "UserAccount.h"
#include "Wallet.h"
#include "Transaction.h"
#include "FileManager.h"
#include "OTPManager.h"
using namespace std;

/*
    Lớp điều phối toàn bộ luồng nghiệp vụ chính của hệ thống:
    - Xử lý các chức năng đăng nhập, đăng ký, quản lý tài khoản, phân quyền, chuyển điểm, nạp điểm, backup...
    - Điều hướng giao diện menu chính (User/Admin)
    - Kiểm soát an toàn nhập liệu, xác thực, phân quyền, clear screen
    - Kết nối tất cả các module: User, Wallet, Transaction, OTP, FileManager
*/
class SystemManager {
private:
    UserAccount* currentUser;    // Tài khoản đang đăng nhập
    Wallet* currentWallet;       // Ví của tài khoản hiện tại
    FileManager fileManager;     // Quản lý dữ liệu
    OTPManager otpManager;       // Quản lý xác thực OTP

public:
    SystemManager();

    // ----- Giao diện chính -----
    void mainMenu();                        // Hiển thị menu chính hệ thống

    // ----- Nghiệp vụ user/admin -----
    void registerUser();                    // Đăng ký tài khoản người dùng
    void registerUserAsAdmin();             // Admin đăng ký hộ tài khoản
    void login();                           // Đăng nhập
    void forgotPassword();                  // Quên mật khẩu (nhập username từ menu)
    void forgotPassword(const string& username); // Quên mật khẩu khi đã nhập username
    string generateRandomPassword(const string& username); // Sinh mật khẩu tự động theo quy tắc

    void logout();                          // Đăng xuất
    void changePassword();                  // Đổi mật khẩu
    void updatePersonalInfo();              // Cập nhật thông tin cá nhân

    // ----- Giao dịch, quản lý ví -----
    void transferPoints();                  // Chuyển điểm cho user khác
    void topupUserWallet();                 // Admin nạp điểm cho user

    // ----- Quản lý tài khoản -----
    void lockUser();                        // Admin khóa tài khoản user
    void unlockUser();                      // Admin mở khóa tài khoản user
    void backup();                          // Sao lưu dữ liệu
    void restore();                         // Phục hồi dữ liệu từ backup

    // ----- Xem báo cáo, danh sách -----
    void viewTransactionLog();              // Xem lịch sử giao dịch
    void viewUserList();                    // Xem danh sách người dùng (admin)

    // ----- Menu chức năng -----
    void adminMenu();                       // Menu dành cho Admin
    void userMenu();                        // Menu dành cho User
    void resetUserPasswordByAdmin();        // Admin đổi mật khẩu hộ user
};
bool safeInput(
    const std::string& prompt,
    std::function<bool(const std::string&)> validator,
    std::string& out,
    const std::string& errMsg,
    bool allowBack = true
);
