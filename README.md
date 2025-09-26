# PE-tool

PE-tool 是一个用于分析和查看 PE 文件（Windows 可执行文件格式）的图形界面工具，基于 ImGui 和 sokol 库开发。

## 功能特性
- 支持加载和解析 PE 文件
- 以图形界面展示 PE 文件结构和详细信息
- 友好的用户交互体验
- 支持 Windows 平台

## 构建与运行

### 依赖
- CMake
- Visual Studio 或 CLion
- sokol 库（已包含在 src/sokol/）
- ImGui 库（已包含在 src/imgui/）

### 构建步骤
1. 使用 CMake 生成项目文件：
   ```
   cmake -S . -B build
   ```
2. 使用 Visual Studio 或 CLion 打开生成的项目文件并编译。
3. 运行生成的 PE-tool.exe。

## 使用方法
- 打开软件后，点击菜单栏可加载 PE 文件。
- 可通过菜单栏“关于”查看软件信息。

## 版本信息
当前版本：1.0.0


