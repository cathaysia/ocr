#include "screen_capture.h"

#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>

ScreenCapture::ScreenCapture(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    hide();
}

void ScreenCapture::paintEvent(QPaintEvent* event) {
    QPainter painter;
    painter.begin(this);
    painter.setPen({ Qt::red, 2 });
    painter.drawPixmap(0, 0, pix_);

    if(endPoint_ != QPoint()) {
        painter.drawRect(startPoint_.x(), startPoint_.y(), endPoint_.x() - startPoint_.x(),
                         endPoint_.y() - startPoint_.y());
    }

    painter.end();
}

void ScreenCapture::mousePressEvent(QMouseEvent* e) {
    if(e->button() != Qt::LeftButton) return;

    startPoint_ = e->pos();
    update();
}

void ScreenCapture::mouseMoveEvent(QMouseEvent* e) {
    if(startPoint_ == QPoint()) return;

    endPoint_ = e->pos();
    update();
}

void ScreenCapture::mouseReleaseEvent(QMouseEvent* e) {
    pix_ =
        pix_.copy(startPoint_.x(), startPoint_.y(), endPoint_.x() - startPoint_.x(), endPoint_.y() - startPoint_.y());

    startPoint_ = {};
    endPoint_   = {};

    signalScreenReady(pix_);

    hide();
}
void ScreenCapture::capture() {
    auto screen = QGuiApplication::primaryScreen();
    pix_        = screen->grabWindow(0);

    showFullScreen();
}
