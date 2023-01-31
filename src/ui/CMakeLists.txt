add_library(
  ${PROJECT_NAME}_ui STATIC
  delimitedcompleter.cpp
  delimitedcompleter.h
  menulineedit.cpp
  menulineedit.h
  screen_capture.cpp
  screen_capture.h
)
add_library(${PROJECT_NAME}::ui ALIAS ${PROJECT_NAME}_ui)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
target_include_directories(${PROJECT_NAME}_ui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME}_ui PRIVATE Qt5::Widgets spdlog::spdlog fmt::fmt)
