#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;
    w.resize(800, 600);
    w.show();

    return app.exec();
}
