#include "code_highlight.h"

#include <pybind11/embed.h>

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
    auto md  = py::module_::import("code_highlight");
    auto res = md.attr("code_shader")(content);

    return res.cast<std::string>();
}
