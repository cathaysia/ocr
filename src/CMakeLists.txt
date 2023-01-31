set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(common)
add_subdirectory(ui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
  ${PROJECT_NAME}
  code_highlight.cpp
  code_highlight.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  ocr_tesseract.cpp
  ocr_tesseract.h
)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/plugins"
          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
)

include(FindDeployQt)

if(DeployQt)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${DeployQt} $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
endif()

include(SearchTesseract)
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
          Tesseract::libtesseract
          Qt${QT_DEFAULT_MAJOR_VERSION}::Widgets
          QHotkey::QHotkey
          pybind11::embed
          ${PROJECT_NAME}::common
          ${PROJECT_NAME}::ui
)
