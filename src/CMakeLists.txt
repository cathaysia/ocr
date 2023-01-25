set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(
  dragon_screen
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  ocr_tesseract.cpp
  ocr_tesseract.h
  screen_capture.h
  screen_capture.cpp
)

find_package(Tesseract REQUIRED)
find_package(Qt5 COMPONENTS Widgets)

target_link_libraries(dragon_screen PRIVATE fmt::fmt spdlog::spdlog Tesseract::libtesseract Qt5::Widgets)
