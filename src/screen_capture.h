#pragma once

#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QWidget>

class ScreenCapture : public QWidget {
    Q_OBJECT;

public:
    ScreenCapture(QWidget* parent = nullptr);

    void capture();

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

signals:
    void signalScreenReady(QPixmap const& pix);

private:
    QPixmap pix_;

    QPoint startPoint_;
    QPoint endPoint_;
};
