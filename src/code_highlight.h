#pragma once

#include <string>
#include <vector>

namespace pybind11 {
class scoped_interpreter;
class object;
}    // namespace pybind11

class CodeHighLightCode {
public:
    CodeHighLightCode();
    ~CodeHighLightCode();

    std::string              ShaderCode(std::string const& content, std::string const& theme);
    std::vector<std::string> GetAvailableThemes();

private:
    pybind11::scoped_interpreter* guard_;
};
