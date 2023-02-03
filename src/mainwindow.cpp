#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "code_highlight.h"
#include "tesseract/ocr_tesseract.h"
#include "ui/float_label.h"

#include <chrono>
#include <future>
#include <iostream>
#include <map>
#include <regex>
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
#include <QList>
#include <QSettings>
#include <QSpinBox>
#include <QStyle>
#include <QStyleFactory>

#include <QHotkey>

#ifdef _WIN32
    #include <Kvantum.h>
#endif

#include "screen_capture.h"

#define DELETE_PTR(ptr) \
    if(ptr) {           \
        delete ptr;     \
        ptr = nullptr;  \
    }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings_(new QSettings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat))
    , capture_(new ScreenCapture(nullptr))
    , tesseract_(new OcrTesseract())
    , hotkey_(new QHotkey(QKeySequence(settings_->value("keyShortcut", "F10").toString()), true, qApp))
    , highlight_(new CodeHighLightCode)
#ifdef _WIN32
    , kvantum_(nullptr)
#endif
{
    ui->setupUi(this);
    if(!hotkey_->isRegistered()) {
        spdlog::error("快捷键注册失败");
    }

    ui->btn_float->setDisabled(true);
    ui->btn_save->setDisabled(true);
    ui->btn_copy->setDisabled(true);
    ui->lbl_img->installEventFilter(this);
    ui->cbox_hidden->setChecked(settings_->value("hiddenWhenTakeScreenshot", false).toBool());

    InitCodeHighLightWidget();
    InitTesseractLangsWidget();
    InitBasicFunctionButton();
    InitSettingPage();

    connect(hotkey_, &QHotkey::activated, this, &MainWindow::slotCaptureScreen);
    connect(ui->btn_capture, &QPushButton::clicked, this, &MainWindow::slotCaptureScreen);
    connect(capture_, &ScreenCapture::signalScreenReady, [this](QPixmap const& pix) {
        showNormal();
        activateWindow();
        emit signalPixmapReady(pix);
    });

    connect(this, &MainWindow::signalHtmlReady, ui->browser_txt, &QTextBrowser::setHtml);
    connect(this, &MainWindow::signalPlaintxtReady, [this](QString const& txt, bool requestHigh) {
        if(!requestHigh) {
            ui->browser_txt->setPlainText(txt);
            return;
        }

        auto html = highlight_->ShaderCode(txt.toStdString(), ui->cbox_themes->currentText().toStdString());

        do {    // 查找背景色
            auto p = html.begin() + html.find("background") + 12;
            if(p + 7 > html.end()) {
                break;
            }

            emit signalChangeBackgroundColor({ p, p + 7 });
        } while(0);

        signalHtmlReady(html.c_str());
    });

    connect(this, &MainWindow::signalChangeBackgroundColor, [this](std::string const& color) {
        spdlog::trace("设置 TxtBrowser 背景色为：'{}'", color);
        ui->browser_txt->setStyleSheet(fmt::format("background-color: {};", color).c_str());
    });

    connect(this, &MainWindow::signalPixmapReady, [this](QPixmap const& pixmap) {
        emit signalPlaintxtReady("获取 OCR 结果中......");

        ui->lbl_img->setPixmap(pixmap);
        ui->btn_float->setEnabled(true);
        ui->btn_save->setEnabled(true);
        ui->btn_copy->setEnabled(true);

        QBuffer buffer;
        buffer.open(QIODevice::ReadWrite);
        auto ret = pixmap.save(&buffer, "PNG");
        if(!ret) {
            spdlog::error("保存图片到缓冲区失败");
            return;
        }

        auto fut = std::async(std::launch::async, [this, &buffer]() {
            return tesseract_->ImageFromMem(buffer.buffer().constData(), buffer.size());
        });

        std::shared_ptr<char> res;
        while(true) {
            auto r = fut.wait_for(std::chrono::milliseconds(40));    /// 1000/40 = 25 fps
            if(r == std::future_status::timeout) {
                qApp->processEvents();
                continue;
            }

            res = fut.get();
            break;
        }

        emit signalPlaintxtReady(res.get(), highlight_->IsAvailable());
    });
}

