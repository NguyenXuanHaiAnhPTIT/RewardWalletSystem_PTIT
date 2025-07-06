#include "../Include/SystemManager.h"
#include "../Sha256/Sha256.h"
#include <iostream>
#include <random>
#include <chrono>
#include <regex>
#include <conio.h>
#include <cstdlib>
#include <filesystem>
#include "../Include/Wallet.h"
using namespace std;

// Hàm ẩn mật khẩu
std::string inputPasswordHidden() {
    std::string password;
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) { // Enter
            std::cout << std::endl;
            break;
        }
        else if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        }
        // Chỉ cho phép ký tự ASCII (không dấu, không Unicode)
        else if (ch >= 32 && ch <= 126) { // các ký tự có thể in ra màn hình (ASCII)
            password += ch;
            std::cout << '*';
        }
        // Nếu là ký tự ngoài ASCII thì bỏ qua hoặc có thể thêm dòng cảnh báo nhẹ
        // else { std::cout << "\nChi nhap ky tu khong dau (ASCII)!\n"; }
    }
    return password;
}

// Hàm xóa màn hình console (Windows/Mac)
void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

// Hàm nhập liệu an toàn, theo quy tắc toàn dự án
bool safeInput(
    const std::string& prompt,
    std::function<bool(const std::string&)> validator,
    std::string& out,
    const std::string& errMsg,
    bool allowBack
) {
    int fail = 0;
    while (true) {
        if (allowBack)
            std::cout << "(Nhan B/b de quay lai)\n";
        std::cout << prompt;
        std::getline(std::cin, out);
        if (allowBack && (out == "B" || out == "b")) {
            std::cout << "Ban da chon quay lai. Tro ve menu truoc...\n";
            return false;
        }
        if (validator(out)) return true;
        std::cout << errMsg << "\n";
        ++fail;
        if (fail >= 3) {
            std::string opt;
            std::cout << "Nhap sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            std::getline(std::cin, opt);
            if (opt == "Y" || opt == "y") return false;
            fail = 0;
        }
    }
}

bool validateEmailInput(const string& s) {
    if (s.empty()) return true;
    if (s.length() > 50) return false;
    static const regex pattern(
        R"((^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$))"
    );
    return regex_match(s, pattern);
}

// --- Constructor ---
SystemManager::SystemManager() {
    currentUser = nullptr;
    currentWallet = nullptr;
    fileManager.initMasterWalletIfNotExists();
}

// --- Menu chính ---
void SystemManager::mainMenu() {
    while (true) {
        clearScreen();
        cout << "-----------------------------------------------\n";
        cout << "         REWARD WALLET SYSTEM - PTIT SEM3      \n";
        cout << "-----------------------------------------------\n";
        cout << "1. Dang nhap      2. Dang ky      3. Thoat\n";
        cout << "-----------------------------------------------\n";

        string choice;
        bool ok = safeInput("Lua chon (1-3): ", [](const string& s) {
            return s == "1" || s == "2" || s == "3";
            }, choice, "Lua chon khong hop le.");

        if (!ok) return;
        if (choice == "1") login();
        else if (choice == "2") registerUser();
        else if (choice == "3") {
            cout << "Tam biet!\n";
            break;
        }
    }
}

