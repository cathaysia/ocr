#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ocr_tesseract.h"

#include <QFileDialog>
#include <QLabel>

#include "screen_capture.h"
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , capture_(new ScreenCapture(nullptr))
    , tesseract_(new OcrTesseract("chi_sim+eng")) {
    ui->setupUi(this);

    ui->lbl_img->installEventFilter(this);

    connect(ui->btn_capture, &QPushButton::clicked, [this]() {
        capture_->capture();
    });

    connect(capture_, &ScreenCapture::signalScreenReady, [this](QPixmap const& pix) {
        ui->lbl_img->setPixmap(pix);
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
