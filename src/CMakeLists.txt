set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

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
  screen_capture.cpp
  screen_capture.h
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
)
