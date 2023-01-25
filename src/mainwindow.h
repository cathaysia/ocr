#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
};

class OcrTesseract;
class ScreenCapture;

class MainWindow : public QMainWindow {
    Q_OBJECT;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    Ui::MainWindow* ui;
    ScreenCapture*  capture_;
    OcrTesseract*   tesseract_;
};
