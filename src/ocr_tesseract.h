#pragma once

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>
#include <stdexcept>
#include <string>

class OcrTesseract {
public:
    OcrTesseract(std::string const& engine);
    ~OcrTesseract();

    std::shared_ptr<char> Image(std::string const& path);
    std::shared_ptr<char> ImageFromMem(const l_uint8* data, size_t len);

private:
    tesseract::TessBaseAPI* api_;
};
