add_library(${PROJECT_NAME}_eventobj STATIC mouse_clicked.cpp mouse_clicked.h focus.h focus.cpp)
add_library(${PROJECT_NAME}::eventobj ALIAS ${PROJECT_NAME}_eventobj)

find_package(
    Qt${QT_DEFAULT_MAJOR_VERSION}
    COMPONENTS Widgets
    REQUIRED
)
target_link_libraries(${PROJECT_NAME}_eventobj PRIVATE Qt${QT_DEFAULT_MAJOR_VERSION}::Widgets spdlog::spdlog fmt::fmt)