MainWindow::~MainWindow() {
    DELETE_PTR(settings_);
    DELETE_PTR(capture_);
    DELETE_PTR(tesseract_);
    DELETE_PTR(hotkey_);
    DELETE_PTR(highlight_);

#ifdef _WIN32
    DELETE_PTR(kvantum_);
#endif
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
    if(!ui->cbox_hidden->isChecked()) {
        capture_->capture();
        return;
    }

    showMinimized();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    capture_->capture();
}
void MainWindow::slotReloadLangs(QString const& text) {
    auto langs = tesseract_->GetAvailableLangs();
    auto res   = std::vector<std::string>();
    auto hashK = std::map<std::string, bool>();

    for(auto& v: text.split(",")) {
        v = v.trimmed();
        if(v.isEmpty()) continue;
        qDebug() << v;
        if(std::find(langs.begin(), langs.end(), v.toStdString()) == langs.end()) {
            spdlog::info("{} 不在支持的语言列表中，忽略", v.toStdString());
            ui->line_lang->setStyleSheet("color: red;");
            return;
        }
        if(hashK.count(v.toStdString()) == 0) {
            res.push_back(v.toStdString());
            hashK[v.toStdString()] = true;
        }
    }
    ui->line_lang->setStyleSheet("");

    if(tesseract_->GetUsedLangs() == res) return;

    tesseract_->LoadLangs(res);
    settings_->setValue("tesseractLangs", fmt::format("{}", fmt::join(res, ", ")).c_str());
}

void MainWindow::InitSettingPage() {

#ifdef _WIN32
    QDir path(QApplication::applicationDirPath() + "/themes/Kvantum/");
    ui->cbox_style->addItems(path.entryList(QDir::Dirs | QDir::NoDot | QDir::NoDotDot));
    connect(ui->cbox_style, &QComboBox::currentTextChanged, [this](QString const& value) {
        spdlog::info("设置 Kvantum 主题：{}", value.toStdString());
        QDir path(QApplication::applicationDirPath() + "/themes/");
        auto conf_file  = QString("%1/Kvantum/%2/%2.kvconfig").arg(path.absolutePath()).arg(value);
        auto svg_file   = QString("%1/Kvantum/%2/%2.svg").arg(path.absolutePath()).arg(value);
        auto color_file = QString("%1/colors/%2.colors").arg(path.absolutePath()).arg(value);
        spdlog::info("主题文件为：{}\n{}\n{}\n", conf_file.toStdString(), svg_file.toStdString(),
                     color_file.toStdString());

        std::once_flag kvantum_flag;
        std::call_once(
            kvantum_flag,
            [](Kvantum::Style** style) {
                *style = new Kvantum::Style();
                qApp->setStyle(*style);
            },
            &kvantum_);
        kvantum_->unpolish(qApp);
        kvantum_->setTheme(conf_file, svg_file, color_file);
        qApp->setPalette(kvantum_->standardPalette());
        settings_->setValue("kvantumTheme", value);
    });
    ui->cbox_style->setCurrentText(settings_->value("kvantumTheme", "KvYaru").toString());
#else
    // clang-format off
    static std::map<std::string, const char*> classMap {
        { "highcontrastinverse", "HighContrastInverse" },
        { "highcontrast", "HighContrast" },
        { "adwaita-highcontrastinverse", "Adwaita-HighContrastInverse" },
        { "adwaita-highcontrast", "Adwaita-HighContrast" },
        { "adwaita-dark", "Adwaita-Dark" },
        { "adwaita", "Adwaita" },
        { "windows", "Windows" },
        { "fusion", "Fusion" }
    };
    // clang-format on

    auto styles = QStyleFactory::keys();
    ui->cbox_style->addItems(styles);
    ui->cbox_style->setCurrentText(classMap[qApp->style()->objectName().toStdString()]);
    spdlog::info("当前 Style 为：{}", qApp->style()->objectName().toStdString());

    connect(ui->cbox_style, &QComboBox::currentTextChanged, [this](QString const& value) {
        qApp->setStyle(QStyleFactory::create(value));
    });
#endif

    connect(ui->cbox_fontFamily, &QFontComboBox::currentTextChanged, [this](QString const& fontFamily) {
        QFont f = qApp->font();
        f.setFamily(fontFamily);
        qApp->setFont(f);
        settings_->setValue("fontFamily", f.family());
    });

    connect(ui->spain_fontSize, QOverload<int>::of(&QSpinBox::valueChanged), [this](int fontSize) {
        QFont f = qApp->font();
        f.setPointSize(fontSize);
        qApp->setFont(f);
        settings_->setValue("fontSize", f.pointSize());
    });

    connect(ui->cbox_editor_fontFamily, &QFontComboBox::currentTextChanged, [this](QString const& fontFamily) {
        QFont f = ui->browser_txt->font();
        f.setFamily(fontFamily);
        ui->browser_txt->setFont(f);
        settings_->setValue("editorFontFamily", f.family());
    });

    connect(ui->spain_editor_fontSize, QOverload<int>::of(&QSpinBox::valueChanged), [this](int fontSize) {
        QFont f = ui->browser_txt->font();
        f.setPointSize(fontSize);
        ui->browser_txt->setFont(f);
        settings_->setValue("editorFontSize", f.pointSize());
    });

    QFont f = qApp->font();
    ui->cbox_fontFamily->setCurrentText(settings_->value("fontFamily", f.family()).toString());
    ui->cbox_editor_fontFamily->setCurrentFont(settings_->value("editorFontFamily", f.family()).toString());
    ui->spain_fontSize->setValue(settings_->value("fontSize", f.pointSize()).toInt());
    ui->spain_editor_fontSize->setValue(settings_->value("editorFontSize", f.pointSize()).toInt());
}

