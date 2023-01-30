#include "code_highlight.h"

#include <pybind11/embed.h>
#include <spdlog/spdlog.h>

#include <QCoreApplication>

namespace py = pybind11;
using namespace py::literals;

CodeHighLightCode::CodeHighLightCode() : guard_(new py::scoped_interpreter {}) {
    auto sys = py::module_::import("sys");
    sys.attr("path").attr("append")(QCoreApplication::applicationDirPath().toStdString());
}

CodeHighLightCode::~CodeHighLightCode() {
    delete guard_;
    guard_ = nullptr;
}

std::string CodeHighLightCode::ShaderCode(std::string const& content) {
    std::string res = content;

    try {
        auto md   = py::module_::import("code_highlight");
        auto func = md.attr("code_shader")(content);
        res       = func.cast<std::string>();
    } catch(std::exception const& e) {
        spdlog::error("调用代码高亮时发生错误：{}", e.what());
    }

    return res;
}
