# 📄 README - Reward Wallet System

***

## 🎉 Giới thiệu dự án
**Hệ thống đăng nhập, đăng ký tài khoản và quản lý ví điểm thưởng** (Reward Wallet System) là ứng dụng quản lý tài khoản, ví điểm, giao dịch và quyền quản trị viên được viết bằng C++. Hệ thống hỗ trợ bảo mật mật khẩu bằng SHA-256, xác thực OTP và chức năng sao lưu/phục hồi dữ liệu.

***

## 💼 Cấu trúc thư mục, mã nguồn và phân công

### 📂Cấu trúc thư mục, mã nguồn

| Chức năng                  | Nội dung chức năng                                                                                                                                           | 
|----------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `FileManager.h`            | Khai báo các phương thức để quản lý việc đọc/ghi dữ liệu của người dùng, ví, và giao dịch, cũng như hỗ trợ sao lưu (backup) và phục hồi (restore) dữ liệu.   | 
| `OTPManager.h`             | Khai báo các phương thức để sinh, quản lý, và xác thực mã OTP (One-Time Password) dùng cho các thao tác nhạy cảm trong hệ thống                              | 
| `SystemManager.h`          | Khai báo các phương thức và thành phần để quản lý toàn bộ nghiệp vụ, giao diện, và luồng chương trình của hệ thống                                           | 
| `Transaction.h`            | Khai báo cấu trúc và các phương thức để quản lý thông tin giao dịch chuyển điểm giữa các ví                                                                  | 
| `UserAccount.h`            | Khai báo cấu trúc và các phương thức để quản lý thông tin tài khoản người dùng (bao gồm cả người dùng thông thường và admin)                                 | 
| `Wallet.h`                 | Khai báo cấu trúc và các phương thức để quản lý thông tin ví điểm thưởng của người dùng hoặc admin.                                                          | 
| `FileManager.cpp`          | Quản lý hoạt động nhập, xuất dữ liệu gồm người dùng, ví, giao dịch và sao lưu/phục hồi dữ liệu                                                               | 
| `Main.cpp`                 | Khởi tạo SystemManager và gọi MainMenu                                                                                                                       | 
| `OTPManager.cpp`           | Quản lý việc tạo, xác thực và xử lý mã OTP (One-Time Password) với thời gian hiệu lực giới hạn                                                               | 
| `SystemManager.cpp`        | Quản lý toàn bộ nghiệp vụ, giao diện, và luồng chương trình của hệ thống                                                                                     | 
| `Transaction.cpp`          | Định nghĩa và xử lý các thông tin liên quan đến một giao dịch trong hệ thống                                                                                 | 
| `UserAccount.cpp`          | Định nghĩa và quản lý thông tin tài khoản người dùng                                                                                                         | 
| `Wallet.cpp`               | Định nghĩa và quản lý thông tin ví người dùng                                                                                                                |
| `admin.dat`                | File lưu trữ thông tin admin                                                                                                                                 |
| `user.dat`                 | Dạng file lưu trữ thông tin người dùng                                                                                                                       |         
| `transaction_log.txt`      | Lưu trũ các bản ghi giao dịch trong hệ thống, mỗi dòng tương ứng với 1 giao dịch                                                                             | 

### 👥 Phân công

| Thành viên                 | Phân công công việc                                    | 
|----------------------------|--------------------------------------------------------|
| Nguyễn Minh Tuấn           | SystemManager.h, SystemManager.cpp, Main.cpp           | 
| Nguyễn Xuân Hải Anh        | OTPManager.h, OTPManager.cpp                           | 
| Phan Phúc Thịnh            | UserAccount.h, UserAccount.cpp, Wallet.h, Wallet.cpp   | 
| Nguyễn Xuân Thanh          | FileManager.h, FileManager.cpp                         | 
***

## 📋 Thành viên tham gia và phân công
| Thành viên                                | Phân chia công việc                                                                                                      | 
|-------------------------------------------|--------------------------------------------------------------------------------------------------------------------------|
|:busts_in_silhouette: Nguyễn Minh Tuấn     | Thiết kế, xây dựng giao diện menu và quản lý luồng nghiệp vụ hệ thống                                                    | 
|:busts_in_silhouette:Nguyễn Xuân Hải Anh   | Thiết lập, quản lý Git repository, cấu hình branch, merge code, xây dựng, quản lý tính bảo mật OTP và mã hóa mật khẩu    | 
|:busts_in_silhouette: Phan Phúc Thịnh      | Thiết kế, phát triển và quản lý dữ liệu người dùng, ví, giao dịch                                                        | 
|:busts_in_silhouette: Nguyễn Xuân Thanh    | Thiết kế chức năng sao lưu/khôi phục dữ liệu, thực hiện kiểm thử toàn diện, tối ưu hóa hiệu suất                         | 
```bash
https://github.com/NguyenXuanHaiAnhPTIT/RewardWalletSystem_PTITSemester3/commits/main/
```

