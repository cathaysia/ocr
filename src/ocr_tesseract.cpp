#include "ocr_tesseract.h"

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

OcrTesseract::OcrTesseract() : api_(new tesseract::TessBaseAPI) {
    LoadLangs({ "chi_sim", "eng" });

    api_->SetPageSegMode(tesseract::PSM_AUTO_OSD);

    api_->GetInitLanguagesAsString();
}

OcrTesseract::~OcrTesseract() {
    api_->End();
    delete api_;
    api_ = nullptr;
}

// FIXME: 中文识别时出现的空格
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
std::shared_ptr<char> OcrTesseract::ImageFromMem(const void* data, size_t len) {
    auto image = pixReadMem((const l_uint8*)data, len);

    api_->SetImage(image);
    auto outText = api_->GetUTF8Text();

    pixDestroy(&image);
    return std::shared_ptr<char>(outText, std::default_delete<char[]>());
}
std::vector<std::string> OcrTesseract::GetAvailableLangs() {
    std::vector<std::string> langs;
    api_->GetAvailableLanguagesAsVector(&langs);
    return langs;
}

void OcrTesseract::LoadLangs(std::vector<std::string> const& langs) {
    auto lan = fmt::format("{}", fmt::join(langs, "+"));
    auto ret = api_->Init(nullptr, lan.c_str());
    if(ret) {
        throw std::runtime_error("tesseract 初始化失败");
    }

    spdlog::info("设置 tesseract 识别的语言为：{}", langs);
}
std::vector<std::string> OcrTesseract::GetUsedLangs() {
    std::vector<std::string> res;
    std::string              source { api_->GetInitLanguagesAsString() };

    auto ps    = source.find('+');
    int  start = 0;
    while(ps != std::string::npos) {
        res.push_back(source.substr(start, ps));
        start = ps + 1;
        ps    = source.find('+', start);
    }
    res.push_back(source.substr(start));

    return res;
}
