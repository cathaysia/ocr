#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <QApplication>
#include <QtGlobal>

#include "mainwindow.h"

void SetEnvIfNotExists(const char* name, QString const& val) {
    auto res = qgetenv(name);
    if(!res.isEmpty()) return;

    qputenv(name, val.toLocal8Bit());
}

int main(int argc, char* argv[]) {
    auto cur       = QApplication::applicationDirPath();
    auto data_path = cur + "tessdata";
    SetEnvIfNotExists("TESSDATA_PREFIX", data_path);
    QApplication app(argc, argv);

    MainWindow w;
    w.resize(800, 600);
    w.show();

    return app.exec();
}
