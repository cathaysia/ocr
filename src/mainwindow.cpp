#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ocr_tesseract.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <QAction>
#include <QBuffer>
#include <QFileDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>

#include <QHotkey>

#include "screen_capture.h"

// TODO: 绑定到快捷键
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

    // langs 按钮
    // TODO: 可以指定语言顺序
    {
        auto   langs = tesseract_->GetAvailableLangs();
        QMenu* menu  = new QMenu;
        for(auto const& lang: langs) {
            auto act = menu->addAction(lang.c_str(), [this, menu]() {
                auto                     acts = menu->actions();
                std::vector<std::string> activateLangs;
                for(auto const& act: acts) {
                    if(!act->isChecked()) continue;
                    activateLangs.push_back(act->text().toStdString());
                }
                tesseract_->LoadLangs(activateLangs);

                ui->btn_lang->setText(fmt::format("选择语言模型({})", fmt::join(activateLangs, ", ")).c_str());
            });
            act->setCheckable(true);
            menu->addAction(act);

            if(lang == "eng" || lang == "chi_sim" || lang == "osd") {
                act->setChecked(true);
            }
        }
        ui->btn_lang->setMenu(menu);
    }

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

        ui->browser_txt->setPlainText({ res.get() });
    });
    ui->btn_lang->setText("选择语言模型(eng, chi_sim, osd)");
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