// --- Đăng ký tài khoản người dùng ---
void SystemManager::registerUser() {
    clearScreen();
    UserAccount user;
    cout << "--- DANG KY TAI KHOAN ---\n";

    // Nhập username, kiểm tra trùng, không quá dài
    bool ok = safeInput(
        "Username (*): ",
        [&](const string& s) {
            if (s.empty() || s.length() > 50) return false;
            UserAccount tmp;
            return !fileManager.loadUser(s, tmp);
        },
        user.username,
        "Username khong duoc de trong, vuot 50 ky tu hoac da ton tai."
    );
    if (!ok) { cout << "Huy tac vu dang ky.\n"; return; }

    // Nhập số điện thoại hợp lệ
    ok = safeInput(
        "So dien thoai (*): ",
        [](const string& s) {
            if (s.empty() || s.length() < 9 || s.length() > 12) return false;
            for (char c : s) if (!isdigit(c)) return false;
            return true;
        },
        user.phone,
        "So dien thoai chi duoc chua so va co do dai tu 9 den 12 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu dang ky.\n"; return; }

    // Họ tên có thể bỏ trống, tối đa 50 ký tự
    ok = safeInput(
        "Ho ten (bo trong neu khong co): ",
        [](const string& s) { return s.empty() || s.length() <= 50; },
        user.fullName,
        "Ho ten khong duoc vuot qua 50 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu dang ky.\n"; return; }

    // Email có thể bỏ trống, nếu có phải hợp lệ
    ok = safeInput(
        "Email (bo trong neu khong co): ",
        validateEmailInput,
        user.email,
        "Email khong dung dinh dang hoac vuot qua 50 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu dang ky.\n"; return; }

    // Nhập mật khẩu hoặc để trống để tự sinh
    string password, confirm;
    int fail = 0;
    while (true) {
        cout << "Nhap mat khau (bo trong de dung mat khau tu sinh): ";
        password = inputPasswordHidden();

        if (!password.empty()) {
            if (password.length() > 50) {
                cout << "Mat khau khong duoc vuot qua 50 ky tu!\n";
                ++fail;
            }
            else {
                bool hasLower = false, hasDigit = false, hasSpecial = false;
                for (char ch : password) {
                    if (islower(ch)) hasLower = true;
                    else if (isdigit(ch)) hasDigit = true;
                    else if (!isalnum(ch)) hasSpecial = true;
                }
                if (!(hasLower && hasDigit && hasSpecial)) {
                    cout << "Mat khau phai co it nhat 1 chu thuong, 1 chu so va 1 ky tu dac biet.\n";
                    ++fail;
                }
                else {
                    cout << "Nhap lai mat khau: ";
                    confirm = inputPasswordHidden();
                    if (confirm != password) {
                        cout << "Mat khau khong trung khop.\n";
                        ++fail;
                    }
                    else {
                        user.isPasswordAutoGenerated = false;
                        break;
                    }
                }
            }
        }
        else {
            password = generateRandomPassword(user.username);
            cout << "Mat khau tu sinh: " << password << "\n";
            user.isPasswordAutoGenerated = true;
            break;
        }
        if (fail >= 3) {
            string opt;
            cout << "Nhap mat khau sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            getline(cin, opt);
            if (opt != "Y" && opt != "y") { cout << "Huy tac vu dang ky.\n"; return; }
            fail = 0;
        }
    }

    user.hashedPassword = Sha256(password);
    user.isAdmin = false;
    user.isLocked = false;
    // Xác thực OTP trước khi lưu tài khoản
    cout << "Nhap ma OTP de xac nhan dang ky tai khoan...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Dang ky that bai do xac thuc OTP!\n";
        return;
    }

    if (fileManager.saveUser(user)) {
        // Tạo ví mới với số dư 0 điểm
        Wallet w(user.username + "_wallet", user.username, 0);
        fileManager.saveWallet(w);
        cout << "Dang ky thanh cong! Vui long dang nhap.\n";
    }
    else cout << "Loi khi luu tai khoan!\n";
}
// --- Admin đăng ký hộ tài khoản ---
void SystemManager::registerUserAsAdmin() {
    clearScreen();
    UserAccount user;
    cout << "--- ADMIN TAO TAI KHOAN HO NGUOI DUNG ---\n";

    bool ok = safeInput(
        "Username (*): ",
        [&](const string& s) {
            if (s.empty() || s.length() > 50) return false;
            UserAccount tmp;
            return !fileManager.loadUser(s, tmp);
        },
        user.username,
        "Username khong duoc de trong, vuot 50 ky tu hoac da ton tai."
    );
    if (!ok) { cout << "Huy tac vu tao tai khoan.\n"; return; }

    ok = safeInput(
        "So dien thoai (*): ",
        [](const string& s) {
            if (s.empty() || s.length() < 9 || s.length() > 12) return false;
            for (char c : s) if (!isdigit(c)) return false;
            return true;
        },
        user.phone,
        "So dien thoai chi duoc chua so va co do dai tu 9 den 12 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu tao tai khoan.\n"; return; }

    ok = safeInput(
        "Ho ten (bo trong neu khong co): ",
        [](const string& s) { return s.empty() || s.length() <= 50; },
        user.fullName,
        "Ho ten khong duoc vuot qua 50 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu tao tai khoan.\n"; return; }

    ok = safeInput(
        "Email (bo trong neu khong co): ",
        validateEmailInput,
        user.email,
        "Email khong dung dinh dang hoac vuot qua 50 ky tu!"
    );
    if (!ok) { cout << "Huy tac vu tao tai khoan.\n"; return; }

    string password, confirm;
    int fail = 0;
    while (true) {
        cout << "Nhap mat khau (bo trong de dung mat khau tu sinh): ";
        password = inputPasswordHidden();
        if (!password.empty()) {
            if (password.length() > 50) {
                cout << "Mat khau khong duoc vuot qua 50 ky tu!\n";
                ++fail;
            }
            else {
                bool hasLower = false, hasDigit = false, hasSpecial = false;
                for (char ch : password) {
                    if (islower(ch)) hasLower = true;
                    else if (isdigit(ch)) hasDigit = true;
                    else if (!isalnum(ch)) hasSpecial = true;
                }
                if (!(hasLower && hasDigit && hasSpecial)) {
                    cout << "Mat khau phai co it nhat 1 chu thuong, 1 chu so va 1 ky tu dac biet.\n";
                    ++fail;
                }
                else {
                    cout << "Nhap lai mat khau: ";
                    confirm = inputPasswordHidden();
                    if (confirm != password) {
                        cout << "Mat khau khong trung khop.\n";
                        ++fail;
                    }
                    else {
                        user.isPasswordAutoGenerated = false;
                        break;
                    }
                }
            }
        }
        else {
            password = generateRandomPassword(user.username);
            cout << "Mat khau tu sinh: " << password << "\n";
            user.isPasswordAutoGenerated = true;
            break;
        }
        if (fail >= 3) {
            string opt;
            cout << "Nhap mat khau sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            getline(cin, opt);
            if (opt != "Y" && opt != "y") { cout << "Huy tac vu tao tai khoan.\n"; return; }
            fail = 0;
        }
    }

    user.hashedPassword = Sha256(password);
    user.isAdmin = false;
    user.isLocked = false;
    // Xác thực OTP trước khi lưu tài khoản
    cout << "Nhap ma OTP de xac nhan tao tai khoan ho nguoi dung...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Tao tai khoan that bai do xac thuc OTP!\n";
        return;
    }
    if (fileManager.saveUser(user)) {
        Wallet w(user.username + "_wallet", user.username, 0);
        fileManager.saveWallet(w);
        cout << "Tao tai khoan thanh cong!\n";
    }
    else cout << "Loi khi luu tai khoan!\n";
}

