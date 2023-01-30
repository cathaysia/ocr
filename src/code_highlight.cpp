#include "code_highlight.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <spdlog/spdlog.h>

#include <QCoreApplication>

namespace py = pybind11;
using namespace py::literals;

CodeHighLightCode& CodeHighLightCode::GetInstance() {
    static CodeHighLightCode high;
    return high;
}

CodeHighLightCode::CodeHighLightCode() : guard_(new py::scoped_interpreter {}), module_(new py::module_) {
    auto sys = py::module_::import("sys");
    sys.attr("path").attr("append")(QCoreApplication::applicationDirPath().toStdString());

    try {
        *module_ = py::module_::import("code_highlight");
    } catch(std::exception const& e) {
        spdlog::error("调用代码高亮时发生错误：{}", e.what());
        delete module_;
        module_ = nullptr;
    }
}

CodeHighLightCode::~CodeHighLightCode() {
    delete guard_;
    guard_ = nullptr;
}

std::string CodeHighLightCode::ShaderCode(std::string const& content, std::string const& theme) {
    if(!IsAvailable()) return {};

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
