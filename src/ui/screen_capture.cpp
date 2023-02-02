#include "screen_capture.h"

#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>

ScreenCapture::ScreenCapture(QWidget* parent) : QDialog(parent) {
    setMouseTracking(true);
    hide();
}

void ScreenCapture::paintEvent(QPaintEvent* event) {
    QPainter painter;
    painter.begin(this);
    painter.setPen({ Qt::red, 2 });

    QPixmap pix(pix_.size());
    pix.fill(QColor(0, 0, 0, 255 * 0.5));

    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Source);
    painter.drawPixmap(0, 0, pix_);

    if(endPoint_ != QPoint()) {
        QPainter p2(&pix);
        p2.setCompositionMode(QPainter::CompositionMode_Clear);
        p2.eraseRect(startPoint_.x(), startPoint_.y(), endPoint_.x() - startPoint_.x(),
                     endPoint_.y() - startPoint_.y());
        p2.end();
    }

    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
    painter.drawPixmap(0, 0, pix);

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

    hide();
    signalScreenReady(pix_);
}
void ScreenCapture::capture() {
    auto screen = QGuiApplication::primaryScreen();
    pix_        = screen->grabWindow(0);

    showFullScreen();
}
