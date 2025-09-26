//
// Created by xusiwei on 2025/9/21.
//

#include "pe_ViewModel.h"


bool pe_ViewModel::loadPeFile(const std::wstring &filePath) {
    try {
        m_model = std::make_unique<pe_model>(filePath);

        return true;
    }catch (const std::exception& e) {
        m_lastError = e.what();
        return false;
    }
}

void pe_ViewModel::setFilePath(const std::wstring &filePath) {
    if (m_model and m_model->file_path() == filePath) {
        return;
    }
    if (m_model) {
        m_model.reset();
        m_lastError.clear();
    }
    loadPeFile(filePath);
}


