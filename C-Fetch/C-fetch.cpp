//   _____      ______   _       _     
//  / ____|    |  ____| | |     | |    
// | |   ______| |__ ___| |_ ___| |__  
// | |  |______|  __/ _ \ __/ __| '_ \   Tên: Core Fetch  
// | |____     | | |  __/ || (__| | | |  Phiên bản: 0.1
//  \_____|    |_|  \___|\__\___|_| |_|  Hoàn thành: 3/12/2025 
// 
// (!) CHƯƠNG TRÌNH: CORE FETCH
// (!) TÁC GIẢ: NGUYỄN TRƯỜNG CHINH (NTC++)
                                     
/************************************************************\
  * Chương trình lấy cảm hứng từ Winfetch (LPTSTR)
  * Tính năng: liệt kê cấu hình và thông tin máy, ...
  * Lưu ý: Đây là phiên bản đầu tiên nên còn nhiều thiếu sót
  * Chúng tôi sẽ cố gắng ra phiên bản mới sớm nhất  
\************************************************************/

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Lmcons.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <comdef.h>
#include <wbemidl.h>
#include <psapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")

#define C_FETCH signed main()

using namespace std;

// Màu chữ 
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33;1m"
#define CYAN    "\033[36m"
#define LIGHT_BLUE "\033[94m"

// Các khối màu
#define BG_BLACK         "\033[40m"
#define BG_RED           "\033[41m"
#define BG_GREEN         "\033[42m"
#define BG_YELLOW        "\033[43m"
#define BG_BLUE          "\033[44m"
#define BG_MAGENTA       "\033[45m"
#define BG_CYAN          "\033[46m"
#define BG_WHITE         "\033[47m"
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"

/****************************************************\
\****************************************************/

string cat_khoang_trang(const string &chuoi){
    size_t dau = chuoi.find_first_not_of(" \t\r\n");
    if(dau == string::npos) return "";
    size_t cuoi = chuoi.find_last_not_of(" \t\r\n");
    return chuoi.substr(dau, cuoi - dau + 1);
}

string chuyen_bytes_sang_gb(uint64_t bytes){
    double gb = (double)bytes / (1024.0*1024.0*1024.0);
    ostringstream os; os << fixed << setprecision(2) << gb;
    return os.str();
}

