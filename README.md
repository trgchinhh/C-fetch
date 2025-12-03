<h3 align="center"><img alt="logo-wide" src="C-fetch.png" width="150px"></h3>

## Giới thiệu 
- C-fetch là 1 công cụ tương tự neofetch hay winfetch, dùng để truy xuất thông tin hệ thống, cấu hình máy và hiển thị 1 cách trực quan.
- Công cụ này viết chủ yếu bằng ngôn ngữ C++, tập trung vào năng suất. Hiện tại C-fetch chỉ hỗ trợ trên hệ điều hành Windows.

## Tính năng 
- Hiển thị thông tin hệ điều hành, Cpu, Ram, kernel,...
- Lấy thông tin hệ thống thông qua Windows API.
- Hiển thị đẹp bằng màu sắc ANSI trên CMD/Powershell.
- Kiểu trình bày gọn, giống phong cách của neofetch / winfetch.

## Cài đặt
### Cách 1 
- B1: Tải file `C-fetch.exe` về máy
- B2: Chọn đường dẫn đến file.exe và bỏ vào biến môi trường
  (-) Cách mở biến môi trường 
  + Start -> Gõ env chọn Edit the system environment variables -> Enviromemt Variables -> Path -> New
  + Paste đường dãn đến file C-fetch.exe vào ô
  + Nhấn ok cho đến khi thoát hết.
- Mở CMD hoặc Powershell gõ `C-fetch.exe` nó sẽ hiện chương trình.
### Cách 2 
(*) Trường hợp chạy bằng GNU/CLANG
- Nếu chạy file.cpp
- Mở CMD/Powershell nhập lệnh biên dịch 
```
g++ c-fetch.cpp -o c-fetch -lwbemuuid -lole32 -loleaut32 -luuid -liphlpapi -lws2_32 -w
```
(*) Trường hợp chạy bằng MSVC
- Cần link các thư viện sau trước khi chạy
```
wbemuuid, ole32, oleaut32, uuid, iphlpapi, ws2_32
```

## Chương trình khi bạn chạy thành công 
<img width="1917" height="1017" alt="image" src="https://github.com/user-attachments/assets/3f4bb54f-f654-42cf-9c67-e878ffb57ebd" />

## Lưu ý
- Phiên bản: `0.1` (bản đầu tiên)
- Một số thông tin có thể chưa đầy đủ hoặc chưa chính xác.
- Tôi sẽ cố gắng ra bản nâng cấp sớm nhất.   

## Ghi chú 
- Đây là phiên bản đầu tiên có thể bị sai sót.
- Rất mong nhiều sự đóng góp để phát triển.
