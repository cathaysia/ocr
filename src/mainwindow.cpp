#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ocr_tesseract.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QTemporaryFile>

#include "screen_capture.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , capture_(new ScreenCapture(nullptr))
    , tesseract_(new OcrTesseract()) {
    ui->setupUi(this);

    // langs 按钮
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

    connect(ui->btn_capture, &QPushButton::clicked, [this]() {
        if(ui->cbox_hidden->isChecked()) {
            showMinimized();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            capture_->capture();
            showNormal();
            activateWindow();
        } else {
            capture_->capture();
        }
    });

    connect(capture_, &ScreenCapture::signalScreenReady, [this](QPixmap const& pix) {
        ui->lbl_img->setPixmap(pix);

        QTemporaryFile tmpfile("XXXXXX.png");
        if(!tmpfile.open()) {
            return;
        }

        pix.save(&tmpfile);
        tmpfile.close();

        auto res = tesseract_->Image(tmpfile.fileName().toStdString());
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