string doc_registry(HKEY root, const char* duong_dan, const char* ten_gia_tri){
    HKEY hKey;
    if(RegOpenKeyExA(root, duong_dan, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return "";
    char buf[1024]; DWORD bufSize = sizeof(buf);
    DWORD type = 0;
    if(RegQueryValueExA(hKey, ten_gia_tri, NULL, &type, (LPBYTE)buf, &bufSize) != ERROR_SUCCESS){
        RegCloseKey(hKey); return "";
    }
    RegCloseKey(hKey);
    if(type == REG_SZ || type == REG_EXPAND_SZ) return string(buf);
    return "";
}

struct ThongTinNguoiDung {
    string ten_nguoi_dung;
    string ten_may;
};

ThongTinNguoiDung lay_nguoi_dung(){
    ThongTinNguoiDung nguoi_dung;
    char ten[UNLEN+1]; DWORD len = UNLEN+1;
    nguoi_dung.ten_nguoi_dung = GetUserNameA(ten, &len) ? string(ten) : "Không có thông tin";
    char may[256]; DWORD hlen = sizeof(may);
    nguoi_dung.ten_may = GetComputerNameA(may, &hlen) ? string(may) : "Không có thông tin";
    return nguoi_dung;
}

string BSTR_sang_string(BSTR b) {
    if (!b) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, b, -1, nullptr, 0, nullptr, nullptr);
    string s(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, b, -1, s.data(), len, nullptr, nullptr);
    return s;
}

struct ThongTinHeThong {
    string he_dieu_hanh;
    string kernel;
    string thoi_gian_chay;
    string shell;
};

ThongTinHeThong lay_he_thong(){
    ThongTinHeThong he_thong;
    string ten_he_dieu_hanh = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
    string phien_ban = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "DisplayVersion");
    string so_build_str = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentBuild");
    string ubr = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "UBR");
    int so_build = so_build_str.empty() ? 0 : stoi(so_build_str);
    if(so_build >= 22000){
        if(ten_he_dieu_hanh.find("Pro") != string::npos) ten_he_dieu_hanh = "Windows 11 Pro";
        else if(ten_he_dieu_hanh.find("Home") != string::npos) ten_he_dieu_hanh = "Windows 11 Home";
        else ten_he_dieu_hanh = "Windows 11";
    } else if(so_build >= 10240) ten_he_dieu_hanh = "Windows 10";
    ostringstream os_str; os_str << ten_he_dieu_hanh;
    if(!phien_ban.empty()) os_str << " " << phien_ban;
    if(!so_build_str.empty()){ os_str << " (Build " << so_build; if(!ubr.empty()) os_str << "." << ubr; os_str << ")"; }
    he_thong.he_dieu_hanh = os_str.str();
    he_thong.kernel = "WIN32_NT " + (so_build_str.empty() ? "?" : so_build_str);
    ULONGLONG ms = GetTickCount64();
    unsigned long long tong_giay = ms / 1000ULL;
    unsigned long long ngay  = tong_giay / 86400ULL; tong_giay %= 86400ULL;
    unsigned long long gio   = tong_giay / 3600ULL; tong_giay %= 3600ULL;
    unsigned long long phut  = tong_giay / 60ULL;
    unsigned long long giay  = tong_giay % 60ULL;
    ostringstream thoi_gian;
    if(ngay) thoi_gian << ngay << " ngày" << ", ";
    if(gio) thoi_gian << gio << " giờ, ";
    thoi_gian << phut << " phút, " << giay << " giây";
    he_thong.thoi_gian_chay = thoi_gian.str();
    if(getenv("WT_SESSION")) he_thong.shell="Windows Terminal";
    else if(getenv("TERM_PROGRAM")) he_thong.shell = getenv("TERM_PROGRAM");
    else if(getenv("PSModulePath")) he_thong.shell = "PowerShell";
    else if(getenv("COMSPEC")){
        string com = getenv("COMSPEC");
        if(com.find("powershell")!=string::npos || com.find("pwsh")!=string::npos) he_thong.shell="PowerShell";
        else if(com.find("cmd.exe")!=string::npos) he_thong.shell="Command Prompt";
        else he_thong.shell=com;
    } else he_thong.shell="Không có thông tin";
    return he_thong;
}

string lay_ten_windows() {
    string ten_san_pham = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
    string chuoi_build = doc_registry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CurrentBuild");
    int so_build = chuoi_build.empty() ? 0 : stoi(chuoi_build);
    if(so_build >= 22000) return "Windows 11";
    else if(so_build >= 10240) return "Windows 10";
    else if(so_build >= 9600) return "Windows 8.1";
    else if(so_build >= 9200) return "Windows 8";
    else if(so_build >= 7601) return "Windows 7";
    else if(so_build >= 1381) return "Windows 2000"; 
    else return ten_san_pham.empty() ? "Windows" : ten_san_pham;
}

struct ThongTinCPU { 
    string ten_cpu; 
    int so_nhan; 
    double toc_do_ghz; 
};

