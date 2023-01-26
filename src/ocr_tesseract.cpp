#include "ocr_tesseract.h"

#include <spdlog/spdlog.h>

OcrTesseract::OcrTesseract(std::string const& engine) : api_(new tesseract::TessBaseAPI) {
    auto ret = api_->Init(nullptr, engine.c_str());
    if(ret) {
        throw std::runtime_error("tesseract 初始化失败");
    }

    api_->SetPageSegMode(tesseract::PSM_AUTO_OSD);
}

OcrTesseract::~OcrTesseract() {
    api_->End();
    delete api_;
    api_ = nullptr;
}

std::shared_ptr<char> OcrTesseract::Image(std::string const& path) {
    Pix* image = pixRead(path.c_str());

    api_->SetImage(image);
    auto ret = api_->Recognize(0);
    if(ret) {
        spdlog::error("tesseract 设置 Recognize 失败");
    }

    auto outText = api_->GetUTF8Text();

    pixDestroy(&image);
    return std::shared_ptr<char>(outText, std::default_delete<char[]>());
}
std::shared_ptr<char> OcrTesseract::ImageFromMem(const l_uint8* data, size_t len) {
    auto image = pixReadMemPnm(data, len);

    api_->SetImage(image);
    auto outText = api_->GetUTF8Text();

    pixDestroy(&image);
    return std::shared_ptr<char>(outText, std::default_delete<char[]>());
}
