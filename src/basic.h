//
// Created by xusiwei on 2025/9/22.
//

#ifndef PE_TOOL_BASIC_H
#define PE_TOOL_BASIC_H
#include <string>
#include <windows.h>
#include <commdlg.h>

class basic {
public:
    static std::wstring StringToWString(const std::string& str);
    static std::string WStringToString(const std::wstring& wstr);
    static std::wstring openFileDialog();
};



#endif //PE_TOOL_BASIC_H
