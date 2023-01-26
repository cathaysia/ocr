#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace tesseract {
class TessBaseAPI;
}

class OcrTesseract {
public:
    OcrTesseract();
    ~OcrTesseract();

    std::shared_ptr<char> Image(std::string const& path);
    std::shared_ptr<char> ImageFromMem(const void* data, size_t len);

    std::vector<std::string> GetAvailableLangs();
    std::vector<std::string> GetUsedLangs();

    void LoadLangs(std::vector<std::string> const& langs);

private:
    tesseract::TessBaseAPI* api_;
};
