set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(common)
add_subdirectory(ui)
add_subdirectory(tesseract)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
  ${PROJECT_NAME}
  WIN32
  code_highlight.cpp
  code_highlight.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/plugins"
          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
)

find_package(
  Qt${QT_DEFAULT_MAJOR_VERSION}
  COMPONENTS Widgets
  REQUIRED
)
find_package(pybind11 REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/autogen")

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE fmt::fmt
          spdlog::spdlog
          Qt${QT_DEFAULT_MAJOR_VERSION}::Widgets
          QHotkey::QHotkey
          pybind11::embed
          ${PROJECT_NAME}::common
          ${PROJECT_NAME}::ui
          ${PROJECT_NAME}::tesseract
)
