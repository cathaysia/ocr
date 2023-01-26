#pragma once

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>
#include <stdexcept>
#include <string>

class OcrTesseract {
public:
    OcrTesseract();
    ~OcrTesseract();

    std::shared_ptr<char> Image(std::string const& path);
    std::shared_ptr<char> ImageFromMem(const void* data, size_t len);

    std::vector<std::string> GetAvailableLangs();

    void LoadLangs(std::vector<std::string> const& langs);

private:
    tesseract::TessBaseAPI* api_;
};
