//
// Created by xusiwei on 2025/9/22.
//

#include "basic.h"

std::wstring basic::StringToWString(const std::string& str) {
    const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                          static_cast<int>(str.size()), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                        static_cast<int>(str.size()), &wstr[0], size_needed);
    return wstr;
}

std::string basic::WStringToString(const std::wstring &wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                                          static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                        (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}

std::wstring basic::openFileDialog()
{
    wchar_t filename[MAX_PATH] = {0};
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"All Files\0*.*\0PE Files\0*.exe;*.dll\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameW(&ofn)) {
        return std::wstring(filename);
    }
    return {};
}
