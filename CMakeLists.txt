cmake_minimum_required(VERSION 3.16)
project(TGL VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(TGL_BUILD_EXAMPLES "Build example programs" ON)
option(TGL_BUILD_TESTS "Build tests" OFF)
option(TGL_USE_WAYLAND "Use Wayland instead of X11 on Linux" OFF)

# Platform detection
if(WIN32)
    set(TGL_PLATFORM "WIN32")
    add_definitions(-DTGL_PLATFORM_WINDOWS)
elseif(APPLE)
    set(TGL_PLATFORM "MACOS")
    add_definitions(-DTGL_PLATFORM_MACOS)
elseif(UNIX)
    set(TGL_PLATFORM "LINUX")
    add_definitions(-DTGL_PLATFORM_LINUX)
    if(TGL_USE_WAYLAND)
        add_definitions(-DTGL_USE_WAYLAND)
    endif()
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Source files
file(GLOB_RECURSE TGL_SOURCES "src/*.cpp")
file(GLOB_RECURSE TGL_HEADERS "include/*.h")

# Library target
add_library(tgl STATIC ${TGL_SOURCES})

target_include_directories(tgl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(tgl PUBLIC OpenGL::GL)

# Platform-specific linking
if(WIN32)
    target_link_libraries(tgl PUBLIC user32 gdi32 winmm ole32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(tgl PUBLIC ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
elseif(UNIX)
    if(TGL_USE_WAYLAND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl)
        target_link_libraries(tgl PUBLIC ${WAYLAND_LIBRARIES} EGL)
        target_include_directories(tgl PUBLIC ${WAYLAND_INCLUDE_DIRS})
    else()
        target_link_libraries(tgl PUBLIC X11 dl pthread)
    endif()
endif()

# Examples
if(TGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install
install(TARGETS tgl EXPORT tgl-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(DIRECTORY include/tgl DESTINATION include)
install(EXPORT tgl-targets
    FILE TGLConfig.cmake
    NAMESPACE TGL::
    DESTINATION lib/cmake/TGL
)