// --- Hàm sinh mật khẩu tự động ---
std::string SystemManager::generateRandomPassword(const std::string& username) {
    static const char specialChars[] = "!@#$%^&*";
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> specialDist(0, sizeof(specialChars) - 2);
    std::uniform_int_distribution<> digitDist(0, 9);

    std::string password = username;
    password += specialChars[specialDist(rng)];
    for (int i = 0; i < 3; ++i) {
        password += std::to_string(digitDist(rng));
    }
    return password;
}
// --- Đăng nhập ---
void SystemManager::login() {
    clearScreen();
    cout << "--- DANG NHAP ---\n";
    string username, password;

    // Nhập username đúng quy tắc (cho phép quay lại, bắt nhập lại nếu không tồn tại)
    bool ok = safeInput(
        "Username: ",
        [&](const string& s) {
            if (s.empty() || s.length() > 50) return false;
            UserAccount tmp;
            return fileManager.loadUser(s, tmp);
        },
        username,
        "Username khong ton tai, khong duoc de trong hoac qua dai!"
    );
    if (!ok) return;

    UserAccount user;
    fileManager.loadUser(username, user);

    int loginAttempts = 0;
    while (true) {
        cout << "Mat khau (hoac nhap 'Q' de quen mat khau): ";
        password = inputPasswordHidden();

        if (password == "Q" || password == "q") {
            forgotPassword(username);
            return;
        }

        if (user.hashedPassword == Sha256(password)) break;
        else {
            cout << "Mat khau sai!\n";
            ++loginAttempts;
            if (loginAttempts >= 3) {
                string ans;
                cout << "Ban co muon quen mat khau? (Y/N): ";
                getline(cin, ans);
                if (ans == "Y" || ans == "y") {
                    forgotPassword(username);
                    return;
                }
                else return;
            }
        }
    }

    if (user.isLocked) {
        cout << "Tai khoan bi khoa! Lien he admin.\n";
        return;
    }

    currentUser = new UserAccount(user);
    Wallet w;
    fileManager.loadWallet(currentUser->username, w);
    currentWallet = new Wallet(w);

    cout << "Dang nhap thanh cong!\n";
    if (currentUser->isPasswordAutoGenerated) {
        cout << "Ban dang su dung mat khau tu sinh, vui long doi mat khau moi!\n";
        changePassword();
    }
    if (currentUser->isAdmin) adminMenu();
    else userMenu();

    delete currentUser;
    delete currentWallet;
    currentUser = nullptr;
    currentWallet = nullptr;
}

// --- Đổi mật khẩu ---
void SystemManager::changePassword() {
    clearScreen();
    cout << "--- DOI MAT KHAU ---\n";
    if (!currentUser) {
        cout << "Ban chua dang nhap!\n";
        return;
    }
    string oldPass, newPass, confirm;
    int failOld = 0;
    // Nhập mật khẩu cũ, cho phép B/b để out, tối đa 3 lần
    while (true) {
        cout << "Nhap mat khau cu (B/b de quay lai): ";
        oldPass = inputPasswordHidden();

        if (oldPass == "B" || oldPass == "b") return;

        if (currentUser->hashedPassword != Sha256(oldPass)) {
            cout << "Mat khau cu khong dung!\n";
            failOld++;
            if (failOld >= 3) {
                string opt;
                cout << "Ban nhap sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
                getline(cin, opt);
                if (opt == "Y" || opt == "y") return;
                failOld = 0;
            }
        }
        else {
            break; // Đúng thì ra khỏi vòng lặp nhập mật khẩu cũ
        }
    }

    // Nhập mật khẩu mới
    int failNew = 0;
    while (true) {
        cout << "Nhap mat khau moi (B/b de quay lai): ";
        newPass = inputPasswordHidden();

        if (newPass == "B" || newPass == "b") return;

        if (newPass.length() > 50) {
            cout << "Mat khau khong duoc vuot qua 50 ky tu!\n";
            ++failNew;
        }
        else {
            bool hasLower = false, hasDigit = false, hasSpecial = false;
            for (char ch : newPass) {
                if (islower(ch)) hasLower = true;
                else if (isdigit(ch)) hasDigit = true;
                else if (!isalnum(ch)) hasSpecial = true;
            }
            if (!(hasLower && hasDigit && hasSpecial)) {
                cout << "Mat khau phai co it nhat 1 chu thuong, 1 chu so va 1 ky tu dac biet.\n";
                ++failNew;
            }
            else {
                cout << "Xac nhan mat khau moi (B/b de quay lai): ";
                getline(cin, confirm);
                if (confirm == "B" || confirm == "b") return;
                if (confirm != newPass) {
                    cout << "Mat khau moi khong trung khop.\n";
                    ++failNew;
                }
                else break;
            }
        }
        if (failNew >= 3) {
            string opt;
            cout << "Nhap mat khau moi sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            getline(cin, opt);
            if (opt == "Y" || opt == "y") return;
            failNew = 0;
        }
    }

    // Xác thực OTP
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    currentUser->hashedPassword = Sha256(newPass);
    currentUser->isPasswordAutoGenerated = false;
    if (fileManager.saveUser(*currentUser)) {
        cout << "Doi mat khau thanh cong!\n";
    }
    else {
        cout << "Loi khi luu mat khau moi!\n";
    }
}
// --- Quên mật khẩu (menu nhập username, hoặc khi đã có username) ---
void SystemManager::forgotPassword() {
    clearScreen();
    cout << "--- QUEN MAT KHAU ---\n";
    string username;
    bool ok = safeInput(
        "Nhap username: ",
        [](const string& s) { return !s.empty() && s.length() <= 50; },
        username,
        "Username khong duoc de trong va toi da 50 ky tu!"
    );
    if (!ok) return;
    forgotPassword(username);
}

