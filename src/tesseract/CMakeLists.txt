add_library(${PROJECT_NAME}_tesseract STATIC ocr_tesseract.cpp ocr_tesseract.h)
add_library(${PROJECT_NAME}::tesseract ALIAS ${PROJECT_NAME}_tesseract)

include(SearchTesseract)
target_link_libraries(${PROJECT_NAME}_tesseract PRIVATE spdlog::spdlog fmt::fmt leptonica Tesseract::libtesseract)
