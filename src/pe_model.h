//
// Created by xusiwei on 2025/9/13.
//

#ifndef PE_TOOL_PE_MODEL_H
#define PE_TOOL_PE_MODEL_H


#include <string>
#include <windows.h>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <unordered_set>

struct SectionInfo {
    std::string name;
    uint32_t virtualAddress;   // RVA
    uint32_t virtualSize;      // 内存大小
    uint32_t rawAddress;       // 文件偏移
    uint32_t rawSize;          // 文件大小
    uint32_t characteristics;  // 属性标志
};

struct ExportedFunction {
    std::string name;
    uint32_t ordinal;
    uint32_t rva;
};

struct ImportedFunction {
    std::string name;
    uint16_t hint = 0;
    uint16_t ordinal = 0;
};

struct ImportModule {
    std::string dllName;
    std::vector<ImportedFunction> functions;
};

class pe_model {
public:
    explicit pe_model(const std::wstring& file_path)
        : m_file_path(file_path) {
        load_pe_info();
    }
    explicit pe_model() {

    }
    ~pe_model() = default;

private:
    std::wstring    m_file_path;
    bool            m_is64 = false;
    uint32_t        m_entryPointRva = 0;
    uint32_t        m_entryPointRaw = 0;
    uint32_t        m_sizeOfImage = 0;
    uint32_t        m_sectionsAlignment = 0;
    uint32_t        m_fileAlignment = 0;
    uint16_t        m_numberOfSections = 0;
    uint32_t        m_sizeOfHeaders = 0;

    std::vector<SectionInfo> m_sections;
    std::vector<ExportedFunction> m_exports;
    std::vector<ImportModule> m_imports;
public:
    [[nodiscard]] uint32_t m_size_of_headers() const {
        return m_sizeOfHeaders;
    }

    [[nodiscard]] uint32_t m_size_of_image() const {
        return m_sizeOfImage;
    }

    [[nodiscard]] uint32_t m_sections_alignment() const {
        return m_sectionsAlignment;
    }

    [[nodiscard]] uint32_t m_file_alignment() const {
        return m_fileAlignment;
    }

    [[nodiscard]] uint16_t m_number_of_sections() const {
        return m_numberOfSections;
    }

    [[nodiscard]] std::wstring file_path() const {
        return m_file_path;
    }

    [[nodiscard]] uint32_t entry_point_rva() const {
        return m_entryPointRva;
    }

    [[nodiscard]] uint32_t entry_point_raw() const {
        return m_entryPointRaw;
    }

    [[nodiscard]] uint64_t image_base() const {
        return m_imageBase;
    }
    void set_file_path(const std::wstring &file_path) {
        m_file_path = file_path;
        load_pe_info();
    }
    const std::vector<SectionInfo>& sections() const {
        return m_sections;
    }

    [[nodiscard]] std::vector<ImportModule> imports() const {
        return m_imports;
    }

    [[nodiscard]] std::vector<ExportedFunction> exports() const {
        return m_exports;
    }

private:
    uint64_t        m_imageBase = 0;

private:
    void load_pe_info() ;

    void cleanup(LPVOID base, HANDLE hMapping, HANDLE hFile);

    uint32_t rvaToRaw(uint32_t rva, PIMAGE_NT_HEADERS ntHeader);

};


#endif //PE_TOOL_PE_MODEL_H