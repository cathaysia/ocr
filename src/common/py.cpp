#include "py.h"

#include <fmt/format.h>
#include <QCoreApplication>

#include <future>

std::once_flag flag;

py::scoped_interpreter& GetPyInstance() {
    static py::scoped_interpreter inter;

    std::call_once(flag, []() {
        auto sys    = py::module_::import("sys");
        auto append = sys.attr("path").attr("append");

        auto app_path = QCoreApplication::applicationDirPath().toStdString();
        append(app_path);
        append(fmt::format("{}/plugins/", app_path));
    });

    return inter;
}