void SystemManager::forgotPassword(const string& username) {
    clearScreen();
    cout << "--- QUEN MAT KHAU ---\n";
    UserAccount user;
    if (!fileManager.loadUser(username, user)) {
        cout << "Username khong ton tai!\n";
        return;
    }

    string phone;
    bool ok = safeInput(
        "Nhap so dien thoai da dang ky: ",
        [&](const string& s) { return s == user.phone; },
        phone,
        "So dien thoai khong dung!"
    );
    if (!ok) return;

    // Xác thực OTP
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    string newPass, confirm;
    int fail = 0;
    while (true) {
        cout << "Nhap mat khau moi (bo trong de dung mat khau tu sinh): ";
        getline(cin, newPass);

        if (newPass.empty()) {
            newPass = generateRandomPassword(user.username);
            cout << "Mat khau tu sinh: " << newPass << "\n";
            user.isPasswordAutoGenerated = true;
            break;
        }
        if (newPass.length() > 50) {
            cout << "Mat khau khong duoc vuot qua 50 ky tu!\n";
            ++fail;
        }
        else {
            bool hasLower = false, hasDigit = false, hasSpecial = false;
            for (char ch : newPass) {
                if (islower(ch)) hasLower = true;
                else if (isdigit(ch)) hasDigit = true;
                else if (!isalnum(ch)) hasSpecial = true;
            }
            if (!(hasLower && hasDigit && hasSpecial)) {
                cout << "Mat khau phai co it nhat 1 chu thuong, 1 chu so va 1 ky tu dac biet.\n";
                ++fail;
            }
            else {
                cout << "Nhap lai mat khau moi: ";
                getline(cin, confirm);
                if (confirm != newPass) {
                    cout << "Mat khau moi khong trung khop.\n";
                    ++fail;
                }
                else {
                    user.isPasswordAutoGenerated = false;
                    break;
                }
            }
        }
        if (fail >= 3) {
            string opt;
            cout << "Nhap mat khau moi sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            getline(cin, opt);
            if (opt != "Y" && opt != "y") return;
            fail = 0;
        }
    }
    user.hashedPassword = Sha256(newPass);

    if (fileManager.saveUser(user)) {
        cout << "Dat lai mat khau thanh cong! Vui long dang nhap lai.\n";
    }
    else {
        cout << "Loi khi dat lai mat khau!\n";
    }
}

// --- Đăng xuất ---
void SystemManager::logout() {
    cout << "Dang xuat thanh cong!\n";
    // Đã xóa user hiện tại ở login, ở đây chỉ thông báo
}

// --- Cập nhật thông tin cá nhân ---
void SystemManager::updatePersonalInfo() {
    clearScreen();
    cout << "--- CAP NHAT THONG TIN CA NHAN ---\n";
    if (!currentUser) {
        cout << "Ban chua dang nhap!\n";
        return;
    }

    string newName, newEmail, newPhone;
    bool updated = false;

    bool ok = safeInput(
        "Ho ten moi (bo trong neu khong doi): ",
        [](const string& s) { return s.length() <= 50; },
        newName,
        "Ho ten khong duoc vuot qua 50 ky tu!"
    );
    if (!ok) return;

    ok = safeInput(
        "Email moi (bo trong neu khong doi): ",
        validateEmailInput,
        newEmail,
        "Email khong dung dinh dang hoac vuot qua 50 ky tu!"
    );
    if (!ok) return;

    ok = safeInput(
        "So dien thoai moi (bo trong neu khong doi): ",
        [](const string& s) {
            if (s.empty()) return true;
            if (s.length() < 9 || s.length() > 12) return false;
            for (char c : s) if (!isdigit(c)) return false;
            return true;
        },
        newPhone,
        "So dien thoai chi co do dai tu 9 den 12 ky tu!"
    );
    if (!ok) return;

    if (newName.empty() && newEmail.empty() && newPhone.empty()) {
        cout << "Khong co gi de cap nhat!\n";
        return;
    }

    // Xác thực OTP
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    if (!newName.empty()) { currentUser->fullName = newName; updated = true; }
    if (!newEmail.empty()) { currentUser->email = newEmail; updated = true; }
    if (!newPhone.empty()) { currentUser->phone = newPhone; updated = true; }

    if (updated && fileManager.saveUser(*currentUser)) {
        cout << "Cap nhat thong tin thanh cong!\n";
    }
    else {
        cout << "Loi khi luu thong tin!\n";
    }
}

