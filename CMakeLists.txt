cmake_minimum_required(VERSION 3.15)
project(CSOPESY-MO1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Console application (original CSOPESY scheduler CLI)
# Globs every src/*.cpp EXCEPT the GUI mockup under src/gui/.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE CONSOLE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER CONSOLE_SOURCES EXCLUDE REGEX "/gui/")

add_executable(csopesy_mo1 ${CONSOLE_SOURCES})
target_include_directories(csopesy_mo1 PRIVATE include)

# ---------------------------------------------------------------------------
# GUI mockup (Dear ImGui + GLFW + OpenGL) — separate, additive target.
# Dear ImGui is vendored under lib/; GLFW comes from the system (Homebrew).
# ---------------------------------------------------------------------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui-1.92.8)

# --- Dear ImGui static lib (core + GLFW/OpenGL3 backends) ---
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# --- GLFW (system install; prefer CMake config, fall back to pkg-config) ---
find_package(glfw3 QUIET)
if(glfw3_FOUND)
    target_link_libraries(imgui PUBLIC glfw)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
    target_link_libraries(imgui PUBLIC PkgConfig::GLFW)
endif()

# --- Platform OpenGL ---
if(APPLE)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC ${OPENGL_FRAMEWORK})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC OpenGL::GL)
endif()

file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS "src/gui/*.cpp")
add_executable(csopesy_gui ${GUI_SOURCES})
target_include_directories(csopesy_gui PRIVATE include include/gui)
target_link_libraries(csopesy_gui PRIVATE imgui)