void MainWindow::InitCodeHighLightWidget() {
    auto r = highlight_->GetAvailableThemes();
    if(r.empty()) {
        ui->lbl_themes->setMaximumSize(0, 0);
        ui->cbox_themes->setMaximumSize(0, 0);
        return;
    }
    for(auto const& item: r) {
        ui->cbox_themes->addItem(item.c_str());
    }

    ui->cbox_themes->setCurrentText(settings_->value("editorHighTheme", "one-dark").toString());
    connect(ui->cbox_themes, &QComboBox::currentTextChanged, [this](QString const& value) {
        settings_->setValue("editorHighTheme", value);
        emit signalPlaintxtReady(ui->browser_txt->toPlainText(), highlight_->IsAvailable());
    });
}

void MainWindow::InitTesseractLangsWidget() {
    connect(ui->line_lang, &QLineEdit::textChanged, this, &MainWindow::slotReloadLangs);

    auto s = fmt::format("{}", fmt::join(tesseract_->GetUsedLangs(), ", "));
    ui->line_lang->setText(settings_->value("tesseractLangs", s.c_str()).toString());

    auto        langs = tesseract_->GetAvailableLangs();
    QStringList qlangs;
    qlangs.reserve(langs.size());
    std::transform(langs.begin(), langs.end(), std::back_inserter(qlangs), [](const std::string& v) {
        return QString::fromStdString(v);
    });
    ui->line_lang->SetCmpList(qlangs);
};

void MainWindow::InitBasicFunctionButton() {
    connect(ui->btn_float, &QPushButton::clicked, [this]() {
        auto const& img = ui->lbl_img->pixmap(Qt::ReturnByValue);

        auto b = new FloatLabel;
        b->SetPixmap(img);
        b->show();
    });
    connect(ui->cbox_hidden, &QCheckBox::clicked, [this](bool checked) {
        settings_->setValue("hiddenWhenTakeScreenshot", checked);
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
};
