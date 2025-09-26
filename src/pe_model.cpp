//
// Created by xusiwei on 2025/9/13.
//

#include "pe_model.h"

void pe_model::load_pe_info() {
    HANDLE hFile = CreateFileW(
            LPCWSTR(m_file_path.c_str()),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file");
    }

    HANDLE hMapping = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!hMapping) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to create file mapping");
    }

    LPVOID base = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!base) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        throw std::runtime_error("Failed to map view of file");
    }

    auto* dosHender = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
    if (dosHender->e_magic != IMAGE_DOS_SIGNATURE) {
        cleanup(base, hMapping, hFile);
        throw std::runtime_error("Not a valid PE file (DOS header mismatch)");
    }

    auto* ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<BYTE*>(base) + dosHender->e_lfanew);

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        cleanup(base, hMapping, hFile);
        throw std::runtime_error("Not a valid PE file (NT header mismatch)");
    }

    m_numberOfSections = ntHeader->FileHeader.NumberOfSections;
    WORD magic = ntHeader->OptionalHeader.Magic;
    IMAGE_DATA_DIRECTORY* export_dir;
    IMAGE_DATA_DIRECTORY* import_dir;
    if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        auto* opt32 = reinterpret_cast<PIMAGE_OPTIONAL_HEADER32>(&ntHeader->OptionalHeader);
        m_is64 = false;
        m_entryPointRva = opt32->AddressOfEntryPoint;
        m_imageBase = static_cast<uint64_t>(opt32->ImageBase);
        m_entryPointRaw = rvaToRaw(m_entryPointRva, ntHeader);
        m_sizeOfImage = opt32->SizeOfImage;
        m_sectionsAlignment = opt32->SectionAlignment;
        m_fileAlignment = opt32->FileAlignment;
        m_sizeOfHeaders = opt32->SizeOfHeaders;
        export_dir = &opt32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        import_dir = &opt32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }else if (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC){
        auto* opt64 = reinterpret_cast<PIMAGE_OPTIONAL_HEADER64>(&ntHeader->OptionalHeader);
        m_is64 = true;
        m_entryPointRva = opt64->AddressOfEntryPoint;
        m_imageBase = opt64->ImageBase;
        m_entryPointRaw = rvaToRaw(m_entryPointRva, ntHeader);
        m_sizeOfImage = opt64->SizeOfImage;
        m_sectionsAlignment = opt64->SectionAlignment;
        m_fileAlignment = opt64->FileAlignment;
        m_sizeOfHeaders = opt64->SizeOfHeaders;
        export_dir = &opt64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        import_dir = &opt64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }else {
        cleanup(base, hMapping, hFile);
        throw std::runtime_error("Unknow PE OptionalHeader magic");
    }

    m_sections.clear();
    auto* sections = IMAGE_FIRST_SECTION(ntHeader);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++sections) {
        m_sections.emplace_back(
            std::string(reinterpret_cast<const char *>(sections->Name), strnlen_s(reinterpret_cast<const char*>(sections->Name), IMAGE_SIZEOF_SHORT_NAME)),
            sections->VirtualAddress,
            sections->Misc.VirtualSize,
            sections->PointerToRawData,
            sections->SizeOfRawData,
            sections->Characteristics);
    }


    if (export_dir->VirtualAddress != 0 and export_dir->Size != 0) {
        // 导出表
        DWORD exportRva = export_dir->VirtualAddress;
        DWORD exportSize = export_dir->Size;

        DWORD exportRaw = rvaToRaw(exportRva, ntHeader);
        if (exportRaw != 0) {
            auto* exportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
                reinterpret_cast<BYTE*>(base)+exportRaw);
            DWORD* funcRVAs = reinterpret_cast<DWORD*>(
                reinterpret_cast<BYTE*>(base)+rvaToRaw(exportDir->AddressOfFunctions, ntHeader));
            DWORD* nameRVAs = reinterpret_cast<DWORD*>(
                reinterpret_cast<BYTE*>(base)+rvaToRaw(exportDir->AddressOfNames, ntHeader));
            WORD* ordinals = reinterpret_cast<WORD*>(
                reinterpret_cast<BYTE*>(base)+rvaToRaw(exportDir->AddressOfNameOrdinals, ntHeader));
            m_exports.clear();
            std::unordered_set<DWORD> namedFuncIndices; // 存储有名函数的func_idx
            for (DWORD i = 0; i < exportDir->NumberOfNames; ++i) {
                DWORD funcIdx = ordinals[i]; // 有名函数的func_idx
                namedFuncIndices.insert(funcIdx);
                m_exports.emplace_back(
                    reinterpret_cast<char*>(base)+rvaToRaw(nameRVAs[i], ntHeader),
                    exportDir->Base + ordinals[i],
                    funcRVAs[ordinals[i]]);
            }
            // 2. 处理无名称的函数（func_idx不在namedFuncIndices中）
            for (DWORD funcIdx = 0; funcIdx < exportDir->NumberOfFunctions; ++funcIdx) {
                // 跳过已处理的有名函数
                if (namedFuncIndices.count(funcIdx)) {
                    continue;
                }

                // 无名函数：名称为空字符串，仅有序号和地址
                m_exports.emplace_back(
                    "", // 无名函数没有名称
                    exportDir->Base + funcIdx, // 序号 = Base + func_idx
                    funcRVAs[funcIdx] // 函数地址
                );
            }
        }
    }
    DWORD importRaw = rvaToRaw(import_dir->VirtualAddress, ntHeader);
    if (importRaw != 0) {
        auto* impDesc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
            reinterpret_cast<BYTE*>(base)+importRaw);

        m_imports.clear();
        while (impDesc->Name != 0) {
            char* dllName = reinterpret_cast<char*>(
                reinterpret_cast<BYTE*>(base) + rvaToRaw(impDesc->Name, ntHeader));
            ImportModule module;
            if (dllName) module.dllName = dllName;

            DWORD thunkRva = impDesc->OriginalFirstThunk ? impDesc->OriginalFirstThunk : impDesc->FirstThunk;
            DWORD thunkRaw = rvaToRaw(thunkRva, ntHeader);

            if (thunkRaw) {
                if (m_is64) {
                    auto* thunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(
                        reinterpret_cast<BYTE*>(base) + thunkRaw);
                    while (thunk->u1.AddressOfData) {
                        ImportedFunction func{};
                        if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
                            func.ordinal = static_cast<uint16_t>(thunk->u1.Ordinal & 0xFFFF);
                        }else {
                            auto* impByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                reinterpret_cast<BYTE*>(base) + rvaToRaw((DWORD)thunk->u1.AddressOfData, ntHeader));
                            func.hint = impByName->Hint;
                            func.name = impByName->Name;
                        }
                        module.functions.push_back(func);
                        ++thunk;
                    }
                } else {
                    auto* thunk = reinterpret_cast<IMAGE_THUNK_DATA32*>(
                        reinterpret_cast<BYTE*>(base) + thunkRaw);
                    while (thunk->u1.AddressOfData) {
                        ImportedFunction func{};
                        if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
                            func.ordinal = static_cast<uint16_t>(thunk->u1.Ordinal & 0xFFFF);
                        } else {
                            auto* impByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                reinterpret_cast<BYTE*>(base) + rvaToRaw(thunk->u1.AddressOfData, ntHeader));
                            func.hint = impByName->Hint;
                            func.name = impByName->Name;
                        }
                        module.functions.push_back(func);
                        ++thunk;
                    }
                }
            }

            m_imports.push_back(module);
            ++impDesc;
        }
    }

    cleanup(base, hMapping, hFile);
}

void pe_model::cleanup(LPVOID base, HANDLE hMapping, HANDLE hFile) {
    if (base) UnmapViewOfFile(base);
    if (hMapping) CloseHandle(hMapping);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}

uint32_t pe_model::rvaToRaw(uint32_t rva, PIMAGE_NT_HEADERS ntHeader) {
    auto* section = IMAGE_FIRST_SECTION(ntHeader);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++section) {
        DWORD start = section->VirtualAddress;
        DWORD end = start + section->Misc.VirtualSize;
        if (rva >= start and rva < end) {
            return (rva - start) + section->PointerToRawData;
        }
    }
    return 0;
}