// --- Menu người dùng ---
void SystemManager::userMenu() {
    while (true) {
        clearScreen();
        cout << "-----------------------------------------------\n";
        cout << "                 MENU NGUOI DUNG              \n";
        cout << "-----------------------------------------------\n";
        cout << "1. Xem thong tin               2. Doi mat khau            \n";
        cout << "3. Chuyen diem                 4. Cap nhat thong tin      \n";
        cout << "5. Lich su giao dich           6. Dang xuat          \n";
        cout << "-----------------------------------------------\n";

        string choice;
        bool ok = safeInput("Chon chuc nang: ", [](const string& s) {
            return s == "1" || s == "2" || s == "3" || s == "4" || s == "5" || s == "6";
            }, choice, "Lua chon khong hop le.");

        if (!ok) return;
        if (choice == "1") currentUser->displayInfo();
        else if (choice == "2") changePassword();
        else if (choice == "3") transferPoints();
        else if (choice == "4") updatePersonalInfo();
        else if (choice == "5") viewTransactionLog();
        else if (choice == "6") break;
    }
}

// --- Menu admin ---
void SystemManager::adminMenu() {
    while (true) {
        clearScreen();
        cout << "-----------------------------------------------\n";
        cout << "                 MENU ADMIN                   \n";
        cout << "-----------------------------------------------\n";
        cout << "1. Xem danh sach   2. Dang ky ho          3. Khoa user\n";
        cout << "4. Mo khoa         5. Doi mat khau ho         \n";
        cout << "6. Nap diem        7. Lich su giao dich       \n";
        cout << "8. Backup          9. Restore             10. Dang xuat\n";
        cout << "-----------------------------------------------\n";

        string choice;
        bool ok = safeInput("Chon chuc nang: ", [](const string& s) {
            return (s >= "1" && s <= "9" && s.length() == 1) || s == "10";
            }, choice, "Lua chon khong hop le.");

        if (!ok) return;
        if (choice == "1") viewUserList();
        else if (choice == "2") registerUserAsAdmin();
        else if (choice == "3") lockUser();
        else if (choice == "4") unlockUser();
        else if (choice == "5") resetUserPasswordByAdmin();
        else if (choice == "6") topupUserWallet();
        else if (choice == "7") viewTransactionLog();
        else if (choice == "8") backup();
        else if (choice == "9") restore();
        else if (choice == "10") break;
    }
}
// --- Chuyển điểm giữa các ví ---
void SystemManager::transferPoints() {
    clearScreen();
    cout << "--- CHUYEN DIEM ---\n";

    string toUsername;
    bool ok = safeInput(
        "Nhap username nguoi nhan: ",
        [&](const string& s) {
            if (s.empty()) return false;
            if (s == currentUser->username) return false;
            UserAccount u;
            return fileManager.loadUser(s, u) && !u.isLocked;
        },
        toUsername,
        "Nguoi nhan khong ton tai, bi khoa hoac ban dang tu chuyen cho chinh minh!"
    );
    if (!ok) { cout << "Huy tac vu chuyen diem.\n"; return; }

    // NGĂN chuyển cho master
    if (toUsername == "master") {
        cout << "Khong the chuyen diem cho vi tong!\n";
        return;
    }
    string inputAmount;
    ok = safeInput(
        "Nhap so diem can chuyen: ",
        [](const string& s) {
            if (s.empty()) return false;
            for (char c : s) if (!isdigit(c)) return false;
            long long a = stoll(s);
            return a > 0;
        },
        inputAmount,
        "So diem chuyen phai la so nguyen duong!"
    );
    if (!ok) { cout << "Huy tac vu chuyen diem.\n"; return; }
    long long amount = stoll(inputAmount);

    Wallet fromWallet, toWallet;
    if (!fileManager.loadWallet(currentUser->username, fromWallet) ||
        !fileManager.loadWallet(toUsername, toWallet)) {
        cout << "Loi doc du lieu vi!\n";
        return;
    }
    if (fromWallet.balance < amount) {
        cout << "So du khong du! Giao dich bi huy.\n";
        // Lưu giao dịch thất bại do không đủ điểm
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        char buf[100];
        tm timeInfo;
        localtime_s(&timeInfo, &now_c);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
        Transaction tx("TX-" + currentUser->username + "-" + toUsername + "-" + to_string(now_c),
            currentUser->username, toUsername, amount, buf, "Failed", "Khong du diem");
        fileManager.saveTransaction(tx);
        return;
    }
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    fromWallet.balance -= amount;
    toWallet.balance += amount;
    bool ok1 = fileManager.saveWallet(fromWallet);
    bool ok2 = fileManager.saveWallet(toWallet);

    // Tạo timestamp
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    char buf[100];
    tm timeInfo;
    localtime_s(&timeInfo, &now_c);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
    string timeStamp = buf;

    Transaction tx(
        "TX-" + currentUser->username + "-" + toUsername + "-" + to_string(now_c),
        currentUser->username,
        toUsername,
        amount,
        timeStamp,
        (ok1 && ok2) ? "Success" : "Failed",
        (ok1 && ok2) ? "Chuyen diem thanh cong" : "Loi khi ghi du lieu vi"
    );
    fileManager.saveTransaction(tx);

    if (ok1 && ok2) {
        cout << "Chuyen diem thanh cong!\n";
        cout << "So du hien tai: " << fromWallet.balance << " diem\n";
    }
    else {
        // Phục hồi lại số dư nếu có lỗi
        fromWallet.balance += amount;
        toWallet.balance -= amount;
        fileManager.saveWallet(fromWallet);
        fileManager.saveWallet(toWallet);
        cout << "Co loi khi luu giao dich. So du da duoc phuc hoi.\n";
    }
}

