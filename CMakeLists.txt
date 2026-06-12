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

# --- GLFW (FetchContent for cross-platform portability) ---
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

target_link_libraries(imgui PUBLIC glfw)

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
