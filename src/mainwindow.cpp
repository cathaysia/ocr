#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "code_highlight.h"
#include "tesseract/ocr_tesseract.h"
#include "ui/float_label.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <QBuffer>
#include <QClipboard>
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
    , hotkey_(new QHotkey(QKeySequence("F10"), true, qApp))
    , high_(new CodeHighLightCode) {
    ui->setupUi(this);
    if(!hotkey_->isRegistered()) {
        spdlog::error("快捷键注册失败");
    }

    ui->btn_float->setDisabled(true);
    ui->btn_save->setDisabled(true);
    ui->btn_copy->setDisabled(true);

    auto r = high_->GetAvailableThemes();
    std::for_each(r.begin(), r.end(), [this](std::string const& item) {
        ui->cbox_themes->addItem(item.c_str());
    });
    if(r.size() == 0) {
        ui->lbl_themes->setMaximumSize(0, 0);
        ui->cbox_themes->setMaximumSize(0, 0);
    }

    ui->cbox_themes->setCurrentText("one-dark");

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

    connect(this, &MainWindow::signalPixmapReady, [this](QPixmap const& pixmap) {
        ui->lbl_img->setPixmap(pixmap);
        ui->btn_float->setEnabled(true);
        ui->btn_save->setEnabled(true);
        ui->btn_copy->setEnabled(true);
        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        pixmap.save(&buffer, "PNG");
        auto res = tesseract_->ImageFromMem(buffer.buffer().constData(), buffer.size());

        auto html = high_->ShaderCode(res.get(), ui->cbox_themes->currentText().toStdString());
        if(high_->IsAvailable()) {
            emit signalHtmlReady(html.c_str());
        } else {
            emit signalPlaintxtReady(html.c_str());
        }
    });

    connect(this, &MainWindow::signalHtmlReady, [this](QString const& html) {
        ui->browser_txt->setHtml(html);
    });
    connect(this, &MainWindow::signalPlaintxtReady, [this](QString const& txt) {
        ui->browser_txt->setPlainText(txt);
    });

    connect(ui->line_lang, &QLineEdit::textChanged, this, &MainWindow::slotHandleLineedit);

    ui->lbl_img->installEventFilter(this);

    connect(ui->btn_capture, &QPushButton::clicked, this, &MainWindow::slotCaptureScreen);
    connect(hotkey_, &QHotkey::activated, this, &MainWindow::slotCaptureScreen);
    connect(capture_, &ScreenCapture::signalScreenReady, [this](QPixmap const& pix) {
        emit signalPixmapReady(pix);
        showNormal();
        activateWindow();
    });

    connect(ui->btn_float, &QPushButton::clicked, [this]() {
        auto const& img = ui->lbl_img->pixmap(Qt::ReturnByValue);

        auto b = new FloatLabel;
        b->SetPixmap(img);
        b->show();
    });

    connect(ui->btn_save, &QPushButton::clicked, [this]() {
        auto file_name = QFileDialog::getSaveFileName();
        if(file_name.isEmpty()) return;

        ui->lbl_img->pixmap(Qt::ReturnByValue).save(file_name);
    });

    connect(ui->btn_copy, &QPushButton::clicked, [this]() {
        auto const& b = QApplication::clipboard();
        b->setPixmap(ui->lbl_img->pixmap(Qt::ReturnByValue), QClipboard::Clipboard);
    });
    connect(ui->btn_paste, &QPushButton::clicked, [this]() {
        auto const& b      = QApplication::clipboard();
        auto        pixmap = b->pixmap();
        spdlog::info("读取图片 {}", pixmap.isNull());
        if(pixmap.isNull()) {
            auto file_name = b->text();
            pixmap.load(file_name);
        }
        spdlog::info("读取图片2 {}", pixmap.isNull());

        if(pixmap.isNull()) return;
        signalPixmapReady(pixmap);
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

    emit signalPixmapReady(pix);

    return true;
}
void MainWindow::slotCaptureScreen() {
    if(ui->cbox_hidden->isChecked()) {
        showMinimized();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        capture_->capture();
    } else {
        capture_->capture();
    }
}
void MainWindow::slotHandleLineedit(QString const& text) {
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
}