// --- Admin nạp điểm cho user ---
void SystemManager::topupUserWallet() {
    clearScreen();
    cout << "--- ADMIN NAP DIEM ---\n";
    string username;
    bool ok = safeInput(
        "Nhap username can nap diem (hoac 'master' de nap vao vi tong): ",
        [&](const string& s) {
            if (s.empty()) return false;
            if (s == "master") return true; // Cho phép nạp vào ví tổng
            UserAccount u;
            return fileManager.loadUser(s, u) && !u.isLocked && !u.isAdmin;
        },
        username,
        "Nguoi dung khong ton tai/bi khoa/khong hop le! (Admin chi nap cho user, hoac 'master')"
    );
    if (!ok) { cout << "Huy tac vu nap diem.\n"; return; }

    string inputAmount;
    ok = safeInput(
        username == "master" ? "Nhap so diem muon nap vao vi tong: " : "Nhap so diem muon chuyen tu vi tong cho user: ",
        [](const string& s) {
            if (s.empty()) return false;
            for (char c : s) if (!isdigit(c)) return false;
            long long a = stoll(s);
            return a > 0;
        },
        inputAmount,
        "So diem phai la so nguyen duong!"
    );
    if (!ok) { cout << "Huy tac vu nap diem.\n"; return; }
    long long amount = stoll(inputAmount);

    // Xác thực OTP
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        // Ghi log thất bại
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        char buf[100];
        tm timeInfo;
        localtime_s(&timeInfo, &now_c);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
        Transaction tx(
            username == "master" ? "TX-ADMIN-TOPUP-MASTER-" + to_string(now_c)
            : "TX-MASTER-" + username + "-" + to_string(now_c),
            username == "master" ? "ADMIN" : "master",
            username,
            amount,
            buf,
            "Failed",
            "OTP that bai"
        );
        fileManager.saveTransaction(tx);
        return;
    }

    if (username == "master") {
        // Nạp điểm vào ví tổng
        Wallet masterWallet;
        fileManager.loadWallet("master", masterWallet); // hoặc "master_wallet" nếu bạn dùng đúng ID
        masterWallet.balance += amount;
        bool ok2 = fileManager.saveWallet(masterWallet);

        // Ghi log
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        char buf[100];
        tm timeInfo;
        localtime_s(&timeInfo, &now_c);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);

        Transaction tx(
            "TX-ADMIN-TOPUP-MASTER-" + to_string(now_c),
            "ADMIN", "master", amount, buf,
            ok2 ? "Success" : "Failed",
            ok2 ? "Nap diem vao vi tong" : "Loi khi ghi du lieu vi tong"
        );
        fileManager.saveTransaction(tx);

        cout << (ok2 ? "Nap diem vao vi tong thanh cong!\n" : "Loi khi nap diem vao vi tong!\n");
        return;
    }
    else {
        // Chuyển điểm từ ví tổng cho user
        Wallet masterWallet, userWallet;
        fileManager.loadWallet("master", masterWallet); // hoặc "master_wallet"
        fileManager.loadWallet(username, userWallet);

        if (masterWallet.balance < amount) {
            cout << "So du vi tong khong du! Giao dich bi huy.\n";
            // Ghi log thất bại
            auto now = chrono::system_clock::now();
            time_t now_c = chrono::system_clock::to_time_t(now);
            char buf[100];
            tm timeInfo;
            localtime_s(&timeInfo, &now_c);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);

            Transaction tx(
                "TX-MASTER-" + username + "-" + to_string(now_c),
                "master", username, amount, buf, "Failed", "Khong du diem trong vi tong"
            );
            fileManager.saveTransaction(tx);
            return;
        }
        // Trừ điểm ví tổng, cộng cho user
        masterWallet.balance -= amount;
        userWallet.balance += amount;
        bool ok1 = fileManager.saveWallet(masterWallet);
        bool ok2 = fileManager.saveWallet(userWallet);

        // Ghi log giao dịch
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        char buf[100];
        tm timeInfo;
        localtime_s(&timeInfo, &now_c);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);

        Transaction tx(
            "TX-MASTER-" + username + "-" + to_string(now_c),
            "master", username, amount, buf,
            (ok1 && ok2) ? "Success" : "Failed",
            (ok1 && ok2) ? "Chuyen diem tu vi tong" : "Loi khi ghi du lieu vi"
        );
        fileManager.saveTransaction(tx);

        cout << ((ok1 && ok2) ? "Chuyen diem thanh cong!\n" : "Loi khi chuyen diem!\n");
        return;
    }
}


