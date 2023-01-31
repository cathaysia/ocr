set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(common)
add_subdirectory(ui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
  dragon_screen
  code_highlight.cpp
  code_highlight.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  ocr_tesseract.cpp
  ocr_tesseract.h
)

find_package(Tesseract REQUIRED)
find_package(
  Qt5
  COMPONENTS Widgets
  REQUIRED
)
find_package(pybind11 REQUIRED)

target_include_directories(dragon_screen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/autogen")

target_link_libraries(
  dragon_screen
  PRIVATE fmt::fmt
          spdlog::spdlog
          Tesseract::libtesseract
          Qt5::Widgets
          QHotkey::QHotkey
          pybind11::embed
          ${PROJECT_NAME}::common
          ${PROJECT_NAME}::ui
)
