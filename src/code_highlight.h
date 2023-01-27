#pragma once

#include <string>

namespace pybind11 {
class scoped_interpreter;
class object;
}    // namespace pybind11

class CodeHighLightCode {
public:
    CodeHighLightCode();
    ~CodeHighLightCode();

    std::string ShaderCode(std::string const& content);

private:
    pybind11::scoped_interpreter* guard_;
};
