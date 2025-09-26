//
// Created by xusiwei on 2025/9/5.
//

#ifndef PE_TOOL_PE_VIEW_H
#define PE_TOOL_PE_VIEW_H

#include "imgui.h"
#include "pe_ViewModel.h"
#include <memory>
#include "basic.h"
#ifndef  SOKOL_APP_IMPL
#include "sokol_app.h"
#endif
#include <string_view>

class pe_view {
public:
    pe_view();
    ~pe_view();

    void show();
    void handle_event(const sapp_event* ev) const;

private:
    void menu_bar();

    void tab_bar() const;

    void tab_header() const;

    void tab_sections() const;

    void tab_exports() const;

    void tab_imports() const;

    void about() const;

    std::unique_ptr<pe_ViewModel> m_view_model;
};


#endif //PE_TOOL_PE_VIEW_H