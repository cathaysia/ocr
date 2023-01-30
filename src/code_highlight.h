#pragma once

#include <string>
#include <vector>

namespace pybind11 {
class scoped_interpreter;
class object;
}    // namespace pybind11

class CodeHighLightCode {
    CodeHighLightCode();
    ~CodeHighLightCode();

public:
    CodeHighLightCode(const CodeHighLightCode&)            = delete;
    CodeHighLightCode(CodeHighLightCode&&)                 = delete;
    CodeHighLightCode& operator=(const CodeHighLightCode&) = delete;
    CodeHighLightCode& operator=(CodeHighLightCode&&)      = delete;

    static CodeHighLightCode& GetInstance();

    std::string              ShaderCode(std::string const& content, std::string const& theme);
    std::vector<std::string> GetAvailableThemes();

private:
    pybind11::scoped_interpreter* guard_;
};
