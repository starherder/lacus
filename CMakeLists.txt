cmake_minimum_required(VERSION 3.15)
project(Lacus VERSION 1.0.0 LANGUAGES CXX)

# 使用C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加UTF-8支持以解决spdlog的Unicode问题
if(WIN32)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
endif()

# 设置输出目录为bin目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 设置外部库路径
set(SDL3_PATH "${CMAKE_SOURCE_DIR}/ext/SDL3/")
set(SPDLOG_PATH "${CMAKE_SOURCE_DIR}/ext/spdlog/")
set(IMGUI_PATH "${CMAKE_SOURCE_DIR}/ext/imgui")
set(EXT_PATH "${CMAKE_SOURCE_DIR}/ext/")


# 手动设置包含目录
include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${SDL3_PATH}/include"
    "${SPDLOG_PATH}/include"
    "${IMGUI_PATH}"
    "${EXT_PATH}"
)

# 根据平台设置SDL3库目录
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_ARCH "x64")
    else()
        set(PLATFORM_ARCH "x86")
    endif()
    link_directories("${SDL3_PATH}/libs")
    link_directories("${SPDLOG_PATH}/libs")
endif()

# 引用的imgui文件
set(IMGUI_SOURCES
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
    # Backend files
    ${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer3.cpp
)

# 递归收集所有子目录的 cpp 文件
file(GLOB_RECURSE SOURCES "src/*.cpp")


# 添加可执行文件
add_executable(${PROJECT_NAME} 
                ${SOURCES} 
                ${IMGUI_SOURCES}
)

# 链接SDL3库和系统库
if(WIN32 AND MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        SDL3-static
        SDL3_image-static
        SDL3_mixer-static
        SDL3_ttf-static
        SDL_uclibc
        winmm
        setupapi
        imm32
        version
        # GCC可能需要的额外库
        mingw32
        mwindows
        Rpcrt4 
        Usp10
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        SDL3-static
        SDL3_image-static
        SDL3_mixer-static
        SDL3_ttf-static
        SDL_uclibc
        winmm
        setupapi
        imm32
        version
    )
endif()


# 对于GCC，我们不需要复制DLL，因为我们使用的是静态库