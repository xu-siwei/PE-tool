//
// Created by xusiwei on 2025/9/21.
//

#ifndef PE_TOOL_PE_VIEWMODEL_H
#define PE_TOOL_PE_VIEWMODEL_H

#include "pe_model.h"
#include <functional>
#include <memory>



class pe_ViewModel {
public:
    using NotifyCallback = std::function<void()>;
    pe_ViewModel() = default;


    void setFilePath(const std::wstring& filePath);

    std::wstring filePath() const { return m_model ? m_model->file_path() : L""; }
    uint32_t entryPointRva() const { return m_model ? m_model->entry_point_rva() : 0; }
    uint32_t entryPointRaw() const { return m_model ? m_model->entry_point_raw() : 0; }
    uint64_t imageBase()     const { return m_model ? m_model->image_base() : 0; }
    uint32_t sizeOfImage()  const { return m_model ? m_model->m_size_of_image() : 0; }
    uint32_t sectionsAlignment() const { return m_model ? m_model->m_sections_alignment() : 0; }
    uint32_t fileAlignment() const { return m_model ? m_model->m_file_alignment() : 0; }
    uint16_t numberOfSections() const { return m_model ? m_model->m_number_of_sections() : 0; }
    uint32_t sizeOfHeaders() const { return m_model ? m_model->m_size_of_headers() : 0; }
    std::vector<SectionInfo> sections() const { return m_model ? m_model->sections() : std::vector<SectionInfo>(); }
    std::vector<ExportedFunction> exports() const {return m_model ? m_model->exports() : std::vector<ExportedFunction>();}
    std::vector<ImportModule> imports() const {return m_model ? m_model->imports() : std::vector<ImportModule>();}
private:
    bool loadPeFile(const std::wstring& filePath);

private:
    std::unique_ptr<pe_model> m_model;
    std::string m_lastError;
};



#endif //PE_TOOL_PE_VIEWMODEL_H
