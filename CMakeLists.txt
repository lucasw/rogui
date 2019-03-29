cmake_minimum_required(VERSION 3.5)
project(imgui_test)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

# TODO(lucasw) currently have to symlink imgui in,
# later could make it a subrepo, or git clone it right here
# from cmake.
include_directories(
  include
  imgui
  imgui/examples
  imgui/examples/libs/gl3w
  ${OpenCV_INCLUDE_DIRECTORIES}
  ${SDL2_INCLUDE_DIRS}
)

add_library(imguitest SHARED
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/examples/imgui_impl_sdl.cpp
  imgui/examples/imgui_impl_opengl3.cpp
  imgui/examples/libs/gl3w/GL/gl3w.c
)
set_source_files_properties(imgui/examples/libs/gl3w/GL/gl3w.c PROPERTIES COMPILE_FLAGS -Wno-pedantic)

target_link_libraries(imguitest
  ${OpenCV_LIBRARIES}
  ${PCL_LIBRARIES}
  ${SDL2_LIBRARIES}
  GL
  dl
)

add_executable(imgui_test
  src/main.cpp
  src/app.cpp
  src/automata.cpp
  src/utility.cpp
)
target_link_libraries(imgui_test imguitest)