ThongTinCPU lay_cpu(){
    ThongTinCPU cpu;
    cpu.ten_cpu = cat_khoang_trang(doc_registry(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0","ProcessorNameString"));
    if(cpu.ten_cpu.empty()) cpu.ten_cpu="Unknown CPU";
    SYSTEM_INFO si; GetSystemInfo(&si);
    cpu.so_nhan = si.dwNumberOfProcessors;
    DWORD mhz=0; DWORD sz=sizeof(mhz);
    HKEY hKey; if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",0,KEY_READ,&hKey)==ERROR_SUCCESS){
        RegQueryValueExA(hKey,"~MHz",NULL,NULL,(LPBYTE)&mhz,&sz);
        RegCloseKey(hKey);
    }
    cpu.toc_do_ghz = (double)mhz/1000.0;
    return cpu;
}

string lay_gpu(){
    DISPLAY_DEVICEA dd; ZeroMemory(&dd,sizeof(dd)); dd.cb=sizeof(dd);
    for(DWORD i=0; EnumDisplayDevicesA(NULL,i,&dd,0); ++i){
        if(dd.StateFlags & DISPLAY_DEVICE_ACTIVE){ string s = cat_khoang_trang(dd.DeviceString); if(!s.empty()) return s; }
        ZeroMemory(&dd,sizeof(dd)); dd.cb=sizeof(dd);
    }
    return "Không có thông tin";
}

struct ThongTinBoNho { 
    uint64_t ram_tong, ram_da_dung;
    uint64_t swap_tong, swap_da_dung;
};

ThongTinBoNho lay_ram(){
    ThongTinBoNho mem{};
    MEMORYSTATUSEX m; m.dwLength = sizeof(m);
    if(GlobalMemoryStatusEx(&m)){
        mem.ram_tong = m.ullTotalPhys;
        mem.ram_da_dung = m.ullTotalPhys - m.ullAvailPhys;
    }
    PERFORMANCE_INFORMATION pi; pi.cb = sizeof(pi);
    if(GetPerformanceInfo(&pi, sizeof(pi))){
        uint64_t page_size = pi.PageSize;
        uint64_t commit_limit = (uint64_t)pi.CommitLimit * page_size; 
        uint64_t commit_total = (uint64_t)pi.CommitTotal * page_size; 
        mem.swap_da_dung = commit_total > mem.ram_da_dung ? commit_total - mem.ram_da_dung : 0;
        mem.swap_tong = commit_limit > mem.ram_tong ? commit_limit - mem.ram_tong : 0;
    }
    return mem;
}

struct ThongTinOCung { 
    string ten_o; 
    uint64_t tong, da_dung; 
};

ThongTinOCung lay_o(const char* o){
    ThongTinOCung d; d.ten_o = o; ULARGE_INTEGER freeBytes;
    if(GetDiskFreeSpaceExA(o,NULL,(PULARGE_INTEGER)&d.tong,&freeBytes)) d.da_dung = d.tong - freeBytes.QuadPart;
    else d.tong=d.da_dung=0;
    return d;
}

string lay_ip(){
    WSADATA wsa; WSAStartup(MAKEWORD(2,2),&wsa);
    ULONG bufLen=0, flags=GAA_FLAG_INCLUDE_PREFIX; GetAdaptersAddresses(AF_UNSPEC,flags,NULL,NULL,&bufLen);
    vector<BYTE> buf(bufLen); IP_ADAPTER_ADDRESSES* addrs = (IP_ADAPTER_ADDRESSES*)buf.data();
    if(GetAdaptersAddresses(AF_UNSPEC,flags,NULL,addrs,&bufLen)!=NO_ERROR){ WSACleanup(); return "Unknown IP"; }
    for(IP_ADAPTER_ADDRESSES* a=addrs;a;a=a->Next){
        if(a->OperStatus!=IfOperStatusUp || a->IfType==IF_TYPE_SOFTWARE_LOOPBACK) continue;
        for(IP_ADAPTER_UNICAST_ADDRESS* ua=a->FirstUnicastAddress;ua;ua=ua->Next){
            SOCKADDR* addr=ua->Address.lpSockaddr;
            if(addr->sa_family==AF_INET){
                char ipbuf[INET_ADDRSTRLEN]={0};
                sockaddr_in* ipv4=(sockaddr_in*)addr;
                inet_ntop(AF_INET,&(ipv4->sin_addr),ipbuf,sizeof(ipbuf));
                string ip = ipbuf; if(ip!="127.0.0.1"){ WSACleanup(); return ip; }
            }
        }
    }
    WSACleanup(); return "Không có thông tin";
}

string lay_pin(){
    SYSTEM_POWER_STATUS sps;
    if(GetSystemPowerStatus(&sps)){
        ostringstream os;
        os<<(sps.BatteryLifePercent!=255 ? (int)sps.BatteryLifePercent : 0)<<"%";
        os<<(sps.ACLineStatus==1?" [Đang cắm sạc]":" [Không cắm sạc]");
        return os.str();
    }
    return "Không có thông tin";
}

string lay_ngon_ngu_may(){
    WCHAR name[LOCALE_NAME_MAX_LENGTH];
    if(GetUserDefaultLocaleName(name,LOCALE_NAME_MAX_LENGTH)){
        int len = WideCharToMultiByte(CP_UTF8,0,name,-1,NULL,0,NULL,NULL);
        if(len>0) {
            string out(len,0); WideCharToMultiByte(CP_UTF8,0,name,-1,&out[0],len,NULL,NULL); 
            if(!out.empty() && out.back()==0) out.pop_back(); 
            return out;
        } 
    }
    return "Không có thông tin";
}

string lay_display(){
    DEVMODEA dm; ZeroMemory(&dm,sizeof(dm)); dm.dmSize=sizeof(dm);
    if(EnumDisplaySettingsA(NULL,ENUM_CURRENT_SETTINGS,&dm)){
        ostringstream os; os<<dm.dmPelsWidth<<"x"<<dm.dmPelsHeight<<" @ "<<dm.dmDisplayFrequency<<" Hz"; 
        return os.str();
    }
    return "Không có thông tin";
}

void enable_ansi_colors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

string khoi_mau(string mau){
    return mau + string(3, ' ') + RESET;
}

vector<string> banner_windows_2000 = {
    "                                             ",
    "                                             ",
    "                         dBBBBBBBb           ",
    "                     dBBBBBBBBBBBBBBBb       ",
    "             000 BBBBBBBBBBBBBBBBBBBB        ",
    ":::::        000000 BBBBBdBBBBBBBBBbBBBBBBB  ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBfBBBBBBBBBBB`BBBB    ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBBBBBBBBBBBBBBBBB     ",
    "::::: ====== 000000 BBBBfBBBBBBBBBBB`BBBB    ",
    "::::: ====== 000000 BBBBBf         `BBBBBBBBB",
    "     ==  000000 B                     BBB    ",
    "                                             ",
    "                                             ",
    "                                             ",
};

vector<string> banner_windows_xp_to_7 = {
    "                                      ",
    "                                      ",
    "        ,.=:!!t3Z3z.,                 ",
    "       :tt:::tt333EE3                 ",
    "       Et:::ztt33EEE  @Ee.,      ..,  ",
    "      ;tt:::tt333EE7 ;EEEEEEttttt33#  ",
    "     :Et:::zt333EEQ. SEEEEEttttt33QL  ",
    "     it::::tt333EEF @EEEEEEttttt33F   ",
    "    ;3=*^``````'*4EEV :EEEEEEttttt33@.",
    "    ,.=::::it=., `` @EEEEEEtttz33QF   ",
    "   ;::::::::zt33)   '4EEEtttji3P*     ",
    "  :t::::::::tt33 :Z3z..  ```` ,..g.   ",
    "  i::::::::zt33F AEEEtttt::::ztF      ",
    " ;:::::::::t33V ;EEEttttt::::t3       ",
    " E::::::::zt33L @EEEtttt::::z3F       ",
    "{3=*^``````'*4E3) ;EEEtttt:::::tZ``   ",
    "            `` :EEEEtttt::::z7        ",
    "                'VEzjt:;;z>*``        ",
    "                                      ",
    "                                      ",
};

vector<string> banner_windows_8_to_10 = {
    "                                       ",
    "                    ....,,:;+ccllll    ",
    "      ...,,+:;  cllllllllllllllllll    ",
    ",cclllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "                                       ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "llllllllllllll  lllllllllllllllllll    ",
    "``'ccllllllllll  lllllllllllllllllll   ",
    "      ``' ll*::  :ccllllllllllllllll   ",
    "                       ````````''*::cll",
    "                                 ````  ",
    "                                       ",
};

vector<string> banner_windows_11 = {
    "                                    ",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "                                    ",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "/////////////////  /////////////////",
    "                                    ",
    "                                    ",
};

/****************************************************\
\****************************************************/

void chuong_trinh_chinh(){
    cout << boolalpha;

    ThongTinNguoiDung user = lay_nguoi_dung();
    ThongTinHeThong sys = lay_he_thong();
    ThongTinCPU cpu = lay_cpu();
    string gpu = lay_gpu();
    ThongTinBoNho mem = lay_ram();
    ThongTinOCung c = lay_o("C:\\"); 
    ThongTinOCung d = lay_o("D:\\");
    string ip = lay_ip();
    string pin = lay_pin();
    string language = lay_ngon_ngu_may();
    string disp = lay_display();

    int phan_tram_ram_da_dung = (int)((double)mem.ram_da_dung / mem.ram_tong * 100);
    int phan_tram_swap_da_dung = (int)((double)mem.swap_da_dung / mem.swap_tong * 100);
    int phan_tram_c_da_dung = (int)((double)c.da_dung / c.tong * 100);
    int phan_tram_d_da_dung = (int)((double)d.da_dung / d.tong * 100);

    vector<string> dong_thong_tin = {
        CYAN + user.ten_nguoi_dung + RESET + "@" + CYAN + user.ten_may + RESET,
        string((user.ten_nguoi_dung + "@" + user.ten_may).size(), '-'),
        string(YELLOW) + "◉" + RESET,
        string(YELLOW) + "├ Hệ điều hành: " + RESET + sys.he_dieu_hanh,
        string(YELLOW) + "├ Kernel: " + RESET + sys.kernel,
        string(YELLOW) + "├ Cpu hoạt động: " + RESET + sys.thoi_gian_chay,
        string(YELLOW) + "├ Shell: " + RESET + sys.shell,
        string(YELLOW) + "├ Màn hình: " + RESET + disp,
        string(YELLOW) + "├ CPU: " + RESET + cpu.ten_cpu + " (" + to_string(cpu.so_nhan) + " nhân)",
        string(YELLOW) + "├ GPU: " + RESET + gpu,
        string(YELLOW) + "├ RAM: " + RESET + chuyen_bytes_sang_gb(mem.ram_da_dung) + " / " + chuyen_bytes_sang_gb(mem.ram_tong) 
                       + " GB (" + GREEN + to_string(phan_tram_ram_da_dung) + "%" + RESET + ")",
        string(YELLOW) + "├ Swap: " + RESET + chuyen_bytes_sang_gb(mem.swap_da_dung) + " / " + chuyen_bytes_sang_gb(mem.swap_tong)
                       + " GB (" + GREEN + to_string(phan_tram_swap_da_dung) + "%" + RESET + ")",
        string(YELLOW) + "├ Ổ C: " + RESET + (c.tong ? chuyen_bytes_sang_gb(c.da_dung) + " / " + chuyen_bytes_sang_gb(c.tong) : "Không tìm thấy")
                       + " GB (" + GREEN + to_string(phan_tram_c_da_dung) + "%" + RESET + ")",
        string(YELLOW) + "├ Ổ D: " + RESET + (d.tong ? chuyen_bytes_sang_gb(d.da_dung) + " / " + chuyen_bytes_sang_gb(d.tong) : "Không tìm thấy")
                       + " GB (" + GREEN + to_string(phan_tram_d_da_dung) + "%" + RESET + ")",
        string(YELLOW) + "├ Ngôn ngữ: " + RESET + language,
        string(YELLOW) + "├ Pin: " + RESET + GREEN + pin.substr(0, 3) + RESET + pin.substr(3, 25) + RESET,
        string(YELLOW) + "├ Local IP (Wi-Fi): " + RESET + ip,
        string(YELLOW) + "◉" + RESET,
        khoi_mau(BG_BLACK) + khoi_mau(BG_RED) + khoi_mau(BG_GREEN) + khoi_mau(BG_YELLOW)  +
        khoi_mau(BG_BLUE) + khoi_mau(BG_MAGENTA) + khoi_mau(BG_CYAN) + khoi_mau(BG_WHITE),
        khoi_mau(BG_BRIGHT_BLACK) + khoi_mau(BG_BRIGHT_RED) + khoi_mau(BG_BRIGHT_GREEN) + khoi_mau(BG_BRIGHT_YELLOW)  +
        khoi_mau(BG_BRIGHT_BLUE) + khoi_mau(BG_BRIGHT_MAGENTA) + khoi_mau(BG_BRIGHT_CYAN) + khoi_mau(BG_BRIGHT_WHITE),
    };

    string ten_windows = lay_ten_windows();
    vector<string> banner;
    if(ten_windows == "Windows 7") banner = banner_windows_xp_to_7;
    else if(ten_windows == "Windows 8.1" || ten_windows == "Windows 8" || ten_windows == "Windows 10") banner = banner_windows_8_to_10;
    else if(ten_windows == "Windows 11") banner = banner_windows_11;
    else banner = banner_windows_2000;

    size_t so_dong_toi_da = max(banner.size(), dong_thong_tin.size());
    for (size_t i = 0; i < so_dong_toi_da; ++i) {
        if (i < banner.size())
            cout << LIGHT_BLUE << banner[i] << RESET;
        else
            cout << string(banner.size(), ' '); 
        cout << "    ";
        if (i < dong_thong_tin.size())
            cout << dong_thong_tin[i];
        cout << endl;
    }
}

// [MAIN]
C_FETCH {
    enable_ansi_colors();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    #ifdef _WIN32
        chuong_trinh_chinh();
    #else 
        cerr << YELLOW << "Cảnh báo: " << RED << "Không hỗ trợ chạy trên hệ điều hành này !" << RESET << endl;
        return -1;
    #endif
}
