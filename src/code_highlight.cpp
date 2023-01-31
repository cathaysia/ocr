#include "code_highlight.h"

#include "common/py.h"

#include <spdlog/spdlog.h>

CodeHighLightCode::CodeHighLightCode() : module_(new py::module_) {
    auto res = GetPyInstance();
    if(res == nullptr) {
        module_ = nullptr;
        spdlog::error("初始化 Python 解释器失败");
        return;
    }

    try {
        *module_ = py::module_::import("code_highlight");
    } catch(std::exception const& e) {
        spdlog::error("调用代码高亮时发生错误：{}", e.what());
        delete module_;
        module_ = nullptr;
    }
}

CodeHighLightCode::~CodeHighLightCode() {
    delete module_;
    module_ = nullptr;
}

std::string CodeHighLightCode::ShaderCode(std::string const& content, std::string const& theme) {
    if(!IsAvailable()) return content;

    std::string res = content;

    try {
        auto func = module_->attr("code_shader")(content, theme);
        res       = func.cast<std::string>();
    } catch(std::exception const& e) {
        spdlog::error("调用代码高亮时发生错误：{}", e.what());
    }

    return res;
}
std::vector<std::string> CodeHighLightCode::GetAvailableThemes() {
    if(!IsAvailable()) return {};

    std::vector<std::string> res;

    try {
        auto func = module_->attr("available_themes")();
        res       = func.cast<std::vector<std::string>>();
    } catch(std::exception const& e) {
        spdlog::error("调用代码高亮时发生错误：{}", e.what());
    }

    return res;
}
bool CodeHighLightCode::IsAvailable() {
    return module_ != nullptr;
}