// --- Admin khóa tài khoản user ---
void SystemManager::lockUser() {
    clearScreen();
    cout << "--- ADMIN KHOA TAI KHOAN USER ---\n";

    string username;
    bool ok = safeInput(
        "Nhap username can khoa: ",
        [&](const string& s) {
            if (s.empty()) return false;
            UserAccount u;
            if (!fileManager.loadUser(s, u)) return false;
            return !u.isAdmin && !u.isLocked;
        },
        username,
        "User khong ton tai, da khoa hoac la admin!"
    );
    if (!ok) { cout << "Huy tac vu khoa user.\n"; return; }

    // Xác thực OTP
    cout << "Nhap OTP de xac thuc khoa tai khoan...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    UserAccount user;
    fileManager.loadUser(username, user);
    user.isLocked = true;
    if (fileManager.saveUser(user)) {
        cout << "Khoa tai khoan thanh cong!\n";
    }
    else {
        cout << "Loi khi khoa tai khoan!\n";
    }
}


// --- Admin mở khóa tài khoản user ---
void SystemManager::unlockUser() {
    clearScreen();
    cout << "--- ADMIN MO KHOA TAI KHOAN USER ---\n";

    string username;
    bool ok = safeInput(
        "Nhap username can mo khoa: ",
        [&](const string& s) {
            if (s.empty()) return false;
            UserAccount u;
            if (!fileManager.loadUser(s, u)) return false;
            return !u.isAdmin && u.isLocked;
        },
        username,
        "User khong bi khoa hoac khong ton tai!"
    );
    if (!ok) { cout << "Huy tac vu mo khoa user.\n"; return; }

    // Xác thực OTP
    cout << "Nhap OTP de xac thuc mo khoa tai khoan...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Da huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    UserAccount user;
    fileManager.loadUser(username, user);
    user.isLocked = false;
    if (fileManager.saveUser(user)) {
        cout << "Mo khoa tai khoan thanh cong!\n";
    }
    else {
        cout << "Loi khi mo khoa tai khoan!\n";
    }
}

// --- Admin reset mật khẩu cho user ---
void SystemManager::resetUserPasswordByAdmin() {
    clearScreen();
    cout << "--- ADMIN DOI MAT KHAU HO NGUOI DUNG ---\n";

    string username;
    bool ok = safeInput(
        "Nhap username can doi mat khau: ",
        [&](const string& s) {
            if (s.empty()) return false;
            UserAccount u;
            return fileManager.loadUser(s, u) && !u.isLocked && !u.isAdmin;
        },
        username,
        "User khong ton tai, da khoa hoac la admin!"
    );
    if (!ok) {
        cout << "Huy tac vu doi mat khau.\n";
        return;
    }

    UserAccount user;
    fileManager.loadUser(username, user);

    cout << "Dang gui OTP den nguoi dung de xac nhan yeu cau doi mat khau...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    string newPass, confirm;
    int fail = 0;
    while (true) {
        cout << "Nhap mat khau moi (bo trong de dung mat khau tu sinh): ";
        newPass = inputPasswordHidden();

        if (!newPass.empty()) {
            if (newPass.length() > 50) {
                cout << "Mat khau khong duoc vuot qua 50 ky tu!\n";
                ++fail;
            }
            else {
                bool hasLower = false, hasDigit = false, hasSpecial = false;
                for (char ch : newPass) {
                    if (islower(ch)) hasLower = true;
                    else if (isdigit(ch)) hasDigit = true;
                    else if (!isalnum(ch)) hasSpecial = true;
                }
                if (!(hasLower && hasDigit && hasSpecial)) {
                    cout << "Mat khau phai co it nhat 1 chu thuong, 1 chu so va 1 ky tu dac biet.\n";
                    ++fail;
                }
                else {
                    cout << "Nhap lai mat khau moi: ";
                    confirm = inputPasswordHidden();
                    if (confirm != newPass) {
                        cout << "Mat khau moi khong trung khop.\n";
                        ++fail;
                    }
                    else {
                        user.isPasswordAutoGenerated = false;
                        break;
                    }
                }
            }
        }
        else {
            newPass = generateRandomPassword(user.username);
            cout << "Mat khau tu sinh: " << newPass << "\n";
            user.isPasswordAutoGenerated = true;
            break;
        }
        if (fail >= 3) {
            string opt;
            cout << "Nhap mat khau moi sai qua 3 lan. Ban co muon quay lai? (Y/N): ";
            getline(cin, opt);
            if (opt != "Y" && opt != "y") return;
            fail = 0;
        }
    }
    user.hashedPassword = Sha256(newPass);

    if (fileManager.saveUser(user)) {
        cout << "Doi mat khau thanh cong!\n";
    }
    else {
        cout << "Loi khi luu mat khau moi!\n";
    }
}

