#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "code_highlight.h"
#include "ocr_tesseract.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <QBuffer>
#include <QCompleter>
#include <QFileDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>

#include <QHotkey>

#include "screen_capture.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , capture_(new ScreenCapture(nullptr))
    , tesseract_(new OcrTesseract())
    , hotkey_(new QHotkey(QKeySequence("F10"), true, qApp)) {
    ui->setupUi(this);
    if(!hotkey_->isRegistered()) {
        spdlog::error("快捷键注册失败");
    }

    CodeHighLightCode hi;
    auto              r = hi.GetAvailableThemes();
    for(auto const& item: r) {
        ui->cbox_themes->addItem(item.c_str());
    }

    ui->cbox_themes->setCurrentText("one-dark");

    auto                        usedLangs = tesseract_->GetUsedLangs();
    std::map<std::string, bool> usedLangsMap;
    {
        auto langs     = tesseract_->GetAvailableLangs();
        auto usedLangs = tesseract_->GetUsedLangs();

        QStringList qlangs;
        for(auto const& s: langs) {
            qlangs << s.c_str();
        }

        ui->line_lang->setText(fmt::format("{}", fmt::join(usedLangs, ", ")).c_str());

        ui->line_lang->SetCmpList(qlangs);
    }

    connect(ui->line_lang, &QLineEdit::textChanged, [this](QString const& text) {
        auto langs = tesseract_->GetAvailableLangs();
        auto res   = std::vector<std::string>();

        for(auto& v: text.split(",")) {
            v = v.trimmed();
            if(v.isEmpty()) continue;
            qDebug() << v;
            if(std::find(langs.begin(), langs.end(), v.toStdString()) == langs.end()) {
                spdlog::info("{} 不在支持的语言列表中，忽略", v.toStdString());
                ui->line_lang->setStyleSheet("color: red;");
                return;
            }
            res.push_back(v.toStdString());
        }
        ui->line_lang->setStyleSheet("");
        // TODO: 这里应当去重，但是不能使用 std::sort 和 uniq

        if(tesseract_->GetUsedLangs() == res) return;

        tesseract_->LoadLangs(res);
    });

    ui->lbl_img->installEventFilter(this);

    auto cap = [this]() {
        // FIXME: 选中后，主窗口会闪烁
        if(ui->cbox_hidden->isChecked()) {
            showMinimized();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            capture_->capture();
            showNormal();
            activateWindow();
        } else {
            capture_->capture();
        }
    };

    connect(ui->btn_capture, &QPushButton::clicked, cap);
    connect(hotkey_, &QHotkey::activated, cap);

    connect(capture_, &ScreenCapture::signalScreenReady, [this](QPixmap const& pix) {
        ui->lbl_img->setPixmap(pix);

        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        pix.save(&buffer, "PNG");
        auto res = tesseract_->ImageFromMem(buffer.buffer().constData(), buffer.size());

        CodeHighLightCode hig;

        ui->browser_txt->setHtml(hig.ShaderCode(res.get(), ui->cbox_themes->currentText().toStdString()).c_str());
    });
}

MainWindow::~MainWindow() {
    delete ui;
    delete capture_;
    delete tesseract_;

    ui         = nullptr;
    tesseract_ = nullptr;
    capture_   = nullptr;
}

bool MainWindow::eventFilter(QObject* obj, QEvent* e) {
    if(obj != ui->lbl_img || e->type() != QEvent::MouseButtonPress) return false;

    auto    pix_path = QFileDialog::getOpenFileName();
    QPixmap pix { pix_path };
    if(pix.isNull()) return false;

    ui->lbl_img->setPixmap(pix);

    auto res = tesseract_->Image(pix_path.toStdString());
    ui->browser_txt->setPlainText({ res.get() });

    return true;
}
