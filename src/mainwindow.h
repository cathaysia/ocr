#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
};

class OcrTesseract;
class CodeHighLightCode;
class ScreenCapture;
class QHotkey;

namespace Kvantum {
class Style;
};

class MainWindow : public QMainWindow {
    Q_OBJECT;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject* obj, QEvent* e) override;

    void slotCaptureScreen();
    void slotReloadLangs(QString const& text);

private:
    void InitSettingPage();
signals:
    void signalPixmapReady(QPixmap const& pixmap);
    void signalHtmlReady(QString const& html);
    void signalPlaintxtReady(QString const& txt);

private:
    Ui::MainWindow*    ui;
    ScreenCapture*     capture_;
    OcrTesseract*      tesseract_;
    QHotkey*           hotkey_;
    CodeHighLightCode* high_;
    Kvantum::Style*    kvantum_;
};
