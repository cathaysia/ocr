find_package(Tesseract QUIET)

if(NOT Tesseract_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(tesseract REQUIRED tesseract IMPORTED_TARGET)
  add_library(Tesseract::libtesseract ALIAS PkgConfig::tesseract)
endif()
