#include "py.h"

#include <fmt/format.h>
#include <QCoreApplication>

#include <spdlog/spdlog.h>
#include <future>

std::once_flag flag;

py::scoped_interpreter* GetPyInstance() {
    static py::scoped_interpreter* inter = nullptr;

    std::call_once(
        flag,
        [](py::scoped_interpreter* pinter) {
            try {
                auto p = new py::scoped_interpreter;
                pinter = p;
            } catch(std::exception const& e) {
                spdlog::error("初始化 Python 解释器失败：{}", e.what());
                return;
            }

            auto sys    = py::module_::import("sys");
            auto append = sys.attr("path").attr("append");

            auto app_path = QCoreApplication::applicationDirPath().toStdString();
            append(app_path);
            append(fmt::format("{}/plugins/", app_path));
        },
        inter);

    return inter;
}
