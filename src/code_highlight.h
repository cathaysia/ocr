#pragma once

#include <string>
#include <vector>

namespace pybind11 {
class scoped_interpreter;
class module_;
class object;
}    // namespace pybind11

class CodeHighLightCode {
public:
    CodeHighLightCode();
    ~CodeHighLightCode();

    CodeHighLightCode(const CodeHighLightCode&)            = delete;
    CodeHighLightCode(CodeHighLightCode&&)                 = delete;
    CodeHighLightCode& operator=(const CodeHighLightCode&) = delete;
    CodeHighLightCode& operator=(CodeHighLightCode&&)      = delete;

    std::string              ShaderCode(std::string const& content, std::string const& theme);
    std::vector<std::string> GetAvailableThemes();
    bool                     IsAvailable();

private:
    pybind11::module_* module_;
};