// --- Backup dữ liệu, có xác thực OTP ---
void SystemManager::backup() {
    clearScreen();
    cout << "--- BACKUP DU LIEU ---\n";

    cout << "Nhap OTP de xac thuc backup...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    bool ok = fileManager.backupData();
    if (ok) cout << "Backup du lieu thanh cong!\n";
    else cout << "Loi khi backup du lieu!\n";
}

// --- Restore dữ liệu từ backup, có xác thực OTP ---
void SystemManager::restore() {
    clearScreen();
    cout << "--- PHUC HOI DU LIEU TU BACKUP ---\n";

    cout << "Nhap OTP de xac thuc restore...\n";
    if (!otpManager.requestOTPWithTimeout()) {
        cout << "Huy tac vu vi xac thuc OTP that bai hoac het han.\n";
        return;
    }

    string xacnhan;
    bool ok = safeInput(
        "Canh bao: Phuc hoi se ghi de toan bo du lieu hien tai! Ban co chac muon tiep tuc? (Y/N): ",
        [](const string& s) { return s == "Y" || s == "y" || s == "N" || s == "n"; },
        xacnhan,
        "Chi duoc nhap Y/y hoac N/n."
    );
    if (!ok || xacnhan == "N" || xacnhan == "n") {
        cout << "Huy thao tac phuc hoi.\n";
        return;
    }
    bool result = fileManager.restoreBackup();
    if (result) cout << "Phuc hoi du lieu thanh cong!\n";
    else cout << "Loi khi phuc hoi du lieu!\n";
}

// --- Xem lịch sử giao dịch ---
void SystemManager::viewTransactionLog() {
    clearScreen();
    cout << "--- LICH SU GIAO DICH ---\n";

    auto txs = fileManager.loadAllTransactions();
    if (txs.empty()) {
        cout << "Khong co giao dich nao!\n";
    }
    else {
        for (const auto& t : txs) {
            if (currentUser->isAdmin ||
                t.fromUser == currentUser->username ||
                t.toUser == currentUser->username) {
                t.display();
                cout << "-------------------------\n";
            }
        }
    }

    // Thêm pause, cho phép nhấn B/b để quay lại
    cout << "Nhan B/b de quay lai menu: ";
    string tmp;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, tmp);
    // Dù nhập gì cũng sẽ quay lại menu
}


// --- Xem danh sách người dùng (admin) ---
void SystemManager::viewUserList() {
    clearScreen();
    cout << "--- DANH SACH NGUOI DUNG ---\n";
    // --- HIEN THI VÍ TỔNG (MASTER WALLET) ---
    Wallet masterWallet;
    if (fileManager.loadWallet("master", masterWallet)) {
        cout << "==============================\n";
        cout << "== Vi Tong He Thong (MASTER) ==\n";
        cout << "Owner    : " << masterWallet.owner << " (vi tong)\n";
        cout << "WalletID : " << masterWallet.walletID << "\n";
        cout << "So Du    : " << masterWallet.balance << " diem\n";
        cout << "==============================\n";
    }
    auto users = fileManager.loadAllUsers();
    if (users.empty()) {
        cout << "Khong co nguoi dung nao!\n";
    }
    else {
        for (const auto& u : users) {
            Wallet w;
            fileManager.loadWallet(u.username, w);
            cout << "==============================\n";
            cout << "Username: " << u.username << "\n";
            cout << "Ho ten : " << u.fullName << "\n";
            cout << "Trang thai: " << (u.isLocked ? "Bi khoa" : "Binh thuong") << "\n";
            cout << "Vai tro   : " << (u.isAdmin ? "Admin" : "User") << "\n";
            cout << "Vi: " << w.walletID << " - So du: " << w.balance << "\n";
            cout << "==============================\n";
        }
    }
    // Chờ 1 lần nhập để out về menu
    cout << "Nhan B/b de quay lai menu: ";
    string tmp;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, tmp);
    // Dù nhập gì cũng sẽ out
}
// Hàm này sẽ kiểm tra và tạo ví tổng nếu chưa có
void FileManager::initMasterWalletIfNotExists() {
    createFolderIfNotExists("Data/User");
    string filename = "Data/User/master_wallet.dat";
    if (!std::filesystem::exists(filename)) {
        Wallet masterWallet("master_wallet", "master", 0);
        saveWallet(masterWallet);
    }
}
