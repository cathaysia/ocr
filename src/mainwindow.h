#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
};

class OcrTesseract;
class CodeHighLightCode;
class ScreenCapture;
class QHotkey;
class QSettings;

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
    void InitCodeHighLightWidget();
    void InitTesseractLangsWidget();
    void InitBasicFunctionButton();
signals:
    void signalPixmapReady(QPixmap const& pixmap);
    void signalHtmlReady(QString const& html);
    void signalPlaintxtReady(QString const& txt, bool requestHigh = false);

private:
    Ui::MainWindow*    ui;
    QSettings*         settings_;
    ScreenCapture*     capture_;
    OcrTesseract*      tesseract_;
    QHotkey*           hotkey_;
    CodeHighLightCode* highlight_;
#ifdef _WIN32
    Kvantum::Style* kvantum_;
#endif
};
