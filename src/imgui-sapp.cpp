#define SOKOL_APP_IMPL
#include "sokol_app.h"
#define SOKOL_GFX_IMPL
#include "sokol_gfx.h"
#define SOKOL_LOG_IMPL
#include "sokol_log.h"
#define SOKOL_GLUE_IMPL
#include "imgui.h"
#include "sokol_glue.h"
#define SOKOL_IMGUI_IMPL
#include "sokol_imgui.h"

#include "pe_view.h"
#include "icons.h"

static sg_pass_action pass_action;

void init() {
  // setup sokol-gfx, sokol-time and sokol-imgui
  sg_desc desc = {};
  desc.environment = sglue_environment();
  desc.logger.func = slog_func;
  sg_setup(&desc);

  // use sokol-imgui with all default-options (we're not doing
  // multi-sampled rendering or using non-default pixel formats)
  simgui_desc_t simgui_desc = {};
  simgui_desc.logger.func = slog_func;
  simgui_setup(&simgui_desc);

  // initial clear color
  pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
  pass_action.colors[0].clear_value = {0.0f, 0.5f, 0.7f, 1.0f};

  // 设置微软雅黑字体,并指定字体大小
  ImFont *font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      "C:/Windows/Fonts/msyh.ttc", 30, nullptr,
      // 设置加载中文
      ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
  // 必须判断一下字体有没有加载成功
  IM_ASSERT(font != nullptr);
  ImGui::GetIO().FontDefault = font;
}

static pe_view g_view;


void frame() {

  const int width = sapp_width();
  const int height = sapp_height();
  simgui_new_frame({width, height, sapp_frame_duration(), sapp_dpi_scale()});

  g_view.show();

  // the sokol_gfx draw pass
  sg_pass pass = {};
  pass.action = pass_action;
  pass.swapchain = sglue_swapchain();
  sg_begin_pass(&pass);
  simgui_render();
  sg_end_pass();
  sg_commit();
}

void cleanup() {
  simgui_shutdown();
  sg_shutdown();
}

void input(const sapp_event *event) {
  simgui_handle_event(event);
  g_view.handle_event(event);;
}

sapp_desc sokol_main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  sapp_desc desc = {};
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = input;
  desc.window_title = "PE-tool";
  desc.ios_keyboard_resizes_canvas = false;
  desc.icon = app_icon;
  desc.enable_clipboard = true;
  desc.enable_dragndrop = true;
  desc.max_dropped_files = 1;
  desc.width = 900;  // 设置默认窗口宽度
  desc.height = 600; // 设置默认窗口高度
  desc.logger.func = slog_func;
  return desc;
}
