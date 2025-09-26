//
// Created by xusiwei on 2025/9/5.
//

#include "pe_view.h"

#include <iostream>
#include <ostream>
#include <format>
#include "string"
#include "sokol_app.h"

pe_view::pe_view() {
    m_view_model = std::make_unique<pe_ViewModel>();
}

pe_view::~pe_view() {
}

void pe_view::menu_bar() {
    /* 菜单栏 */
    bool open_about = false;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("文件")) {
            if (ImGui::MenuItem("打开", "Ctrl+O")) {
                // 打开文件操作
                std::wstring filePath = basic::openFileDialog();
                if (!filePath.empty()) {
                    m_view_model->setFilePath(filePath);
                }
            }
            if (ImGui::MenuItem("退出", "Ctrl+Q")) {
                sapp_quit();// 退出操作
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("帮助")) {
            if (ImGui::MenuItem("关于")) {
                open_about = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (open_about) {
        ImGui::OpenPopup("About");
    }
}

void pe_view::tab_bar() const {
    // 获取主窗口大小
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + 1));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y + 1));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
                                    | ImGuiWindowFlags_NoResize
                                    | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoScrollbar
                                    | ImGuiWindowFlags_NoScrollWithMouse;
    if (ImGui::Begin("mainWindow", nullptr, window_flags)) {
        // 在TabBar之前添加文件路径显示
        auto filePath = m_view_model->filePath();
        if (!filePath.empty()) {
            ImGui::Text(basic::WStringToString(filePath).c_str());
        } else {
            ImGui::Text("未选择文件");
        }

        if (ImGui::BeginTabBar("tab")) {
            if (ImGui::BeginTabItem("文件头")) {
                tab_header();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("节表")) {
                tab_sections();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("导出表")) {
                tab_exports();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("导入表")) {
                tab_imports();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}

void pe_view::tab_header() const {
    ImGui::Columns(2, "tab_header", false);

    /* 文本与输入框对齐 */
    ImGui::AlignTextToFramePadding();

    ImGui::Text("image_base");
    ImGui::NextColumn();
    std::string image_base_str = std::format("0x{:016X}", m_view_model->imageBase());
    ImGui::InputText("##image_base", image_base_str.data(), image_base_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("entry_point_rva");
    ImGui::NextColumn();
    std:: string entry_point_rva_str = std::format("0x{:08X}", m_view_model->entryPointRva());
    ImGui::InputText("##entry_rva", entry_point_rva_str.data(), entry_point_rva_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("entry_point_raw");
    ImGui::NextColumn();
    std::string entry_point_raw_str = std::format("0x{:08X}", m_view_model->entryPointRaw());
    ImGui::InputText("##entry_raw", entry_point_raw_str.data(), entry_point_raw_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("size_of_image");
    ImGui::NextColumn();
    std::string size_of_image_str = std::format("0x{:08X}", m_view_model->sizeOfImage());
    ImGui::InputText("##size_of_image", size_of_image_str.data(), size_of_image_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("sections_alignment");
    ImGui::NextColumn();
    std::string sections_alignment_str = std::format("0x{:08X}", m_view_model->sectionsAlignment());
    ImGui::InputText("##sections_alignment", sections_alignment_str.data(), sections_alignment_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("file_alignment");
    ImGui::NextColumn();
    std::string file_alignment_str = std::format("0x{:08X}", m_view_model->fileAlignment());
    ImGui::InputText("##file_alignment", file_alignment_str.data(), file_alignment_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("number_of_sections");
    ImGui::NextColumn();
    std::string number_of_sections_str = std::format("0x{:04X}", m_view_model->numberOfSections());
    ImGui::InputText("##number_of_sections", number_of_sections_str.data(), number_of_sections_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Text("sizeof_headers");
    ImGui::NextColumn();
    std::string size_of_headers_str = std::format("0x{:08X}", m_view_model->sizeOfHeaders());
    ImGui::InputText("##size_of_headers", size_of_headers_str.data(), size_of_headers_str.size(),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::NextColumn();

    ImGui::Columns(1);

}

void pe_view::tab_sections() const {
    auto sections = m_view_model->sections();
    if (ImGui::BeginTable("sections_table", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("virtual_address");
        ImGui::TableSetupColumn("virtual_size");
        ImGui::TableSetupColumn("raw_address");
        ImGui::TableSetupColumn("raw_size");
        ImGui::TableSetupColumn("characteristics");
        ImGui::TableHeadersRow();

        for (const auto& section : sections) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", section.name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%08X", section.virtualAddress);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("0x%08X", section.virtualSize);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("0x%08X", section.rawAddress);
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("0x%08X", section.rawSize);
            ImGui::TableSetColumnIndex(5);
            ImGui::Text("0x%08X", section.characteristics);
        }

        ImGui::EndTable();
    }
}

void pe_view::tab_exports() const {
    auto exports = m_view_model->exports();
    if (ImGui::BeginTable("exports_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("ordinal");
        ImGui::TableSetupColumn("rva");
        ImGui::TableHeadersRow();
        for (const auto& exp : exports) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", exp.name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", exp.ordinal);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("0x%08X", exp.rva);
        }
        ImGui::EndTable();
    }
}

void pe_view::tab_imports() const {
    auto imports = m_view_model->imports();
    for (const auto& import : imports) {
        std::string nodeLabel = std::format("{} ({})", import.dllName, import.functions.size());
        if (ImGui::TreeNode(nodeLabel.c_str())) {
            if (ImGui::BeginTable((import.dllName + "_imports_table").c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
                ImGui::TableSetupColumn("name");
                ImGui::TableSetupColumn("hint");
                ImGui::TableSetupColumn("ordinal");
                ImGui::TableHeadersRow();
                for (const auto& func : import.functions) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", func.name.empty() ? "-" : func.name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("0x%04X", func.hint);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", func.ordinal);
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
    }
}

void pe_view::about() const {
    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("PE-tool");
        ImGui::Text("Version 1.0.0");
        ImGui::Text("开发者：xu.siwei");
        ImGui::Separator();
        // 点击 OK 按钮关闭弹窗
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void pe_view::handle_event(const sapp_event* ev) const {
    if (ev->type == SAPP_EVENTTYPE_FILES_DROPPED) {
        int num_files = sapp_get_num_dropped_files();
        for (int i = 0; i < num_files; i++) {
            const char* path = sapp_get_dropped_file_path(i);

            if (path && *path) {
                std::string filePath(path);
                std::wstring wpath = basic::StringToWString(filePath);

                if (m_view_model) {
                    m_view_model->setFilePath(wpath);
                }

                std::cout << "Dropped file: " << filePath << std::endl;
            }
        }
    }
}

void pe_view::show() {
    menu_bar();
    tab_bar();
    about();
}
