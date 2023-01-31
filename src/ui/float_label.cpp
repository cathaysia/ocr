#include "float_label.h"

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QTimer>

#include <chrono>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

FloatLabel::FloatLabel(QWidget* parent)
    : QWidget(parent)
    , menu_(new QMenu(this))
    , lbl_img_(new QLabel(this))
    , startPoint_(new QPoint)
    , timer_(new QTimer)
    , escPressed_(false) {

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    timer_->setSingleShot(true);

    auto hbox = new QHBoxLayout;
    this->setLayout(hbox);
    hbox->addWidget(lbl_img_);

    hbox->setMargin(1);
    setStyleSheet(R"(FloatLabel{margin: 1px; background-color: #f47920;})");

    auto act = menu_->addAction("关闭");
    connect(act, &QAction::triggered, [this]() {
        this->close();
    });

    auto top = menu_->addAction("置顶窗口");
    connect(top, &QAction::triggered, [this]() {
        setWindowFlag(Qt::WindowStaysOnTopHint, true);
        show();
    });

    auto notop = menu_->addAction("取消置顶");
    connect(notop, &QAction::triggered, [this]() {
        setWindowFlag(Qt::WindowStaysOnTopHint, false);
        show();
    });

    connect(this, &FloatLabel::signalRequestContextMenu, [this](QPoint const& pos) {
        menu_->exec(pos);
    });
}

FloatLabel::~FloatLabel() {
    delete menu_;
    menu_ = nullptr;

    delete startPoint_;
    startPoint_ = nullptr;
}
void FloatLabel::SetPixmap(QPixmap const& pixmap) {
    lbl_img_->setPixmap(pixmap);
}
void FloatLabel::mousePressEvent(QMouseEvent* e) {
    if(e->button() == Qt::RightButton) {
        timer_->start(std::chrono::milliseconds(300));
    } else if(e->button() == Qt::LeftButton) {
        *startPoint_ = e->pos();
    }
}

void FloatLabel::mouseReleaseEvent(QMouseEvent* e) {
    if(e->button() == Qt::RightButton) {
        if(!timer_->isActive()) return;

        emit signalRequestContextMenu(this->mapToGlobal(e->pos()));
        timer_->stop();
    } else if(e->button() == Qt::LeftButton) {
        *startPoint_ = {};
    }
}

void FloatLabel::mouseMoveEvent(QMouseEvent* e) {
    if(*startPoint_ != QPoint {}) {
        auto const& start = *startPoint_;
        auto const& end   = e->pos();

        this->move(this->mapToGlobal(end - start));
    }
}
void FloatLabel::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Escape) {
        escPressed_ = true;
        timer_->start(std::chrono::milliseconds(800));
    }
}

void FloatLabel::keyReleaseEvent(QKeyEvent* e) {
    if((e->key() == Qt::Key_Escape) && (timer_->isActive())) {
        timer_->stop();
        this->close();
    }
}
