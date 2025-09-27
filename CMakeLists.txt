# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(PE-tool)


# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置宏定义 SOKOL_GLCORE
add_definitions(-DSOKOL_GLCORE)

# 添加头文件搜索路径
include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sokol
)

# 收集 src 目录下所有 .cpp 文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES} src/res/resource.rc)

