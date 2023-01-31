add_library(${PROJECT_NAME}_common STATIC py.cpp py.h)
add_library(${PROJECT_NAME}::common ALIAS ${PROJECT_NAME}_common)

find_package(pybind11 REQUIRED)
find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS Core REQUIRED)
target_link_libraries(${PROJECT_NAME}_common PUBLIC pybind11::embed)
target_link_libraries(${PROJECT_NAME}_common PRIVATE Qt${QT_DEFAULT_MAJOR_VERSION}::Core spdlog::spdlog fmt::fmt)
