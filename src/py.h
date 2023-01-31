#pragma once

#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace py::literals;

// 此函数必须执行以初始化 Python 解释器。即使不需要 Py 解释器实例
py::scoped_interpreter& GetPyInstance();