***

## 🔎 Phân tích và đặc tả chức năng
### 📌 1. Đăng ký tài khoản
- Input: Username, số điện thoại, tên, email, mật khẩu hoặc tự sinh.
- Output: Thông báo thành công và tạo ví 0 điểm.
  
### 📌 2. Đăng nhập
- Input: Username, mật khẩu.
- Output: Thông báo thành công hoặc lỗi, hiển thị menu tương ứng.

### 📌 3. Quên mật khẩu
- Input: Username, số điện thoại, OTP, mật khẩu mới.
- Output: Thông báo đặt lại thành công.

### 📌 4. Đổi mật khẩu
- Input: Mật khẩu cũ, mật khẩu mới, OTP.
- Output: Thông báo đổi thành công.

### 📌 5. Chuyển điểm
- Input: Username người nhận, số điểm, OTP.
- Output: Thông báo thành công, cập nhật số dư.
### 📌 6. Nạp điểm (Admin)
- Input: Username, số điểm, OTP.
- Output: Thông báo thành công, cập nhật số dư.

### 📌 7. Đổi mật khẩu hộ (Admin)
- Input: Username, mật khẩu mới hoặc tự sinh.
- Output: Thông báo đổi thành công.

### 📌 8. Backup / Restore
- Input: Xác nhận thao tác.
- Output: Thông báo thành công hoặc lỗi.

### 📌 9. Xem lịch sử giao dịch
- Input: Không.
- Output: Danh sách chi tiết giao dịch.

##  📥 Cách tải, biên dịch và chạy chương trình
### 📌 1. Source code
Clone hoặc tải về từ Github:

```bash
https://github.com/NguyenXuanHaiAnhPTIT/RewardWalletSystem_PTITSemester3
```

### 📌 2. Biên dịch và chạy chương trình
####  🖥️ **Visual Studio 2022**

---

## 📜 Hướng dẫn sử dụng và các chức năng

 ### 🌐 Main Menu

| Chức năng                  | Nội dung chức năng                                      | 
|----------------------------|---------------------------------------------------------|
| Dang nhap                  | Đăng nhập (gồm admin và user)                           | 
| Dang ky                    | Đăng ký tài khoản mới                                   | 
| Thoat                      | Thoát chương trình                                      | 
### 🔑 User Menu
| Chức năng                  | Nội dung chức năng                                      | 
|----------------------------|---------------------------------------------------------|
| Thông tin ca nhan          | Xem thông tin cá nhân                                   | 
| Doi mat khau               | Đổi mật khẩu (Yêu cầu OTP)                              | 
| Cap nhat thong tin ca nhan |  Cập nhật, thay đổi thông tin cá nhân (Yêu cầu OTP)     | 
| Lich su giao dich          | Xem lịch sử giao dịch                                   | 
| Dang xuat                  | Đăng xuất                                               | 
 ### 🛠️ Admin Menu
| Chức năng                  | Nội dung chức năng                                      | 
|----------------------------|---------------------------------------------------------|
| Danh sach nguoi dung       | Danh sách người dùng                                    | 
| Dang ky                    | Đổi mật khẩu (Yêu cầu OTP)                              | 
| Khoa tai khoan             |  Cập nhật, thay đổi thông tin cá nhân (Yêu cầu OTP)     | 
| Mo khoa tai khoan          | Xem lịch sử giao dịch                                   | 
| Nap diem                   | Đăng xuất                                               | 
| Lich su giao dich          | Xem thông tin cá nhân                                   | 
| Sao luu du lieu            | Đổi mật khẩu (Yêu cầu OTP)                              | 
| Khoi phuc du lieu          |  Cập nhật, thay đổi thông tin cá nhân (Yêu cầu OTP)     | 
| Dang xuat                  | Đăng xuất                                               | 


## 📚 Tài liệu tham khảo
```bash
 https://nvlpubs.nist.gov
```
```bash
 https://en.cppreference.com
```
```bash
 https://chatgpt.com
```
```bash
 https://grok.com
```
## 📝 Chú thích mã nguồn
- Code đầy đủ comment từng hàm.
- Mô tả input/output chi tiết.
- Có xử lý lỗi, rollback, xác minh OTP.
